#include "homecontroller/thread/thread_pool.h"

#include "homecontroller/util/logger.h"

#include <sstream>

namespace hc {
namespace thread {

    void thread_pool::start(std::size_t num_threads) {
        m_threads.resize(num_threads);

        for (std::size_t i = 0; i < num_threads; i++) {
            m_threads.at(i) = std::thread(&thread_pool::worker_loop, this);
        }

        util::logger::dbg("created " + std::to_string(num_threads) + " threads");
    }

    void thread_pool::stop() {
        std::unique_lock<std::mutex> lock(m_mutex);

        m_should_terminate = true;

        lock.unlock();
        m_cv.notify_all();

        for (auto& t : m_threads) {
            t.join();
        }

        util::logger::dbg(std::to_string(m_threads.size()) + " threads exited");
        m_threads.clear();
    }

    void thread_pool::add_job(std::function<void()> job) {
        std::unique_lock<std::mutex> lock(m_mutex);

        m_job_queue.push(job);
        m_cv.notify_one();
    }

    void thread_pool::worker_loop() {
        std::ostringstream id_ss;
        id_ss << std::this_thread::get_id();

        std::string thread_id = id_ss.str();

        while(true) {
            std::unique_lock<std::mutex> lock(m_mutex);

            m_cv.wait(lock, [this]() {
                return !m_job_queue.empty() || m_should_terminate;
            });

            if (m_should_terminate) return;

            std::function<void()> job = m_job_queue.front();
            m_job_queue.pop();

            lock.unlock();

            std::chrono::time_point<std::chrono::system_clock> start_time = std::chrono::system_clock::now();

            util::logger::dbg("thread [" + thread_id + "] started job");
            job();
            util::logger::dbg("thread [" + thread_id + "] finished job");

            std::chrono::time_point<std::chrono::system_clock> curr_time = std::chrono::system_clock::now();
            util::logger::dbg("execution time: " + std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(curr_time - start_time).count() / 1000000) + "ms");

        }
    }

}
}