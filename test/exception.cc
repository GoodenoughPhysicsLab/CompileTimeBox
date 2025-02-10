#include <cmath>
#ifdef NDEBUG
    #undef NDEBUG
#endif

#include <cassert>
#include <ctb/exception.hh>

using namespace ctb::exception;

struct NoDefaultConstructor_ {
    int n_;
    constexpr NoDefaultConstructor_() noexcept = delete;
    constexpr NoDefaultConstructor_(int n) : n_{n} {}
};

consteval void test_optional() noexcept {
    constexpr auto x = optional<int>{1};
    constexpr auto y = optional<int>{nullopt};
    static_assert(has_value(x) == true);
    static_assert(has_value(y) == false);
    static_assert(get_value(x) == 1);
    static_assert(value_or(y, 1) == 1);
    constexpr auto _7 = optional<NoDefaultConstructor_>{nullopt};
    static_assert(has_value(_7) == false);
}

consteval void test_expected() noexcept {
    constexpr int num{};
    constexpr auto x = expected<int, int>{1};
    constexpr auto y = expected<int, int>{ctb::exception::unexpected{1}};
    constexpr auto z = expected<int, int>{num};
    static_assert(has_value(x) == true);
    static_assert(has_value(y) == false);
    static_assert(has_value(z) == true);
    static_assert(get_value(x) == 1);
    static_assert(get_error(y) == 1);
    static_assert(value_or(x, 2) == 1);
    static_assert(value_or(y, 2) == 2);
    // static_assert(value_or(y, 2.5) == 2); // error, implicit conversion is not allowed
}

inline void test_optional_in_runtime() noexcept {
    auto x = optional<int>{1};
    x = 2;
    assert(get_value(x) == 2);
    x.reset();
    assert(has_value(x) == false);
    x.emplace(5);
    assert(get_value(x) == 5);
}

inline void test_expected_in_runtime() noexcept {
    auto x = expected<int, int>{1};
    x = 2;
    assert(get_value(x) == 2);
    auto y = expected<NoDefaultConstructor_, int>{ctb::exception::unexpected{0}};
    y = NoDefaultConstructor_{1};
    assert(get_value(y).n_ == 1);
}

int main() noexcept {
    test_optional_in_runtime();
    test_expected_in_runtime();
    return 0;
}
