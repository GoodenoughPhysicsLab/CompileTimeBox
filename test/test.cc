#include "../include/namedtuple.hh"
#include <string_view>

using namespace namedtuple;

void test_template_str() noexcept {
    static_assert(::std::string_view{"abc"} == details::template_str{"abc"});
    static_assert(::std::u8string_view{u8"abc"} == details::template_str{u8"abc"});
}

void test_names() noexcept {
    static_assert(details::names::is_names<details::names::names<"a", "blabla">> == true);
    static_assert(details::names::is_names<int> == false);
    static_assert(details::names::get_name<1, details::names::names<"a", "blabla">>() == "blabla");
    static_assert(details::names::get_name<0, details::names::names<u8"滑稽", "bla">>() == u8"滑稽");
    static_assert(details::names::get_size<details::names::names<"a", "blabla">>() == 2);
}

void test_namedtuple() noexcept {
    constexpr auto nt = make_namedtuple<u8"hhh", "blaa">(1, u8"233hh");
    static_assert(::std::u8string_view{get<"blaa">(nt)} == u8"233hh");
    static_assert(get<0>(nt) == 1);
}

int main() noexcept {
    return 0;
}
