#pragma once

#include <functional>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>
#include <thread>

namespace hc {
namespace thread {
    
    class thread_pool {
        public:
            thread_pool()
                : m_should_terminate(false)
            {}

            ~thread_pool() {}

            void start(std::size_t num_threads);
            void stop();

            void add_job(std::function<void()> job);

        private:
            void worker_loop();

            std::mutex m_mutex;
            std::condition_variable m_cv;

            std::vector<std::thread> m_threads;
            std::queue<std::function<void()>> m_job_queue;

            bool m_should_terminate;
    };

}
}