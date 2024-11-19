#pragma once

#include <cassert>
#include "../include/metastr.hh"

inline void test_metastr_init() noexcept {
    constexpr auto _1 = metastr::metastr{"abc"};
    constexpr auto _2 = metastr::metastr{L"abc"};
    constexpr auto _3 = metastr::metastr{u8"abc"};
    constexpr auto _4 = metastr::metastr{u"abc"};
    constexpr auto _5 = metastr::metastr{U"abc"};
    static_assert(_1 == _2);
    static_assert(_1 == _3);
    static_assert(_1 == _4);
    static_assert(_1 == _5);
}

inline void test_metastr_eq() noexcept {
    static_assert(::std::u8string_view{u8"abc"} == metastr::metastr{"abc"});
    static_assert(metastr::metastr{"abc"} == "abc");
    static_assert(metastr::metastr{"abc"} != "ab");
    static_assert(metastr::metastr{"abc"} != "abcd");
    static_assert(metastr::metastr{"abc\0\0"} == "abc");
    static_assert(metastr::metastr{"abc"} == "abc\0\0");
    static_assert(metastr::metastr{"abc"} == metastr::metastr{"abc"});
    static_assert(metastr::metastr{"abc"} == metastr::metastr{"abc\0\0"});
    static_assert(metastr::metastr{"abc"} != metastr::metastr{"abcd"});
    static_assert(metastr::metastr{"abc"} != metastr::metastr{"ab"});
    static_assert(u8"abc" == metastr::metastr{"abc"});
    static_assert(u8"ab" != metastr::metastr{"abc"});
    static_assert(u8"abcd" != metastr::metastr{"abc"});
    static_assert(u"滑稽" != metastr::metastr{u8"滑稽"}); //NOTE: fucking encoding
    static_assert("ello,." != metastr::metastr{"ello, "});
}

inline void runtime_test_metastr_eq() noexcept {
    assert(metastr::metastr{"abc"} == ::std::u8string{u8"abc"});
    assert(metastr::metastr{"abc\0"} == ::std::u8string{u8"abc"});
    assert(metastr::metastr{"abc"} != ::std::u8string{u8"ab"});
    assert(metastr::metastr{"abc"} != ::std::u8string{u8"abcd"});
    assert(metastr::metastr{"abc"} == ::std::u8string_view{u8"abc"});
    assert(metastr::metastr{"abc\0\0"} == ::std::u8string_view{u8"abc"});
    assert(metastr::metastr{"abc"} == ::std::u8string_view{u8"abc\0"});
    assert(metastr::metastr{"abc"} != ::std::u8string_view{u8"ab"});
    assert(metastr::metastr{"abc"} != ::std::u8string_view{u8"abcd"});
    assert(::std::string_view{"ello,."} != metastr::metastr{"ello, "});
}

consteval void test_concat() noexcept {
    static_assert(
        metastr::concat(metastr::metastr{"abc"}, metastr::metastr{"def"})
        == metastr::metastr{"abcdef"}
    );
    constexpr auto str1 = metastr::metastr{"abc"};
    constexpr auto str2 = metastr::metastr{"def"};
    static_assert(metastr::concat("abc", "def") == metastr::metastr{"abcdef"});
    static_assert(metastr::concat(str1, "def") == metastr::metastr{"abcdef"});
    static_assert(metastr::concat("abc", str2) == metastr::metastr{"abcdef"});
    static_assert(metastr::concat(str1, str2) == metastr::metastr{"abcdef"});
    static_assert(
        metastr::concat(u8"abc", u8"def", u8"2333", u8"滑稽")
        == metastr::metastr{u8"abcdef2333滑稽"}
    );
}

inline void test_code_cvt() noexcept {
    static_assert(metastr::metastr{U"测逝"} != u8"测逝");
    static_assert(metastr::code_cvt<char8_t>(metastr::metastr{U"测逝"}) == u8"测逝");
    static_assert(metastr::code_cvt<char8_t>(metastr::metastr{U"测逝"}) != U"测逝");
    static_assert(metastr::code_cvt<char8_t>(metastr::metastr{u"测逝"}) == u8"测逝");
    static_assert(metastr::code_cvt<char8_t>(metastr::metastr{u"测逝"}) != u"测逝");
}

inline void test_pop_back() noexcept {
    constexpr auto _1 = metastr::metastr{"abcd"};
    constexpr auto _2 = _1.pop_back();
    static_assert(_2.len == 4);
    constexpr auto _3 = metastr::metastr{"abc"};
    constexpr auto _4 = metastr::metastr{"ab"};
    static_assert(_2 == _3);
    static_assert(_2 != _4);
    static_assert(_2.pop_back() == _4);
}

inline void test_substr() noexcept {
    constexpr auto _1 = metastr::metastr{"Hello, World!"};
    static_assert(_1.substr<7, 5>() == metastr::metastr{"World"});
    static_assert(_1.substr<1, 6>() != metastr::metastr{"ello,."});
    static_assert(_1.substr<1, 6>() != metastr::metastr{"ello. "});
    static_assert(_1.substr<7>() == metastr::metastr{"World!"});
}

inline void test_reduce_trailing_zero() noexcept {
    constexpr auto _1 = metastr::metastr{u8"abc\0\0"};
    constexpr auto _2 = metastr::reduce_trailing_zero<char8_t, 6, _1>();
    constexpr auto _3 = metastr::metastr{u8"abc"};
    static_assert(_2.len == 4);
    static_assert(::std::equal(_2.str, _2.str + _2.len - 1, _3.str));
}

template<metastr::metastr Str>
class Test {};

consteval void test_metastr_in_template() noexcept {
    [[maybe_unused]] constexpr auto _ = Test<"abc">{};
}
