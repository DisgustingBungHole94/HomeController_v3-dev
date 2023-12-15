#include "guitar_sync_program.h"

#include <homecontroller/util/logger.h>

#include <vector>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <ccomplex>
#include <fftw3.h>

void guitar_sync_program::on_start() {
    static const std::string DEVICE_NAME = "hw:1,0";
    unsigned int rate = 44100;
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

    if ((res = snd_pcm_hw_params_set_channels(m_capture_handle, hw_params, NUM_CHANNELS)) < 0) {
        hc::util::logger::err("guitar sync: failed to set channel count");
        return;
    }

    if ((res = snd_pcm_hw_params_set_periods(m_capture_handle, hw_params, 2, 0)) < 0) {
        hc::util::logger::err("guitar sync: failed to set period count");
        return;
    }

    if ((res = snd_pcm_hw_params_set_buffer_size(m_capture_handle, hw_params, (8192 * 2) >> 2)) < 0) {
        hc::util::logger::err("guitar sync: failed to set buffer size");
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

    for (int i = 0; i < NUM_CHANNELS; i++) {
        m_channels[i].m_in = (double*)fftw_malloc(sizeof(double) * BUFFER_SIZE);
        if (!m_channels[i].m_in) {
            hc::util::logger::err("guitar sync: failed to alloc fft in buffer");
            return;
        }

        m_channels[i].m_out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * ((BUFFER_SIZE / 2) + 1));
        if (!m_channels[i].m_out) {
            hc::util::logger::err("guitar sync: failed to alloc fft out buffer");
            return;
        }

        m_channels[i].m_p = fftw_plan_dft_r2c_1d(BUFFER_SIZE, m_channels[i].m_in, m_channels[i].m_out, FFTW_ESTIMATE);
        if (!m_channels[i].m_p) {
            hc::util::logger::err("guitar sync: failed to alloc fft p");
            return;
        }
    }

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

    for (int i = 0; i < NUM_CHANNELS; i++) {
        for (int j = 0; j < NUM_FRAMES / sizeof(buffer[0]) / NUM_CHANNELS; j++) {
            m_channels[i].m_in[j] = buffer[j * NUM_CHANNELS + i];
        }

        fftw_execute(m_channels[i].m_p);
    }

    for (int i = 0; i < NUM_CHANNELS; i++) {
        for (int j = 0; j < BUFFER_SIZE / 2 - 1; j++) {
            m_channels[i].m_mag[j] = std::sqrt((m_channels[i].m_out[j][0] * m_channels[i].m_out[j][0]) + m_channels[i].m_out[j][1] * m_channels[i].m_out[j][1]);
            m_channels[i].m_db[j] = (10.0f * log10(m_channels[i].m_mag[j] + 1.0));
        }
    }

    /*float avg = 0.0f;
    for (int j = 0; j < BUFFER_SIZE / 2 - 1; j++) {
        avg += m_channels[1].m_db[j];
    }
    avg /= BUFFER_SIZE / 2 - 1;*/

    std::cout << "***" << std::endl;
    std::cout << m_channels[1].m_mag[0] << std::endl;
    std::cout << m_channels[1].m_mag[10] << std::endl;
    std::cout << m_channels[1].m_mag[20] << std::endl;
    std::cout << m_channels[1].m_mag[30] << std::endl;
    std::cout << m_channels[1].m_mag[40] << std::endl;
    std::cout << m_channels[1].m_mag[50] << std::endl;
    std::cout << "***" << std::endl;
}

void guitar_sync_program::on_interrupt() {}

void guitar_sync_program::on_stop() {
    if (!m_init) {
        return;
    }

    snd_pcm_close(m_capture_handle);
}