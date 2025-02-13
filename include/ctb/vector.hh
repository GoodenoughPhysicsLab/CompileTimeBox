#pragma once

#if __cpp_concepts < 201907L
    #error "`ctb` requires at least C++20"
#endif  // __cpp_concepts < 201907L

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <type_traits>

namespace ctb::vector {

using len_type_ = ::std::size_t;

/* A compile-time Vector
 * it may looks like ::std::array
 */
template<typename T, len_type_ N>
struct vector {
    static_assert(N > 0);

    using value_type = T;
    using size_type = ::std::size_t;
    using difference_type = ::std::ptrdiff_t;
    T arr[N]{};

    constexpr vector() noexcept = default;
    constexpr ~vector() noexcept = default;

    constexpr vector(T const (&data)[N]) noexcept {
        ::std::copy(data, data + N, this->arr);
    }

    template<typename Arg, typename... Args>
        requires (::std::same_as<Arg, Args> && ...)
    constexpr vector(Arg const& arg, Args const&... args) noexcept {
        Arg tmp_[]{arg, args...};
        ::std::copy(tmp_, tmp_ + N, this->arr);
    }

    constexpr vector(vector const& other) noexcept {
        ::std::copy(other.arr, other.arr + N, this->arr);
    }

    template<typename U, len_type_ N_r>
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
            return ::std::equal(this->arr, this->arr + N - 1, other);
        }
        // clang-format on
    }

    template<typename U, len_type_ N_r>
    [[nodiscard]]
    constexpr bool operator==(vector<U, N_r> const& other) const noexcept {
        return *this == other.arr;
    }

    [[nodiscard]]
    constexpr auto operator[](size_type index) const noexcept {
        assert(index < N);
        return this->arr[index];
    }

    [[nodiscard]]
    constexpr auto begin() const noexcept {
        return this->arr;
    }

    [[nodiscard]]
    constexpr auto end() const noexcept {
        return this->arr + N;
    }

    [[nodiscard]]
    static constexpr len_type_ size() noexcept {
        return N;
    }

    [[nodiscard]]
    constexpr auto data() const noexcept -> decltype(auto) {
        return (this->arr);
    }
};

template<typename Arg, typename... Args>
    requires (::std::same_as<Arg, Args> && ...)
vector(Arg, Args...) -> vector<Arg, sizeof...(Args) + 1>;

template<::std::size_t I, typename T, len_type_ N>
[[nodiscard]]
constexpr auto get(vector<T, N> const& vec) noexcept {
    return vec[I];
}

template<::std::ptrdiff_t Start, ::std::ptrdiff_t End, typename T, len_type_ N>
[[nodiscard]]
consteval auto slice(vector<T, N> const& vec) noexcept {
    constexpr auto start = Start < 0 ? N + Start : Start;
    constexpr auto end = End < 0 ? N + End : End;

    static_assert(start < end, "ctb::vector::IndexError: out of range");

    T tmp_[end - start]{};
    ::std::copy(vec.arr + start, vec.arr + end, tmp_);
    return vector{tmp_};
}

}  // namespace ctb::vector
