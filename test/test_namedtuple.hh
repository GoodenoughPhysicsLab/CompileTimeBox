#pragma once

#include <string_view>
#include "../include/namedtuple.hh"

inline void test_names() noexcept {
    static_assert(namedtuple::details::is_names<namedtuple::names<"a", "blabla">> == true);
    static_assert(namedtuple::details::is_names<int> == false);
    static_assert(namedtuple::details::get_name<1, namedtuple::names<"a", "blabla">>() == "blabla");
    static_assert(namedtuple::details::get_name<0, namedtuple::names<u8"滑稽", "bla">>() == u8"滑稽");
    static_assert(namedtuple::details::get_size<namedtuple::names<"a", "blabla">>() == 2);
}

inline void test_namedtuple() noexcept {
    constexpr auto nt = namedtuple::make_namedtuple<u8"hhh", "blaa">(1, u8"233hh");
    static_assert(::std::u8string_view{get<"blaa">(nt)} == u8"233hh");
    static_assert(get<0>(nt) == 1);
    [[maybe_unused]] auto [a, b]{nt};
}
