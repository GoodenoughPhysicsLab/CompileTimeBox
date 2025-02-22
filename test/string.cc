#include <ctb/exception.hh>
#include <ctb/string.hh>

using namespace ctb::string;

consteval void test_str_init() noexcept {
    constexpr auto _1 = string{"abc"};
    constexpr auto _2 = string{_1};
    constexpr auto _3 = string{L"abc"};
    constexpr auto _4 = string{u8"abc"};
    constexpr auto _5 = string{u"abc"};
    constexpr auto _6 = string{U"abc"};
    static_assert(_1 == _2);
    static_assert(_1 == _3);
    static_assert(_1 == _4);
    static_assert(_1 == _5);
    static_assert(_1 == _6);
}

consteval void test_index() noexcept {
    constexpr auto _1 = string{"abc"};
    static_assert(_1[0] == 'a');
    static_assert(_1[1] == 'b');
    static_assert(_1[2] == 'c');
}

consteval void test_str_eq() noexcept {
    static_assert(::std::u8string_view{u8"abc"} == string{"abc"});
    static_assert(string{"abc"} == "abc");
    static_assert(string{"abc"} != "ab");
    static_assert(string{"abc"} != "abcd");
    static_assert(string{"abc\0\0"} == "abc");
    static_assert(string{"abc"} == "abc\0\0");
    static_assert(string{"abc"} == string{"abc"});
    static_assert(string{"abc"} == string{"abc\0\0"});
    static_assert(string{"abc"} != string{"abcd"});
    static_assert(string{"abc"} != string{"ab"});
    static_assert(string{"hello\0aa"} == string{"hello"});
    static_assert(string{"hello\0aa"} != string{"hello0"});
    static_assert(string{"hello\0aa"} == string{"hello\0\0\0\0"});
    static_assert(u8"abc" == string{"abc"});
    static_assert(u8"ab" != string{"abc"});
    static_assert(u8"abcd" != string{"abc"});
    static_assert(u"滑稽" != string{u8"滑稽"}); //NOTE: fucking encoding
    static_assert("ello,." != string{"ello, "});
}

consteval void test_concat() noexcept {
    static_assert(
        concat(string{"abc"}, string{"def"})
        == string{"abcdef"}
    );
    constexpr auto str1 = string{"abc"};
    constexpr auto str2 = string{"def"};
    static_assert(concat("abc", "def") == string{"abcdef"});
    static_assert(concat(str1, "def") == string{"abcdef"});
    static_assert(concat("abc", str2) == string{"abcdef"});
    static_assert(concat(str1, str2) == string{"abcdef"});
    static_assert(concat(str1, str2) != string{"abcdeg"});
    static_assert(
        concat(u8"abc", u8"def", u8"2333", u8"滑稽")
        == string{u8"abcdef2333滑稽"}
    );
}

consteval void test_code_cvt() noexcept {
    static_assert(string{U"测逝"} != u8"测逝");
    static_assert(code_cvt<char8_t>(string{U"测逝"}) == u8"测逝");
    static_assert(code_cvt<char8_t>(string{U"测逝"}) != U"测逝");
    static_assert(code_cvt<char8_t>(string{u"测逝"}) == u8"测逝");
    static_assert(code_cvt<char8_t>(string{u"测逝"}) != u"测逝");
}

consteval void test_pop_back() noexcept {
    constexpr auto _1 = string{"abcd"};
    constexpr auto _2 = _1.pop_back();
    static_assert(_2.len == 4);
    constexpr auto _4 = string{"ab"};
    static_assert(_2 == "abc");
    static_assert(_2 != _4);
    static_assert(_2.pop_back() == _4);
}

consteval void test_substr() noexcept {
    constexpr auto _1 = string{"Hello, World!"};
    static_assert(_1.substr<7, 5>() == string{"World"});
    static_assert(_1.substr<1, 6>() != string{"ello,."});
    static_assert(_1.substr<1, 6>() != string{"ello. "});
    static_assert(_1.substr<7>() == string{"World!"});
}

consteval void test_reduce_trailing_zero() noexcept {
    constexpr auto _1 = string{u8"abc\0\0"};
    constexpr auto _2 = reduce_trailing_zero<_1>();
    constexpr auto _3 = string{u8"abc"};
    static_assert(_2.len == 4);
    static_assert(::std::equal(_2.str.data(), _2.str.data() + _2.len, _3.str.data()));
    static_assert(details::get_first_l0_(_1) == 3);
    constexpr auto _4 = string{"abc\0a"};
    static_assert(details::get_first_l0_(_4) == 3);
    static_assert(details::get_first_l0_(_3) == 3);
}

consteval void test_find() noexcept {
    constexpr auto _1 = string{"Hello, World!"};
    constexpr auto _2 = string{"World"};
    static_assert(ctb::exception::get_value(find<_1, _2>()) == 7);
}

template<string Str>
class Test {};

consteval void test_metastr_in_template() noexcept {
    [[maybe_unused]] constexpr auto _ = Test<"abc">{};
}

inline void runtime_test_eq() noexcept {
    ctb::exception::assert_true(string{"abc"} == ::std::u8string{u8"abc"});
    ctb::exception::assert_true(string{"abc\0"} == ::std::u8string{u8"abc"});
    ctb::exception::assert_true(string{"abc"} != ::std::u8string{u8"ab"});
    ctb::exception::assert_true(string{"abc"} != ::std::u8string{u8"abcd"});
    ctb::exception::assert_true(string{"abc"} == ::std::u8string_view{u8"abc"});
    ctb::exception::assert_true(string{"abc\0\0"} == ::std::u8string_view{u8"abc"});
    ctb::exception::assert_true(string{"abc"} == ::std::u8string_view{u8"abc\0"});
    ctb::exception::assert_true(string{"abc"} != ::std::u8string_view{u8"ab"});
    ctb::exception::assert_true(string{"abc"} != ::std::u8string_view{u8"abcd"});
    ctb::exception::assert_true(::std::string_view{"ello,."} != string{"ello, "});
    ctb::exception::assert_true(
        concat("k", string{"aaa"}, ::std::string{"bbb"}, ::std::string_view{"ccc"})
        == ::std::string{"kaaabbbccc"}
    );
}

inline void runtime_test_iter() noexcept {
    constexpr auto _1 = string{"abc"};
    for (auto ch : _1) {
        ctb::exception::assert_true(ch == 'a' || ch == 'b' || ch == 'c');
    }
}

int main() noexcept {
    runtime_test_eq();
    runtime_test_iter();

    return 0;
}
