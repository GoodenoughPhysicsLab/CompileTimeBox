#include "../include/namedtuple.hh"

using namespace namedtuple;

void test_template_str() noexcept {
    static_assert(::std::string_view{"abc"} == details::template_str{"abc"});
}

void test_names() noexcept {
    static_assert(details::names::is_names<details::names::names<"a", "blabla">> == true);
    static_assert(details::names::is_names<int> == false);
    static_assert(details::names::get_name<1, details::names::names<"a", "blabla">>() == "blabla");
    static_assert(details::names::get_name<0, details::names::names<"滑稽", "bla">>() == "滑稽");
    static_assert(details::names::get_size<details::names::names<"a", "blabla">>() == 2);
}

void test_namedtuple() noexcept {
    constexpr auto nt = make_namedtuple<"hhh", "blaa">(1, "233hh");
    static_assert(::std::string_view{get<"blaa">(nt)} == "233hh");
}

int main() noexcept {
    return 0;
}
