#include <string_view>
#include <ctb/namedtuple.hh>

using namespace ctb::namedtuple;

consteval void test_names() noexcept {
    static_assert(details::is_names<names<"a", "blabla">> == true);
    static_assert(details::is_names<int> == false);
    static_assert(details::get_name<1, names<"a", "blabla">>() == "blabla");
    static_assert(details::get_name<0, names<u8"滑稽", "bla">>() == u8"滑稽");
    static_assert(details::get_size<names<"a", "blabla">>() == 2);
}

consteval void test_namedtuple() noexcept {
    constexpr auto nt = make_namedtuple<u8"hhh", "blaa">(1, u8"233hh");
    static_assert(::std::u8string_view{get<"blaa">(nt)} == u8"233hh");
    static_assert(get<0>(nt) == 1);
    [[maybe_unused]] auto [a, b]{nt};
}

int main() noexcept {
    return 0;
}
