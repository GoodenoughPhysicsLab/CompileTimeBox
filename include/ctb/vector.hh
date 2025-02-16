#pragma once

#if __cpp_concepts < 201907L
    #error "`ctb` requires at least C++20"
#endif  // __cpp_concepts < 201907L

#include <algorithm>
#include <cstddef>
#include <type_traits>
#include "exception.hh"

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
        requires ((::std::is_same_v<Arg, Args> && ...) && sizeof...(Args) < N)
    constexpr vector(Arg const& arg, Args const&... args) noexcept {
        Arg const tmp_[N]{arg, args...};
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
    constexpr auto begin() const noexcept {
        return this->arr;
    }

    [[nodiscard]]
    constexpr auto end() const noexcept {
        return this->arr + N;
    }

#if __has_cpp_attribute(__gnu__::__always_inline__)
    [[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
    [[msvc::forceinline]]
#endif
    [[nodiscard]]
    static constexpr auto size() noexcept {
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

template<typename T, len_type_ N>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
[[nodiscard]]
constexpr auto get_value(vector<T, N>& self, ::std::size_t index) noexcept -> T& {
#if !defined(NDEBUG)
    if (index >= N) [[unlikely]] {
        exception::terminate();
    }
#endif
    return self.arr[index];
}

template<typename T, len_type_ N>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
[[nodiscard]]
constexpr auto get_value(vector<T, N> const& self, ::std::size_t index) noexcept -> T const& {
#if !defined(NDEBUG)
    if (index >= N) [[unlikely]] {
        exception::terminate();
    }
#endif
    return self.arr[index];
}

template<typename T, len_type_ N>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
[[nodiscard]]
constexpr auto get_value(vector<T, N>&& self, ::std::size_t index) noexcept -> T&& {
#if !defined(NDEBUG)
    if (index >= N) [[unlikely]] {
        exception::terminate();
    }
#endif
    return ::std::move(self.arr[index]);
}

template<typename T, len_type_ N>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
[[nodiscard]]
constexpr auto get_value(vector<T, N> const&& self, ::std::size_t index) noexcept -> T const&& {
#if !defined(NDEBUG)
    if (index >= N) [[unlikely]] {
        exception::terminate();
    }
#endif
    return ::std::move(self.arr[index]);
}

}  // namespace ctb::vector
