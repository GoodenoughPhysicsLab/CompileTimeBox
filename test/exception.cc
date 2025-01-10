#include <ctb/exception.hh>

using namespace ctb::exception;

consteval void test_optional() noexcept {
    constexpr auto x = Optional<int>{1};
    constexpr auto y = Optional<int>{nullopt};
    static_assert(x.has_value() == true);
    static_assert(y.has_value() == false);
    static_assert(x.value() == 1);
    static_assert(y.value_or(1) == 1);
}

int main() noexcept {
    return 0;
}
