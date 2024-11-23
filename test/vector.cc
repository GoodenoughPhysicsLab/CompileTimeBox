#include <cstdint>
#include <ctb/vector.hh>

using namespace ctb::vector;

consteval void test_init() noexcept {
    [[maybe_unused]] constexpr auto _1 = Vector{{1u, 2u}};
    [[maybe_unused]] constexpr auto _2 = Vector{1u, 2u};
    [[maybe_unused]] constexpr auto _3{_1};
}

consteval void test_eq() noexcept {
    constexpr auto _1 = Vector{{1u, 2u}};
    constexpr auto _2 = Vector{_1};
    constexpr auto _3 = Vector{1, 2};
    constexpr uint32_t _4[]{1u, 2u};
    static_assert(_1 == _2);
    static_assert(_1 == _4);
    static_assert(_1 != _3);
}

consteval void test_index() noexcept {
    constexpr auto _1 = Vector{1u, 2u, 3u};
    static_assert(_1[0] == 1u);
    static_assert(_1[1] == 2u);
    static_assert(_1[2] == 3u);
}

consteval void test_slice() noexcept {
    constexpr auto _1 = Vector{1u, 2u, 3u, 4u, 5u};
    static_assert(_1.slice<0, 3>() == Vector{1u, 2u, 3u});
    static_assert(_1.slice<2, 4>() == Vector{3u, 4u});
    static_assert(_1.slice<2, -1>() == Vector{3u, 4u});
}

consteval void test_data() noexcept {
    constexpr auto _1 = Vector{1u, 2u, 3u};
    static_assert(_1.data()[0] == 1u);
}

int main() noexcept {
    return 0;
}
