#pragma once

#include "../include/metastr.hh"

inline void test_metastr() noexcept {
    static_assert(::std::u8string_view{u8"abc"} == metastr::metastr{"abc"});
    static_assert(metastr::metastr{"abc"} == metastr::metastr{"abc"});
    static_assert(metastr::metastr{"abc"} != metastr::metastr{"abcd"});
    static_assert(metastr::metastr{"abc"} != metastr::metastr{"ab"});
    static_assert(u8"abc" == metastr::metastr{"abc"});
    static_assert(u8"ab" != metastr::metastr{"abc"});
    static_assert(u8"abcd" != metastr::metastr{"abc"});
    static_assert(u8"滑稽" != metastr::metastr{"滑稽"}); //NOTE: fucking encoding
    static_assert(metastr::metastr{"abc"} == ::std::u8string{u8"abc"});
    static_assert(metastr::metastr{"abc"} != ::std::u8string{u8"ab"});
    static_assert(metastr::metastr{"abc"} != ::std::u8string{u8"abcd"});
    static_assert(metastr::metastr{"abc"} == ::std::u8string_view{u8"abc"});
    static_assert(metastr::metastr{"abc"} != ::std::u8string_view{u8"ab"});
    static_assert(metastr::metastr{"abc"} != ::std::u8string_view{u8"abcd"});
}

template<metastr::metastr Str>
class Test {};

inline void test_metastr_in_template() noexcept {
    [[maybe_unused]] constexpr auto _ = Test<"abc">{};
}
