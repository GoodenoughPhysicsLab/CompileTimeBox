#include <ctb/exception.hh>
#include <ctb/tuple.hh>

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
#endif

using namespace ctb::tuple;

consteval void test_init() noexcept {
    constexpr tuple t{1, 2};
    static_assert(get<0>(t) == 1);
    static_assert(get<1>(tuple{1, 2}) == 2);
    static_assert(is_tuple<decltype(t)>);
    static_assert(!is_tuple<int>);
}

inline void test_structured_binding() noexcept {
    ctb::tuple::tuple t{1, 2};
    auto const& [a, b] = t;
    ctb::exception::assert_true(a == 1);
    ctb::exception::assert_true(b == 2);
}

int main() noexcept {
    test_structured_binding();
    return 0;
}
