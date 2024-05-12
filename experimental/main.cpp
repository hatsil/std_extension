#include "std_extention/exception.hpp"
#include "std_extention/executor.hpp"

#include <iostream>
#include <mutex>
#include <thread>
#include <chrono>
#include <vector>
#include <stdexcept>

using namespace std::chrono_literals;

void foo(const std::string &str) {
    throw ext::exception(str);
}

void bar(int i) {
    static std::mutex mutex;
    {
        std::lock_guard guard(mutex);
        std::cout << "thread " << std::this_thread::get_id() << ": " << i << std::endl;
    }
    std::this_thread::sleep_for(1s);
}

int baz(int i) {
    bar(i);
    return i;
}

void bug1(int i) {
    if (i > 10) {
        throw ext::exception("i > 10");
    }
    bar(i);
}

void bug2(int i) {
    if (i > 10) {
        throw std::runtime_error("i > 10");
    }
    bar(i);
}

void bug3(int i) {
    if (i > 10) {
        throw i;
    }
    bar(i);
}

int main() {
    std::cout << "Hello, CMake!" << std::endl;
    try {
        foo("foo throws");
    } catch (const ext::exception &e) {
        std::cout << e << std::endl;
    }

    {
        ext::executor executor(10);
        for (int i = 0; i < 20; i++) {
            executor.emplace_back(bar, i+1);
        }
        executor.forced_shutdown();
    }

    std::cout << std::endl;

    try {
        ext::executor executor(0);
    } catch (const ext::executor_exception &e) {
        std::cout << e << std::endl;
    }

    {
        std::vector<std::future<void>> futures;
        ext::executor executor(10);
        for (int i = 20; i < 40; i++) {
            futures.push_back(executor.emplace_back(bar, i+1));
        }
        executor.shutdown();
        for (auto &&future : futures) {
            future.get();
        }
    }

    {
        std::vector<std::future<int>> futures;
        ext::executor executor(10);
        for (int i = 20; i < 40; i++) {
            futures.push_back(executor.emplace_back(baz, i+1));
        }

        executor.shutdown();
        for (auto &&future : futures) {
            std::cout << future.get() << std::endl;
        }
    }

    for (auto bug : {bug1, bug2, bug3}) {
        std::vector<std::future<void>> futures;
        ext::executor executor(10);

        for (int i = 0; i < 20; i++) {
            futures.push_back(executor.emplace_back(bug, i+1));
        }

        for (auto &&future : futures) try {
            future.get();
        } catch(const ext::exception &e) {
            std::cout << "ext::exception:\n" << e << std::endl;
        } catch(const std::exception &e) {
            std::cout << "std::exception:\n" << e.what() << std::endl;
        } catch(const int &e) {
            std::cout << "int thrown: " << e << std::endl;
        }

        executor.shutdown();
    }

    return 0;
}
