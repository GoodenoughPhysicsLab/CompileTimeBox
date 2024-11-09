#pragma once

#ifndef __cpp_concepts
    #error "This library requires C++20 concepts"
#endif

#include <cstddef>
#include <algorithm>
#include <type_traits>

#ifndef METASTR_N_STL_SUPPORT
    #include <string>
    #include <string_view>
#endif

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
    static constexpr auto len{N}; // export
    Char str[N]{};

    constexpr metastr(Char const(&arr)[N]) {
        ::std::copy(arr, arr + N - 1, str);
    }

    template<is_char Char_r, ::std::size_t N_r>
    constexpr bool operator==(metastr<Char_r, N_r> const& other) const noexcept {
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

#ifndef METASTR_N_STL_SUPPORT
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
#endif
};

namespace details {

template<typename>
constexpr bool is_metastr_ = false;

template<is_char Char, ::std::size_t N>
constexpr bool is_metastr_<metastr<Char, N>> = true;

} // namespace detalis

template<typename T>
concept is_metastr = details::is_metastr_<::std::remove_cvref_t<T>>;

namespace details {

template<typename>
constexpr bool is_c_str_ = false;

template<is_char Char, ::std::size_t N>
constexpr bool is_c_str_<Char[N]> = true;

template<typename T>
concept is_c_str = is_c_str_<::std::remove_cvref_t<T>>;

template<typename T>
concept can_concat = is_metastr<T> || is_c_str<T>;

} // namespace details

template<details::can_concat... T>
constexpr auto concat(T const&... strs) noexcept {
    return concat(
        [strs] {
            if constexpr (is_metastr<T>) {
                return strs;
            } else { // details::is_c_str<T>
                return metastr{strs};
            }
        }()...
    );
}

template<is_metastr Str1, is_metastr... Strs>
    requires (::std::is_same_v<typename Str1::char_type, typename Strs::char_type> && ...)
constexpr auto concat(Str1 const& str1, Strs const&... strs) noexcept {
    typename Str1::char_type tmp_[Str1::len + (Strs::len + ...) - sizeof...(Strs)]{};
    auto res = metastr{tmp_};
    constexpr decltype(Str1::len) lens[]{Str1::len - 1, (Strs::len - 1)...};
    ::std::size_t index{}, offset{};
    ::std::copy(str1.str, str1.str + Str1::len - 1, res.str);
    (::std::copy(strs.str, strs.str + Strs::len - 1, (offset += lens[index++], res.str + offset)), ...);
    return res;
}

} // namespace metastr
