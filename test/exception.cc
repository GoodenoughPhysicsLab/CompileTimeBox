#include <cassert>
#include <ctb/exception.hh>

using namespace ctb::exception;

struct NoDefaultConstructor_ {
    constexpr NoDefaultConstructor_() noexcept = delete;
};

consteval void test_optional() noexcept {
    constexpr auto x = Optional<int>{1};
    constexpr auto y = Optional<int>{nullopt};
    static_assert(x.has_value() == true);
    static_assert(y.has_value() == false);
    static_assert(x.value() == 1);
    static_assert(y.value_or(1) == 1);
    [[maybe_unused]] constexpr auto _7 = Optional<NoDefaultConstructor_>{nullopt};
    static_assert(_7.has_value() == false);
}

inline void runtime_tests() noexcept {
    auto x = Optional<int>{1};
    x.value() = 2;
    assert(x.value() == 2);
}

int main() noexcept {
    runtime_tests();
    return 0;
}
