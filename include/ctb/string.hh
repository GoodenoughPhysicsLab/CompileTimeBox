#pragma once

#if __cpp_concepts < 201907L
    #error "`ctb` requires at least C++20"
#endif  // __cpp_concepts < 201907L

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <type_traits>

#include "exception.hh"
#include "vector.hh"

#ifndef CTB_N_STL_SUPPORT
    #include <string>
    #include <string_view>
#endif  // !defined(CTB_N_STL_SUPPORT)

namespace ctb::string {

// clang-format off
template<typename Char>
concept is_char =
    std::is_same_v<::std::remove_cv_t<Char>, char>
    || std::is_same_v<::std::remove_cv_t<Char>, wchar_t>
#if __cpp_char8_t >= 201811L
    || ::std::is_same_v<::std::remove_cv_t<Char>, char8_t>
#endif  // __cpp_char8_t >= 201811L
    || std::is_same_v<::std::remove_cv_t<Char>, char16_t>
    || std::is_same_v<::std::remove_cv_t<Char>, char32_t>;
// clang-format on

namespace details::transcoding {

constexpr auto LEAD_SURROGATE_MIN = char16_t{0xd800u};
constexpr auto LEAD_SURROGATE_MAX = char16_t{0xdbffu};
constexpr auto TRAIL_SURROGATE_MIN = char16_t{0xdc00u};
constexpr auto TRAIL_SURROGATE_MAX = char16_t{0xdfffu};
// LEAD_SURROGATE_MIN - (0x10000 >> 10)
constexpr auto LEAD_OFFSET = char16_t{0xd7c0u};
// 0x10000u - (LEAD_SURROGATE_MIN << 10) - TRAIL_SURROGATE_MIN
constexpr auto SURROGATE_OFFSET = char32_t{0xfca02400u};
// Maximum valid value for a Unicode code point
constexpr auto CODE_POINT_MAX = char32_t{0x0010ffffu};

/* Assume sizeof(char) == 1
 * Assume encoding of char8_t and char is utf-8
 * Despite encoding of char sometimes is not utf-8 on windows
 *
 * So, do NOT use `char`
 */
template<typename Char>
concept is_utf8 =
#if __cpp_char8_t >= 201811L
    ::std::is_same_v<::std::remove_cv_t<Char>, char8_t> ||  // unprintable utf-8 type
#endif  // __cpp_char8_t >= 201811L
    ::std::is_same_v<::std::remove_cv_t<Char>, char>;  // printable utf-8 type

/* Assume encoding of char16_t and
 * encoding of wchar_t(on windows) is utf-16
 *
 * So, do NOT use wchar_t
 */
template<typename Char>
concept is_utf16 =
#ifdef _WIN32
    ::std::is_same_v<::std::remove_cv_t<Char>, wchar_t> ||
#endif  // defined(_WIN32)
    ::std::is_same_v<::std::remove_cv_t<Char>, char16_t>;

/* Assume encoding of char32_t and
 * wchar(not on windows) is utf-32
 *
 * So, do NOT use wchar_t
 */
template<typename Char>
concept is_utf32 =
#ifndef _WIN32
    ::std::is_same_v<::std::remove_cv_t<Char>, wchar_t> ||
#endif  // !defined(_WIN32)
    ::std::is_same_v<::std::remove_cv_t<Char>, char32_t>;

template<typename Char, typename Char_r>
concept is_same_encoding = details::transcoding::is_utf8<Char> && details::transcoding::is_utf8<Char_r> ||
                           details::transcoding::is_utf16<Char> && details::transcoding::is_utf16<Char_r> ||
                           details::transcoding::is_utf32<Char> && details::transcoding::is_utf32<Char_r>;

}  // namespace details::transcoding

/* class string
 *
 * A string literal that can be used in template.
 *
 * This class just for compile-time using, and to support it, functions/methods in
 * namespace string usually return a new object of class string.
 *
 * if you have another runtime requirement, please convert string to
 * std::string or std::string_view.
 */
template<is_char Char, ::std::size_t N>
struct string {
    static_assert(N != 0);

    using value_type = Char;
    static constexpr auto len{N};
    ::ctb::vector::vector<Char, N> str;

    constexpr string() noexcept = delete;
    constexpr ~string() noexcept = default;

    constexpr string(Char const (&arr)[N]) noexcept
        : str{arr} {
#ifndef NDEBUG
        for (size_t i{}; i < N; ++i) {
            if (arr[i] == '\0') {
                return;
            }
        }
        exception::terminate();
#endif  // !defined(NDEBUG)
    }

    constexpr string(string<Char, N> const& other) noexcept
        : str{other.str} {
    }

    /* Same behavior as ::std::string::substr
     */
    template<::std::size_t pos, ::std::size_t N_r = N - pos - 1>
    [[nodiscard]]
    constexpr auto substr() const noexcept {
        static_assert(pos < N, "ctb::string::IndexError: pos out of range");

        constexpr auto n = ::std::min(N_r, N - pos - 1);
        Char tmp_[n + 1]{};
        ::std::copy(this->str.data() + pos, this->str.data() + pos + n, tmp_);
        return string<Char, n + 1>{tmp_};
    }

    [[nodiscard]]
    constexpr auto pop_back() const noexcept {
        static_assert(N > 1, "Empty string can't be poped back");
        return this->substr<0, string<Char, N>::size() - 1>();
    }

    [[nodiscard]]
    static constexpr ::std::size_t size() noexcept {
        return N - 1;
    }

    [[nodiscard]]
    constexpr auto begin() const noexcept {
        return this->str.data();
    }

    [[nodiscard]]
    constexpr auto end() const noexcept {
        return this->str.data() + N - 1;
    }

    [[nodiscard]]
    constexpr auto operator[](::std::size_t i) const noexcept {
        assert(i < N);
        return vector::get_value(this->str, i);
    }

    template<is_char Char_r, ::std::size_t N_r>
    [[nodiscard]]
    constexpr bool operator==(Char_r const (&other)[N_r]) const noexcept {
        constexpr auto min_num = ::std::min(N, N_r);
        for (size_t i{}; i < min_num; ++i) {
            if (static_cast<::std::ptrdiff_t>(vector::get_value(this->str, i)) !=
                static_cast<::std::ptrdiff_t>(other[i])) {
                return false;
            }
            if (vector::get_value(this->str, i) == '\0') {
                return true;
            }
        }
        if constexpr (N <= N_r) {
            for (::std::size_t i{N - 1}; i < N_r; ++i) {
                if (other[i] != '\0') {
                    return false;
                }
            }
            return true;
        } else {
            for (::std::size_t i{N_r - 1}; i < N; ++i) {
                if (vector::get_value(this->str, i) != '\0') {
                    return false;
                }
            }
            return true;
        }
    }

    template<is_char Char_r, ::std::size_t N_r>
    [[nodiscard]]
    constexpr bool operator==(string<Char_r, N_r> const& other) const noexcept {
        return *this == other.str.data();
    }

#ifndef CTB_N_STL_SUPPORT
    template<is_char Char_r>
    [[nodiscard]]  // TODO bugfix: "abc\0abc" == "abc"
    constexpr bool
    operator==(::std::basic_string_view<Char_r> const& other) const noexcept {
        if (N < other.size()) {
            if (!::std::equal(this->str.begin(), this->str.end() - 1, other.begin())) {
                return false;
            }
            for (::std::size_t i{N - 1}; i < other.size(); ++i) {
                if (other[i] != '\0') {
                    return false;
                }
            }
            return true;
        } else {
            if (!::std::equal(other.begin(), other.end(), this->str.data())) {
                return false;
            }
            for (::std::size_t i{other.size()}; i < N; ++i) {
                if (vector::get_value(this->str, i) != '\0') {
                    return false;
                }
            }
            return true;
        }
    }

    template<is_char Char_r>
    [[nodiscard]]
    constexpr bool operator==(::std::basic_string<Char_r> const other) const noexcept {
        return *this == ::std::basic_string_view<Char_r>{other};
    }

    [[nodiscard]]
    constexpr operator ::std::basic_string<Char>() const noexcept {
        return ::std::basic_string<Char>{str.data(), N - 1};
    }

    [[nodiscard]]
    constexpr operator ::std::basic_string_view<Char>() const noexcept {
        return ::std::basic_string_view<Char>{str.data(), N - 1};
    }
#endif  // !defined(CTB_N_STL_SUPPORT)
};

namespace details::transcoding {

template<details::transcoding::is_utf32 Char, ::std::size_t N, details::transcoding::is_utf8 u8_type>
[[nodiscard]]
constexpr auto utf32to8(string<Char, N> const& u32str) noexcept {
    u8_type tmp_[N * 4 - 3]{};

    auto index = ::std::size_t{};
    for (auto u32chr : u32str) {
        // clang-format off
        assert(
            u32chr <= details::transcoding::CODE_POINT_MAX
            && (u32chr < details::transcoding::LEAD_SURROGATE_MIN
            || u32chr > details::transcoding::TRAIL_SURROGATE_MAX)
        );
        // clang-format on

        if (u32chr < 0x80) {
            tmp_[index++] = static_cast<u8_type>(u32chr);
        } else if (u32chr < 0x800) {
            tmp_[index++] = static_cast<u8_type>((u32chr >> 6) | 0xc0);
            tmp_[index++] = static_cast<u8_type>((u32chr & 0x3f) | 0x80);
        } else if (u32chr < 0x10000) {
            tmp_[index++] = static_cast<u8_type>((u32chr >> 12) | 0xe0);
            tmp_[index++] = static_cast<u8_type>(((u32chr >> 6) & 0x3f) | 0x80);
            tmp_[index++] = static_cast<u8_type>((u32chr & 0x3f) | 0x80);
        } else {
            tmp_[index++] = static_cast<u8_type>((u32chr >> 18) | 0xf0);
            tmp_[index++] = static_cast<u8_type>(((u32chr >> 12) & 0x3f) | 0x80);
            tmp_[index++] = static_cast<u8_type>(((u32chr >> 6) & 0x3f));
            tmp_[index++] = static_cast<u8_type>((u32chr & 0x3f) | 0x80);
        }
    }

    return string{tmp_};
}

template<details::transcoding::is_utf16 Char, ::std::size_t N, details::transcoding::is_utf8 u8_type>
[[nodiscard]]
constexpr auto utf16to8(string<Char, N> const& u16str) noexcept {
    u8_type tmp_[4 * N - 3]{};

    auto index = ::std::size_t{};
    for (::std::size_t i{}; i < N;) {
        auto u32chr = static_cast<char32_t>(vector::get_value(u16str.str, i++) & 0xffff);
        // clang-format off
        assert(
            u32chr < details::transcoding::TRAIL_SURROGATE_MIN
            || u32chr > details::transcoding::TRAIL_SURROGATE_MAX
        );
        if (u32chr >= details::transcoding::LEAD_SURROGATE_MIN
            && u32chr <= details::transcoding::LEAD_SURROGATE_MAX)
        {
            assert(i < N);
            auto const trail_surrogate = static_cast<char32_t>(vector::get_value(u16str.str, i++) & 0xffff);
            assert(
                trail_surrogate >= details::transcoding::TRAIL_SURROGATE_MIN
                && trail_surrogate <= details::transcoding::TRAIL_SURROGATE_MAX
            );
            u32chr = (u32chr << 10) + trail_surrogate + details::transcoding::SURROGATE_OFFSET;
        }
        // clang-format on

        if (u32chr < 0x80) {
            tmp_[index++] = static_cast<u8_type>(u32chr);
        } else if (u32chr < 0x800) {
            tmp_[index++] = static_cast<u8_type>((u32chr >> 6) | 0xc0);
            tmp_[index++] = static_cast<u8_type>((u32chr & 0x3f) | 0x80);
        } else if (u32chr < 0x10000) {
            tmp_[index++] = static_cast<u8_type>((u32chr >> 12) | 0xe0);
            tmp_[index++] = static_cast<u8_type>(((u32chr >> 6) & 0x3f) | 0x80);
            tmp_[index++] = static_cast<u8_type>((u32chr & 0x3f) | 0x80);
        } else {
            tmp_[index++] = static_cast<u8_type>((u32chr >> 18) | 0xf0);
            tmp_[index++] = static_cast<u8_type>(((u32chr >> 12) & 0x3f) | 0x80);
            tmp_[index++] = static_cast<u8_type>(((u32chr >> 6) & 0x3f));
            tmp_[index++] = static_cast<u8_type>((u32chr & 0x3f) | 0x80);
        }
    }

    return string{tmp_};
}

}  // namespace details::transcoding

/* Convert a string to another encoding.
 * Assume char, char8_t -> utf-8
 *        char16_t, wchar_t(Windows) -> utf-16
 *        char32_t, wchar_t(not Windows) -> utf-32
 *
 * Despite on linux, MacOS and Windows(USA), char's encoding is `utf-8`
 * but on windows, char's encoding relys on the system.
 * So, I strongly suggest you do NOT use type `char` and `wchar_t` derectly.
 *
 * And do NOT try to use char8_t with utf-32 encoding etc. (Thank god,
 * compiler has some checks that can avoid some mistakes like this)
 */
template<is_char Char_r, is_char Char, ::std::size_t N>
[[nodiscard]]
constexpr auto code_cvt(string<Char, N> const& str) noexcept {
    if constexpr (details::transcoding::is_same_encoding<Char, Char_r>) {
        Char_r tmp_[N]{};
        ::std::copy(str.str.data(), str.str.data() + N - 1, tmp_);
        return string{tmp_};
    } else if constexpr (details::transcoding::is_utf32<Char> && details::transcoding::is_utf8<Char_r>) {
        return details::transcoding::utf32to8<Char, N, Char_r>(str);
    } else if constexpr (details::transcoding::is_utf16<Char> && details::transcoding::is_utf8<Char_r>) {
        return details::transcoding::utf16to8<Char, N, Char_r>(str);
    }
}

namespace details {

template<typename>
constexpr bool is_ctb_string_ = false;

template<is_char Char, ::std::size_t N>
constexpr bool is_ctb_string_<string<Char, N>> = true;

}  // namespace details

template<typename T>
concept is_ctb_string = details::is_ctb_string_<::std::remove_cvref_t<T>>;

namespace details {

template<typename>
constexpr bool is_c_str_ = false;

template<is_char Char, ::std::size_t N>
constexpr bool is_c_str_<Char[N]> = true;

template<typename T>
concept is_c_str = is_c_str_<::std::remove_cvref_t<T>>;

template<typename T>
concept can_concat = is_ctb_string<T> || is_c_str<T>;

template<can_concat T>
[[nodiscard]]
constexpr auto concat_helper(T const& str) noexcept {
    if constexpr (is_ctb_string<T>) {
        return str;
    } else if constexpr (is_c_str<T>) {
        return string{str};
    } else {
        // InternalError: please bug-report
#ifdef NDEBUG
        exception::unreachable();
#else  // ^^^ defined(NDEBUG) / vvv !defined(NDEBUG)
        exception::terminate();
#endif  // !defined(NDEBUG)
    }
}

}  // namespace details

template<details::can_concat... T>
[[nodiscard]]
constexpr auto concat(T const&... strs) noexcept {
    return concat(details::concat_helper(strs)...);
}

template<is_ctb_string Str1, is_ctb_string... Strs>
    requires (::std::is_same_v<typename Str1::value_type, typename Strs::value_type> && ...)
[[nodiscard]]
constexpr auto concat(Str1 const& str1, Strs const&... strs) noexcept {
    typename Str1::value_type tmp_[Str1::len + (Strs::len + ...) - sizeof...(Strs)]{};
    constexpr decltype(Str1::len) lens[]{Str1::len - 1, (Strs::len - 1)...};
    ::std::size_t index{}, offset{};
    ::std::copy(str1.str.data(), str1.str.data() + Str1::len - 1, tmp_);
    (::std::copy(strs.str.data(), strs.str.data() + Strs::len - 1, (offset += lens[index++], tmp_ + offset)), ...);
    return string{tmp_};
}

#ifndef CTB_N_STL_SUPPORT

namespace details {

template<is_char Char, ::std::size_t N>
[[nodiscard]]
constexpr auto c_str2string(Char const (&str)[N]) noexcept {
    return ::std::basic_string<::std::remove_cv_t<Char>>{str};
}

template<typename T>
[[nodiscard]]
constexpr auto concat_helper2(T const& str) noexcept {
    if constexpr (is_c_str<T>) {
        return c_str2string(str);
    } else {
        return static_cast<::std::basic_string<::std::remove_cvref_t<typename T::value_type>>>(str);
    }
}

}  // namespace details

template<typename... Strs>
    requires ((!details::can_concat<Strs> || ...) &&
              ((details::is_c_str<Strs> || requires { typename Strs::value_type; }) && ...))
[[nodiscard]]
constexpr auto concat(Strs const&... strs) noexcept {
    return (details::concat_helper2(strs) + ...);
}

#endif  // !defined(CTB_N_STL_SUPPORT)

namespace details {

template<is_char Char, ::std::size_t N>
[[nodiscard]]
constexpr ::std::size_t get_first_l0_(string<Char, N> str) noexcept {
    for (::std::size_t i{}; i <= str.size(); ++i) {
        if (str[i] == '\0') {
            return i;
        }
    }
    exception::terminate();
}

}  // namespace details

template<string str>
[[nodiscard]]
constexpr auto reduce_trailing_zero() noexcept {
    return str.template substr<0, details::get_first_l0_(str)>();
}

template<string str_, string substr_>
    requires (details::transcoding::is_same_encoding<typename decltype(str_)::value_type,
                                                     typename decltype(substr_)::value_type>)
[[nodiscard]]
constexpr exception::optional<::std::size_t> find() noexcept {
    constexpr auto str = reduce_trailing_zero<str_>();
    constexpr auto substr = reduce_trailing_zero<substr_>();
    constexpr auto N = str.size();
    constexpr auto M = substr.size();
    if constexpr (N < M) {
        return exception::nullopt;
    } else {
        // kmp
        unsigned int prefix_len{}, i{1};
        auto next = vector::vector<unsigned int, M>{};
        while (i < substr.size()) {
            if (substr[i] == substr[prefix_len]) [[unlikely]] {
                next.arr[i++] = ++prefix_len;
            } else {
                if (prefix_len == 0) {
                    next.arr[i++] = 0;
                } else {
                    prefix_len = vector::get_value(next, prefix_len - 1);
                }
            }
        }

        i = 0;
        unsigned int j{};
        while (i < str.size()) {
            auto chr = str[i];
            if (chr == substr[j]) {
                if (j == substr.size() - 1) {
                    return i - j;
                } else {
                    ++i;
                    ++j;
                }
            } else {
                if (j == 0) {
                    ++i;
                } else {
                    j = vector::get_value(next, j - 1);
                }
            }
        }
        return exception::nullopt;
    }
}

}  // namespace ctb::string
