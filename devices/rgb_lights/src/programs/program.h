#pragma once

#include <string>

class rgb_lights;

class program {
    public:
        program() {}
        ~program() {}

        void run();
        void interrupt(const std::string& data);

        void pause();
        void unpause();
        void stop();

        virtual void on_start();
        virtual void loop();
        virtual void on_interrupt();
        virtual void on_stop();

        void set_app(rgb_lights* app) { m_app = app; }

        void set_speed(float speed) { m_speed = speed; }
        float get_speed() { return m_speed; }

    protected:
        rgb_lights* m_app;

        float m_speed;

        std::string m_interrupt_data;

    private:
        enum class program_state {
            RUNNING, INTERRUPT, PAUSED, STOPPED
        } m_state;

};