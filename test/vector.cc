#ifdef NDEBUG
    #undef NDEBUG
#endif

#include <cassert>
#include <cstdint>
#include <ctb/vector.hh>

using namespace ctb::vector;

consteval void test_init() noexcept {
    [[maybe_unused]] constexpr auto _1 = vector{{1u, 2u}};
    [[maybe_unused]] constexpr auto _2 = vector{1u, 2u};
    [[maybe_unused]] constexpr auto _3{_1};
    int arr[]{1u, 2u};
    [[maybe_unused]] auto _4 = vector{arr};
    [[maybe_unused]] constexpr vector<int, 2> _5{};
    [[maybe_unused]] constexpr vector<::std::size_t, 4> _6{1, 2};
}

consteval void test_eq() noexcept {
    constexpr auto _1 = vector{{1u, 2u}};
    constexpr auto _2 = vector{_1};
    constexpr auto _3 = vector{1, 2};
    constexpr uint32_t _4[]{1u, 2u};
    static_assert(_1 == _2);
    static_assert(_1 == _4);
    static_assert(_1 != _3);
}

consteval void test_index() noexcept {
    constexpr auto _1 = vector{1u, 2u, 3u};
    static_assert(get_value(_1, 0) == 1u);
    static_assert(get_value(_1, 1) == 2u);
    static_assert(get_value(_1, 2) == 3u);
}

consteval void test_slice() noexcept {
    constexpr auto _1 = vector{1u, 2u, 3u, 4u, 5u};
    static_assert(slice<0, 3>(_1) == vector{1u, 2u, 3u});
    static_assert(slice<2, 4>(_1) == vector{3u, 4u});
    static_assert(slice<2, -1>(_1) == vector{3u, 4u});
}

consteval void test_data() noexcept {
    constexpr auto _1 = vector{1u, 2u, 3u};
    static_assert(_1.data()[0] == 1u);
}

consteval void test_size() noexcept {
    constexpr auto _1 = vector{1u, 2u, 3u};
    static_assert(_1.size() == 3u);
}

inline void runtime_test_iter() noexcept {
    constexpr auto _1 = vector{1u, 2u, 3u};
    static_assert(*_1.begin() == 1u);
    static_assert(*(_1.end() - 1) == 3u);
    for (auto i : _1) {
        assert(i == 1u || i == 2u || i == 3u);
    }
}

int main() noexcept {
    runtime_test_iter();

    return 0;
}
