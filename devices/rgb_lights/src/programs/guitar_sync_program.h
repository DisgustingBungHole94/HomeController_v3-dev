#pragma once

#include "program.h"

#include <alsa/asoundlib.h>

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
        bool m_init;

        snd_pcm_t* m_capture_handle;
        std::size_t m_format_width;
};