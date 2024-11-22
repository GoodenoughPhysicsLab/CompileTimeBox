#include <cstdint>
#include <ctb/vector.hh>

using namespace ctb::vector;

consteval void test_init() noexcept {
    [[maybe_unused]] constexpr auto _1 = vector{{1u, 2u}};
    [[maybe_unused]] constexpr auto _2{_1};
}

consteval void test_eq() noexcept {
    constexpr auto _1 = vector{{1u, 2u}};
    constexpr auto _2 = vector{1u, 2u};
    constexpr auto _3 = vector{1, 2};
    constexpr uint32_t _4[]{1u, 2u};
    static_assert(_1 == _2);
    static_assert(_1 == _4);
    static_assert(_1 != _3);
}

int main() noexcept {
    return 0;
}
