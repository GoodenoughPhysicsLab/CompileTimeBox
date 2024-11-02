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


template<is_char Char, ::std::size_t N>
struct metastr {
    using char_type = Char;
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

} // namespace metastr
