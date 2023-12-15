#pragma once

#include "program.h"

#include <alsa/asoundlib.h>
#include <fftw3.h>

class rgb_lights;

class guitar_sync_program : public program {
    public:
        guitar_sync_program() 
            : m_init(false)    
        {}

        ~guitar_sync_program() {}

        void on_start() override;
        void loop() override;
        void on_interrupt() override;
        void on_stop() override;

    private:
        static const int BUFFER_SIZE = 128;
        static const int NUM_CHANNELS = 2;

        struct channel_data {
            double* m_in;
            fftw_complex* m_out;


            fftw_plan m_p;

            double m_mag[BUFFER_SIZE / 2 - 1];
            double m_db[BUFFER_SIZE / 2 - 1];
        } m_channels[NUM_CHANNELS];

        bool m_init;

        snd_pcm_t* m_capture_handle;
        std::size_t m_format_width;
};