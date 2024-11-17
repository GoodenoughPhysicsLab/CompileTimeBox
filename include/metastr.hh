#pragma once

#ifndef __cpp_concepts
    #error "This library requires C++20 concepts"
#endif

#include <algorithm>
#include <cstddef>
#include <type_traits>

#ifndef METASTR_N_STL_SUPPORT
    #include <string>
    #include <string_view>
#endif

namespace metastr {

template<typename Char>
concept is_char = std::is_same_v<Char, char> || std::is_same_v<Char, wchar_t>
#if __cpp_char8_t >= 201811L
                  || ::std::is_same_v<Char, char8_t>
#endif
                  || std::is_same_v<Char, char16_t> || std::is_same_v<Char, char32_t>;

/* metastr
 *   A string literal that can be used in template.
 *   This just for compile time, otherwise you should use ::std::string
 */
template<is_char Char, ::std::size_t N>
struct metastr {
    using char_type = Char;
    static constexpr auto len{N};
    Char str[N]{};

    constexpr metastr(Char const (&arr)[N]) {
        ::std::copy(arr, arr + N - 1, str);
    }

    template<is_char Char_r, ::std::size_t N_r>
    constexpr bool operator==(Char_r const (&other)[N_r]) const noexcept {
        if constexpr (N <= N_r) {
            if (!::std::equal(this->str, this->str + N - 2, other)) {
                return false;
            }
            for (::std::size_t i{N - 1}; i < N_r; ++i) {
                if (other[i] != '\0') {
                    return false;
                }
            }
            return true;
        } else {
            if (!::std::equal(other, other + N_r - 2, this->str)) {
                return false;
            }
            for (::std::size_t i{N_r - 1}; i < N; ++i) {
                if (this->str[i] != '\0') {
                    return false;
                }
            }
            return true;
        }
    }

    template<is_char Char_r, ::std::size_t N_r>
    constexpr bool operator==(metastr<Char_r, N_r> const& other) const noexcept {
        return *this == other.str;
    }

#ifndef METASTR_N_STL_SUPPORT
    template<is_char Char_r>
    constexpr bool operator==(::std::basic_string_view<Char_r> const& other) const noexcept {
        if (N <= other.size()) {
            if (!::std::equal(this->str, this->str + N - 2, other.begin())) {
                return false;
            }
            for (::std::size_t i{N - 1}; i < other.size(); ++i) {
                if (other[i] != '\0') {
                    return false;
                }
            }
            return true;
        } else {
            if (!::std::equal(other.begin(), other.end() - 1, this->str)) {
                return false;
            }
            for (::std::size_t i{other.size()}; i < N; ++i) {
                if (this->str[i] != '\0') {
                    return false;
                }
            }
            return true;
        }
    }

    template<is_char Char_r>
    constexpr bool operator==(::std::basic_string<Char_r> const other) const noexcept {
        return *this == ::std::basic_string_view<Char_r>{other};
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

}  // namespace details

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

}  // namespace details

template<details::can_concat... T>
[[nodiscard]] constexpr auto concat(T const&... strs) noexcept {
    return concat([strs] {
        if constexpr (is_metastr<T>) {
            return strs;
        } else {  // details::is_c_str<T>
            return metastr{strs};
        }
    }()...);
}

template<is_metastr Str1, is_metastr... Strs>
    requires (::std::is_same_v<typename Str1::char_type, typename Strs::char_type> && ...)
[[nodiscard]] constexpr auto concat(Str1 const& str1, Strs const&... strs) noexcept {
    constexpr typename Str1::char_type tmp_[Str1::len + (Strs::len + ...) - sizeof...(Strs)]{};
    auto res = metastr{tmp_};
    constexpr decltype(Str1::len) lens[]{Str1::len - 1, (Strs::len - 1)...};
    ::std::size_t index{}, offset{};
    ::std::copy(str1.str, str1.str + Str1::len - 1, res.str);
    (::std::copy(strs.str, strs.str + Strs::len - 1, (offset += lens[index++], res.str + offset)), ...);
    return res;
}

}  // namespace metastr
