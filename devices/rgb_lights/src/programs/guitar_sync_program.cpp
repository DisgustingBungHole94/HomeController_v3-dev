#include "guitar_sync_program.h"

#include <homecontroller/util/logger.h>

#include <vector>
#include <iostream>

void guitar_sync_program::on_start() {
    static const std::string DEVICE_NAME = "hw:1,0";
    unsigned int rate = 44100;
    unsigned int num_channels = 2;
    snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;

    int res;

    if ((res = snd_pcm_open(&m_capture_handle, DEVICE_NAME.c_str(), SND_PCM_STREAM_CAPTURE, 0)) < 0) {
        hc::util::logger::err("guitar sync: failed to open audio device!");
        return;
    }

    hc::util::logger::log("guitar sync: opened audio device!");

    snd_pcm_hw_params_t* hw_params;

    if ((res = snd_pcm_hw_params_malloc(&hw_params)) < 0) {
        hc::util::logger::err("guitar sync: failed to allocate hardware param struct");
        return;
    }

    if ((res = snd_pcm_hw_params_any(m_capture_handle, hw_params)) < 0) {
        hc::util::logger::err("guitar sync: failed to init hardware params struct");
        return;
    }

    if ((res = snd_pcm_hw_params_set_access(m_capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
        hc::util::logger::err("guitar sync: failed to set access type");
        return;
    }

    if ((res = snd_pcm_hw_params_set_format(m_capture_handle, hw_params, format)) < 0) {
        hc::util::logger::err("guitar sync: failed to set sample format");
        return;
    }

    m_format_width = snd_pcm_format_width(format);

    if ((res = snd_pcm_hw_params_set_rate_near(m_capture_handle, hw_params, &rate, 0)) < 0) {
        hc::util::logger::err("guitar sync: failed to set sample rate");
        return;
    }

    if ((res = snd_pcm_hw_params_set_channels(m_capture_handle, hw_params, num_channels)) < 0) {
        hc::util::logger::err("guitar sync: failed to set channel count");
        return;
    }

    if ((res = snd_pcm_hw_params(m_capture_handle, hw_params)) < 0) {
        hc::util::logger::err("guitar sync: failed to set hardware params");
        return;
    }

    snd_pcm_hw_params_free(hw_params);

    if ((res = snd_pcm_prepare(m_capture_handle)) < 0) {
        hc::util::logger::err("guitar sync: failed to prepare audio interface for use");
        return;
    }

    hc::util::logger::log("guitar sync: audio device is ready!");

    m_init = true;
}

void guitar_sync_program::loop() {
    if (!m_init) {
        return;
    }

    const std::size_t NUM_FRAMES = 128;

    std::vector<char> buffer;
    buffer.resize(NUM_FRAMES * m_format_width / 8 * 2);

    if (snd_pcm_readi(m_capture_handle, &buffer[0], NUM_FRAMES) != NUM_FRAMES) {
        hc::util::logger::err("guitar sync: read from audio device failed");
        return;
    }

    int avg = 0;
    for (std::size_t i = 0; i < buffer.size(); i++) {
        avg += buffer[i];
    }
    avg /= buffer.size();

    std::cout << avg << std::endl;
}

void guitar_sync_program::on_interrupt() {}

void guitar_sync_program::on_stop() {
    if (!m_init) {
        return;
    }

    snd_pcm_close(m_capture_handle);
}