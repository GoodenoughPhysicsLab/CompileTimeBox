#pragma once

#include <algorithm>
#include <string>
#include <type_traits>

namespace metastr {

template<typename Char>
concept is_char =
    std::is_same_v<Char, char>
    || std::is_same_v<Char, wchar_t>
#if __cpp_char8_t >= 201811L
    || ::std::is_same_v<Char, char8_t>
#endif
    || std::is_same_v<Char, char16_t>
    || std::is_same_v<Char, char32_t>;

/* metastr
 *   A string literal that can be used in template.
 *   This just for compile time, otherwise you should use ::std::string
 */
template<is_char Char, ::std::size_t N>
struct metastr {
    using char_type = Char; // export
    Char str[N]{};

    constexpr metastr(Char const(&arr)[N]) {
        ::std::copy(arr, arr + N - 1, str);
    }

    template<is_char Char_other, ::std::size_t N_r>
    constexpr bool operator==(metastr<Char_other, N_r> const& other) const noexcept {
        if constexpr (N == N_r) {
            return ::std::equal(str, str + N - 1, other.str);
        } else {
            return false;
        }
    }

    template<is_char Char_r, ::std::size_t N_r>
    constexpr bool operator==(Char_r const(&other)[N_r]) const noexcept {
        if constexpr (N == N_r) {
            return ::std::equal(str, str + N - 1, other);
        } else {
            return false;
        }
    }

    template<is_char Char_r>
    constexpr bool operator==(::std::basic_string<Char_r> const& other) const noexcept {
        return ::std::equal(this->str, this->str + N - 1, other.begin(), other.end());
    }

    template<is_char Char_r>
    constexpr bool operator==(::std::basic_string_view<Char_r> const other) const noexcept {
        return ::std::equal(this->str, this->str + N - 1, other.begin(), other.end());
    }

    constexpr operator ::std::basic_string<Char>() const noexcept {
        return ::std::basic_string<Char>{str, N - 1};
    }

    constexpr operator ::std::basic_string_view<Char>() const noexcept {
        return ::std::basic_string_view<Char>{str, N - 1};
    }
};

namespace details {

template<typename>
constexpr bool is_metastr_ = false;

template<is_char Char, ::std::size_t N>
constexpr bool is_metastr_<metastr<Char, N>> = true;

template<is_char Char, ::std::size_t N1, ::std::size_t N2>
consteval auto concat2metastr(metastr<Char, N1> const&& str1, metastr<Char, N2> const&& str2) noexcept {
    Char tmp_[N1 + N2 - 1]{};
    metastr<Char, N1 + N2 - 1> result{tmp_};
    ::std::copy(str1.str, str1.str + N1 - 1, result.str);
    ::std::copy(str2.str, str2.str + N2 - 1, result.str + N1 - 1);
    return result;
}

} // namespace detalis

template<typename T>
concept is_metastr = details::is_metastr_<::std::remove_cvref_t<T>>;

template<is_char... Char, ::std::size_t... N>
    requires (sizeof...(Char) == sizeof...(N))
consteval auto concat(Char const(&... strs)[N]) noexcept {
    return concat(metastr{strs}...);
}

template<is_metastr Str1, is_metastr Str2, is_metastr... MetaStrs>
    requires (
        ::std::is_same_v<typename Str1::char_type, typename Str2::char_type>
        && (::std::is_same_v<typename Str1::char_type, typename MetaStrs::char_type> && ...)
    )
consteval auto concat(Str1 str1, Str2 str2, MetaStrs const&&... meta_strs) noexcept {
    if constexpr (sizeof...(MetaStrs) == 0) {
        return details::concat2metastr(
            ::std::forward<decltype(str1)>(str1),
            ::std::forward<decltype(str2)>(str2)
        );
    }
    else {
        return concat(
            details::concat2metastr(
                ::std::forward<decltype(str1)>(str1),
                ::std::forward<decltype(str2)>(str2)
            ),
            ::std::forward<decltype(meta_strs)>(meta_strs)...
        );
    }
}

} // namespace metastr
