#pragma once

#include "std_extension/exception.hpp"

#include <memory>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <unordered_map>

namespace ext {
class condition_variable;

class thread final {
public:
    thread();

    template <class F, class... Args> explicit thread(F &&f, Args &&...args);

    thread(thread &&moved) noexcept            = default;
    thread &operator=(thread &&moved) noexcept = default;

    thread(const thread &)            = delete;
    thread &operator=(const thread &) = delete;

    ~thread();

    // members
    void                            swap(thread &other) noexcept;
    bool                            joinable() const noexcept;
    void                            join();
    void                            detach();
    std::thread::id                 get_id() const noexcept;
    std::thread::native_handle_type native_handle();

    void interrupt();

private:
    friend class condition_variable;

    struct Spore {
        Spore() noexcept;

        template <class F, class... Args> Spore(F &&f, Args &&...args);

        bool                m_interrupted;
        condition_variable *m_cv;
        std::mutex         *m_cv_mutex;
        std::thread         m_thread;
        std::mutex          m_mutex;
    };

    static std::unordered_map<std::thread::id, std::shared_ptr<Spore>> &get_threads() noexcept;
    static std::shared_mutex                                           &get_mutex() noexcept;
    static std::shared_ptr<Spore>                                       get_spore();

    std::shared_ptr<Spore> m_spore;
};
} // namespace ext
