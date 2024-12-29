#pragma once

#if !__cpp_explicit_this_parameter >= 202110L
    #error "This library requires C++23"
#endif // !__cpp_explicit_this_parameter >= 202110L

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <type_traits>

namespace ctb::vector {

using len_type_ = ::std::size_t;

/* A compile-time Vector
 * it may looks like ::std::array
 */
template<::std::integral T, len_type_ N>
struct Vector {
    static_assert(N > 0);

    using value_type = T;
    using size_type = ::std::size_t;
    using difference_type = ::std::ptrdiff_t;
    T arr[N]{};

    constexpr Vector() noexcept = delete;

    constexpr Vector(T const (&data)[N]) noexcept {
        ::std::copy(data, data + N, this->arr);
    }

    template<typename Arg, typename... Args>
        requires (::std::same_as<Arg, Args> && ...)
    constexpr Vector(Arg const& arg, Args const&... args) noexcept {
        Arg tmp_[]{arg, args...};
        ::std::copy(tmp_, tmp_ + N, this->arr);
    }

    constexpr Vector(Vector const& other) noexcept {
        ::std::copy(other.arr, other.arr + N, this->arr);
    }

    template<::std::integral U, len_type_ N_r>
    [[nodiscard]]
    constexpr bool operator==(this auto const& self, U const (&other)[N_r]) noexcept {
        // clang-format off
        if constexpr (
            sizeof(N) != sizeof(N_r)
            || ::std::is_unsigned_v<T> ^ ::std::is_unsigned_v<U>
            || N != N_r
        ) {
            return false;
        } else {
            return ::std::equal(self.arr, self.arr + N - 1, other);
        }
        // clang-format on
    }

    template<::std::integral U, len_type_ N_r>
    [[nodiscard]]
    constexpr bool operator==(this auto const& self, Vector<U, N_r> const& other) noexcept {
        return self == other.arr;
    }

    [[nodiscard]]
    constexpr auto operator[](this auto const& self, size_type index) noexcept {
        assert(index < N);
        return self.arr[index];
    }

    [[nodiscard]]
    constexpr auto begin(this auto const& self) noexcept {
        return self.arr;
    }

    [[nodiscard]]
    constexpr auto end(this auto const& self) noexcept {
        return self.arr + N;
    }

    [[nodiscard]]
    static constexpr len_type_ size() noexcept {
        return N;
    }

    [[nodiscard]]
    constexpr auto data(this auto const& self) noexcept -> decltype(auto) {
        return (self.arr);
    }

    template<difference_type Start, difference_type End>
    [[nodiscard]]
    constexpr auto slice(this auto const& self) noexcept {
        constexpr auto start = Start < 0 ? N + Start : Start;
        constexpr auto end = End < 0 ? N + End : End;

        static_assert(start < end, "ctb::vector::IndexError: out of range");

        value_type tmp_[end - start]{};
        ::std::copy(self.arr + start, self.arr + end, tmp_);
        return Vector<value_type, end - start>{tmp_};
    }
};

template<typename Arg, typename... Args>
    requires (::std::same_as<Arg, Args> && ...)
Vector(Arg, Args...) -> Vector<Arg, sizeof...(Args) + 1>;

template<::std::size_t I, ::std::integral T, len_type_ N>
constexpr auto get(Vector<T, N> const& vec) noexcept {
    return vec[I];
}

}  // namespace ctb::vector
