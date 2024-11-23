#ifdef NDEBUG
    #undef NDEBUG
#endif

#include <cassert>
#include <ctb/string.hh>

using namespace ctb::string;

consteval void test_metastr_init() noexcept {
    constexpr auto _1 = String{"abc"};
    constexpr auto _2 = String{_1};
    constexpr auto _3 = String{L"abc"};
    constexpr auto _4 = String{u8"abc"};
    constexpr auto _5 = String{u"abc"};
    constexpr auto _6 = String{U"abc"};
    static_assert(_1 == _2);
    static_assert(_1 == _3);
    static_assert(_1 == _4);
    static_assert(_1 == _5);
    static_assert(_1 == _6);
}

consteval void test_metastr_eq() noexcept {
    static_assert(::std::u8string_view{u8"abc"} == String{"abc"});
    static_assert(String{"abc"} == "abc");
    static_assert(String{"abc"} != "ab");
    static_assert(String{"abc"} != "abcd");
    static_assert(String{"abc\0\0"} == "abc");
    static_assert(String{"abc"} == "abc\0\0");
    static_assert(String{"abc"} == String{"abc"});
    static_assert(String{"abc"} == String{"abc\0\0"});
    static_assert(String{"abc"} != String{"abcd"});
    static_assert(String{"abc"} != String{"ab"});
    static_assert(u8"abc" == String{"abc"});
    static_assert(u8"ab" != String{"abc"});
    static_assert(u8"abcd" != String{"abc"});
    static_assert(u"滑稽" != String{u8"滑稽"}); //NOTE: fucking encoding
    static_assert("ello,." != String{"ello, "});
}

consteval void test_concat() noexcept {
    static_assert(
        concat(String{"abc"}, String{"def"})
        == String{"abcdef"}
    );
    constexpr auto str1 = String{"abc"};
    constexpr auto str2 = String{"def"};
    static_assert(concat("abc", "def") == String{"abcdef"});
    static_assert(concat(str1, "def") == String{"abcdef"});
    static_assert(concat("abc", str2) == String{"abcdef"});
    static_assert(concat(str1, str2) == String{"abcdef"});
    static_assert(concat(str1, str2) != String{"abcdeg"});
    static_assert(
        concat(u8"abc", u8"def", u8"2333", u8"滑稽")
        == String{u8"abcdef2333滑稽"}
    );
}

consteval void test_code_cvt() noexcept {
    static_assert(String{U"测逝"} != u8"测逝");
    static_assert(code_cvt<char8_t>(String{U"测逝"}) == u8"测逝");
    static_assert(code_cvt<char8_t>(String{U"测逝"}) != U"测逝");
    static_assert(code_cvt<char8_t>(String{u"测逝"}) == u8"测逝");
    static_assert(code_cvt<char8_t>(String{u"测逝"}) != u"测逝");
}

consteval void test_pop_back() noexcept {
    constexpr auto _1 = String{"abcd"};
    constexpr auto _2 = _1.pop_back();
    static_assert(_2.len == 4);
    constexpr auto _4 = String{"ab"};
    static_assert(_2 == "abc");
    static_assert(_2 != _4);
    static_assert(_2.pop_back() == _4);
}

consteval void test_substr() noexcept {
    constexpr auto _1 = String{"Hello, World!"};
    static_assert(_1.substr<7, 5>() == String{"World"});
    static_assert(_1.substr<1, 6>() != String{"ello,."});
    static_assert(_1.substr<1, 6>() != String{"ello. "});
    static_assert(_1.substr<7>() == String{"World!"});
}

consteval void test_reduce_trailing_zero() noexcept {
    constexpr auto _1 = String{u8"abc\0\0"};
    constexpr auto _2 = reduce_trailing_zero<_1>();
    constexpr auto _3 = String{u8"abc"};
    static_assert(_2.len == 4);
    static_assert(::std::equal(_2.str.data(), _2.str.data() + _2.len, _3.str.data()));
}

template<String Str>
class Test {};

consteval void test_metastr_in_template() noexcept {
    [[maybe_unused]] constexpr auto _ = Test<"abc">{};
}

int main() noexcept {
    assert(String{"abc"} == ::std::u8string{u8"abc"});
    assert(String{"abc\0"} == ::std::u8string{u8"abc"});
    assert(String{"abc"} != ::std::u8string{u8"ab"});
    assert(String{"abc"} != ::std::u8string{u8"abcd"});
    assert(String{"abc"} == ::std::u8string_view{u8"abc"});
    assert(String{"abc\0\0"} == ::std::u8string_view{u8"abc"});
    assert(String{"abc"} == ::std::u8string_view{u8"abc\0"});
    assert(String{"abc"} != ::std::u8string_view{u8"ab"});
    assert(String{"abc"} != ::std::u8string_view{u8"abcd"});
    assert(::std::string_view{"ello,."} != String{"ello, "});
    assert(
        concat("k", String{"aaa"}, ::std::string{"bbb"}, ::std::string_view{"ccc"})
        == ::std::string{"kaaabbbccc"}
    );

    return 0;
}
