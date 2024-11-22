#pragma once

#if !__cpp_concepts >= 201907L
    #error "namedtuple requires at least c++20"
#endif

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <type_traits>

namespace ctb::vector {

/* A compile-time vector
 * looks like ::std::array
 */
template<::std::integral T, ::std::size_t N>
struct vector {
    using value_type = T;
    T data[N]{};

    constexpr vector() noexcept = delete;

    constexpr vector(T const (&data)[N]) noexcept {
        ::std::copy(data, data + N - 1, this->data);
    }

    template<typename Arg, typename... Args>
        requires (::std::same_as<Arg, Args> && ...)
    constexpr vector(Arg const& arg, Args const&... args) noexcept {
        Arg tmp_[]{arg, args...};
        ::std::copy(tmp_, tmp_ + N - 1, this->data);
    }

    constexpr vector(vector const& other) noexcept {
        ::std::copy(other.data, other.data + N - 1, this->data);
    }

    template<::std::integral U, ::std::size_t N_r>
    [[nodiscard]]
    constexpr bool operator==(U const (&other)[N_r]) const noexcept {
        // clang-format off
        if constexpr (
            sizeof(N) != sizeof(N_r)
            || ::std::is_unsigned_v<T> ^ ::std::is_unsigned_v<U>
            || N != N_r
        ) {
            return false;
        } else {
            return ::std::equal(this->data, this->data + N - 1, other);
        }
        // clang-format on
    }

    template<::std::integral U, ::std::size_t N_r>
    [[nodiscard]]
    constexpr bool operator==(vector<U, N_r> const& other) const noexcept {
        return *this == other.data;
    }
};

template<typename Arg, typename... Args>
    requires (::std::same_as<Arg, Args> && ...)
vector(Arg, Args...) -> vector<Arg, sizeof...(Args) + 1>;

}  // namespace ctb::vector
