#include <cstdio>
#include <exception>
#include <cstdlib>
#include <cstdint>

void foo(const char *what);

class custom_exception {
public:
    custom_exception(const char *what_str) noexcept;
    custom_exception(const custom_exception &) noexcept = default;
    custom_exception(custom_exception &&) noexcept = default;

    custom_exception &operator=(const custom_exception &) noexcept = default;
    custom_exception &operator=(custom_exception &&) noexcept = default;

    const char *what() const noexcept;
private:
    const char *m_what;
};

int main () {
    try {
        foo("calls foo from main");
    } catch (const custom_exception &e) {
        std::printf("ERROR: %s\n", e.what());
    }
}

void foo(const char *what) {
    throw custom_exception(what);
}

custom_exception::custom_exception(const char *what_str) noexcept : m_what(what_str) {}

const char *custom_exception::what() const noexcept {
    return m_what;
}
