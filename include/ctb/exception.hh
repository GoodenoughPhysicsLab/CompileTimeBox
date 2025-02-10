#pragma once

#if __cpp_concepts < 201907L
    #error "`ctb` requires at least C++20"
#endif  // __cpp_concepts < 201907L

#include <new>
#include <utility>
#include <type_traits>
#if defined(_MSC_VER) && !defined(__clang__)
    #include <cstdlib>
#endif

namespace ctb::exception {

#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
[[noreturn]]
inline void terminate() noexcept {
    // https://llvm.org/doxygen/Compiler_8h_source.html
#if defined(__has_builtin)
    #if __has_builtin(__builtin_trap)
    __builtin_trap();
    #elif __has_builtin(__builtin_abort)
    __builtin_abort();
    #else
    ::std::abort();
    #endif
#else
    ::std::abort();
#endif
}

#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
[[noreturn]]
inline void unreachable() noexcept {
#if defined(_MSC_VER) && !defined(__clang__)  // MSVC
    __assume(false);
#else  // GCC, Clang
    __builtin_unreachable();
#endif
}

inline constexpr struct nullopt_t {
    constexpr nullopt_t() noexcept = default;
    constexpr ~nullopt_t() noexcept = default;
    constexpr nullopt_t(nullopt_t const&) noexcept = delete;
    constexpr nullopt_t(nullopt_t const&&) noexcept = delete;
    constexpr auto operator=(nullopt_t const&) noexcept = delete;
    constexpr auto operator=(nullopt_t const&&) noexcept = delete;
} nullopt{};

template<typename T>
    requires (!::std::is_same_v<::std::remove_cvref_t<T>, nullopt_t>)
struct optional {
    using value_type = ::std::remove_cvref_t<T>;

    union {
        char fail_;
        value_type ok_;
    };

    bool has_value_;

    /* use optional{nullopt} instead
     */
    constexpr optional() noexcept = delete;

    constexpr ~optional() noexcept {
        if (this->has_value_) [[likely]] {
            this->ok_.~value_type();
        }
    }

    constexpr optional(value_type const& val) noexcept
        requires (::std::is_copy_constructible_v<value_type>)
        : ok_{val}, has_value_{true} {
    }

    constexpr optional(value_type&& val) noexcept
        requires (::std::is_move_constructible_v<value_type>)
        : ok_{::std::move(val)}, has_value_{true} {
    }

    constexpr optional(nullopt_t const&) noexcept
        : fail_{}, has_value_{false} {
    }

    constexpr optional(nullopt_t const&&) noexcept = delete;

    constexpr optional(optional const& other) noexcept
        requires (::std::is_copy_constructible_v<value_type>)
        : ok_{other.ok_}, has_value_{other.has_value_} {
    }

    constexpr optional(optional const&& other) noexcept {
        new (this) optional{::std::move(other)};
    }

    template<typename U>
        requires (::std::is_copy_constructible_v<value_type> && ::std::is_convertible_v<U, value_type> &&
                  !::std::is_same_v<::std::remove_cvref_t<U>, nullopt_t>)
#if __cpp_explicit_this_parameter >= 202110L
    constexpr auto operator=(this optional& self, U&& val) noexcept -> decltype(auto) {
        self.ok_ = ::std::forward<U>(val);
        self.has_value_ = true;
        return (self);
    }
#else
    constexpr auto operator=(U&& val) & noexcept -> decltype(auto) {
        this->ok_ = ::std::forward<U>(val);
        this->has_value_ = true;
        return (*this);
    }
#endif

#if __cpp_explicit_this_parameter >= 202110L
    constexpr auto operator=(this optional self, nullopt_t const&) noexcept -> decltype(auto) {
        if (self.has_value_) {
            self.reset();
        }
        return (self);
    }
#else
    constexpr auto operator=(nullopt_t const&) & noexcept -> decltype(auto) {
        if (this->has_value_) {
            this->reset();
        }
        return (*this);
    }
#endif
    /* opt = nullopt_t{} is not allowed
     */
    constexpr auto operator=(nullopt_t const&&) noexcept = delete;

#if __cpp_explicit_this_parameter >= 202110L
    constexpr auto operator=(this optional& self, optional const& other) noexcept -> decltype(auto)
        requires (::std::is_copy_constructible_v<value_type>)
    {
        self.val_ = other.ok_;
        self.has_value_ = other.has_value_;
        return (self);
    }
#else
    constexpr auto operator=(optional const& other) & noexcept -> decltype(auto)
        requires (::std::is_copy_constructible_v<value_type>)
    {
        this->ok_ = other.ok_;
        this->has_value_ = other.has_value_;
        return (*this);
    }
#endif

#if __cpp_explicit_this_parameter >= 202110L
    constexpr auto operator=(this optional& self, optional&& other) noexcept -> decltype(auto) {
        self.val_ = ::std::move(other.ok_);
        self.has_value_ = ::std::move(other.has_value_);
        return (self);
    }
#else
    constexpr auto operator=(optional&& other) & noexcept -> decltype(auto) {
        ok_ = ::std::move(other.ok_);
        has_value_ = ::std::move(other.has_value_);
        return (*this);
    }
#endif

    /* No need to support, use has_value() instead
     */
    constexpr operator bool() noexcept = delete;

    /* use get_value() instead
     */
    constexpr auto operator*() noexcept = delete;
    constexpr auto operator->() noexcept = delete;

#if __cpp_explicit_this_parameter >= 202110L
    constexpr void reset(this auto&& self) noexcept {
        if (self.has_value_) {
            self.ok_.~value_type();
            self.has_value_ = false;
        }
    }
#else
    constexpr void reset() noexcept {
        if (this->has_value_) {
            this->ok_.~value_type();
            this->has_value_ = false;
        }
    }
#endif

    template<typename U, typename... Args>
        requires (::std::is_constructible_v<value_type, U, Args...> &&
                  !::std::is_same_v<::std::remove_cvref_t<U>, nullopt_t>)
    constexpr void emplace(U&& value, Args&&... args) noexcept {
        if (this->has_value_) [[unlikely]] {
            this->ok_.~value_type();
        } else {
            this->has_value_ = true;
        }
        new (&this->ok_) value_type(::std::forward<U>(value), ::std::forward<Args>(args)...);
    }
};

namespace details {

template<typename T>
constexpr bool is_optional_ = false;

template<typename T>
constexpr bool is_optional_<optional<T>> = true;

}  // namespace details

template<typename T>
concept is_optional = details::is_optional_<::std::remove_cvref_t<T>>;

template<typename T>
struct unexpected {
    using value_type = ::std::remove_cvref_t<T>;
    value_type val_;

    constexpr unexpected() noexcept = delete;

    constexpr unexpected(T const& val) noexcept
        requires (::std::is_copy_constructible_v<value_type>)
        : val_{val} {
    }

    constexpr unexpected(T&& val) noexcept
        requires (::std::is_move_constructible_v<value_type>)
        : val_{::std::move(val)} {
    }

    constexpr unexpected(unexpected<T> const& other) noexcept
        : val_{other.val_} {
    }

    constexpr unexpected(unexpected<T>&& other) noexcept
        : val_{::std::move(other.val_)} {
    }
};

template<typename T>
constexpr bool is_unexpected_v = false;

template<typename T>
constexpr bool is_unexpected_v<unexpected<T>> = true;

template<typename T>
concept is_unexpected = is_unexpected_v<::std::remove_cvref_t<T>>;

template<typename Ok, typename Fail>
struct expected {
    using value_type = ::std::remove_cvref_t<Ok>;
    using error_type = ::std::remove_cvref_t<Fail>;

    union {
        value_type ok_;
        error_type fail_;
    };

    bool has_value_;

    constexpr expected() noexcept = delete;

    constexpr expected(Ok const& ok) noexcept
        requires (::std::is_copy_constructible_v<Ok>)
        : ok_{ok}, has_value_{true} {
    }

    constexpr expected(Ok&& ok) noexcept
        requires (::std::is_move_constructible_v<Ok>)
        : ok_{::std::move(ok)}, has_value_{true} {
    }

    constexpr expected(unexpected<Fail> const& fail) noexcept
        requires (::std::is_copy_constructible_v<Fail>)
        : fail_{fail.val_}, has_value_{false} {
    }

    constexpr expected(unexpected<Fail>&& fail) noexcept
        requires (::std::is_move_constructible_v<Fail>)
        : fail_{::std::move(fail.val_)}, has_value_{false} {
    }

    constexpr expected(expected<Ok, Fail> const& other) noexcept
        : has_value_{other.has_value_} {
        if (this->has_value_) [[likely]] {
            new (&this->ok_) value_type(other.ok_);
        } else {
            new (&this->fail_) error_type(other.fail_);
        }
    }

    constexpr expected(expected<Ok, Fail>&& other) noexcept
        : has_value_{::std::move(other.has_value_)} {
        if (this->has_value_) [[likely]] {
            new (&this->ok_) value_type(::std::move(other.ok_));
        } else {
            new (&this->fail_) error_type(::std::move(other.fail_));
        }
    }

    constexpr ~expected() noexcept {
        if (this->has_value_) [[likely]] {
            this->ok_.~value_type();
        } else {
            this->fail_.~error_type();
        }
    }

    template<typename T>
        requires (::std::is_same_v<::std::remove_cvref_t<T>, Ok>)
#if __cpp_explicit_this_parameter >= 202110L
    constexpr auto operator=(this expected& self, T&& ok) noexcept -> decltype(auto)
        requires (::std::is_copy_constructible_v<value_type> || ::std::is_move_constructible_v<value_type>)
    {
        if (self.has_value_) [[likely]] {
            self.ok_ = ::std::forward<T>(ok);
        } else {
            self.fail_.~error_type();
            new (&self.ok_) value_type(::std::forward<T>(ok));
            self.has_value_ = true;
        }
        return (self);
    }
#else
    constexpr auto operator=(T&& ok) & noexcept -> decltype(auto)
        requires (::std::is_copy_constructible_v<value_type> || ::std::is_move_constructible_v<value_type>)
    {
        if (this->has_value_) [[likely]] {
            this->ok_ = ::std::forward<T>(ok);
        } else {
            this->fail_.~error_type();
            new (&this->ok_) value_type(::std::forward<T>(ok));
            this->has_value_ = true;
        }
        return (*this);
    }
#endif

    template<is_unexpected T>
#if __cpp_explicit_this_parameter >= 202110L
    constexpr auto operator=(this expected& self, T const& fail) noexcept -> decltype(auto) {
        self.has_value_ = false;
        self.fail_ = fail.val_;
        return (self);
    }
#else
    constexpr auto operator=(T const& fail) & noexcept -> decltype(auto) {
        this->has_value_ = false;
        this->fail_ = fail.val_;
        return (*this);
    }
#endif

    template<is_unexpected T>
#if __cpp_explicit_this_parameter >= 202110L
    constexpr auto operator=(this expected& self, T&& fail) noexcept -> decltype(auto) {
        self.has_value_ = false;
        self.fail_ = ::std::move(fail.val_);
        return (self);
    }
#else
    constexpr auto operator=(T&& fail) & noexcept -> decltype(auto) {
        this->has_value_ = false;
        this->fail_ = ::std::move(fail.val_);
        return (*this);
    }
#endif

#if __cpp_explicit_this_parameter >= 202110L
    constexpr auto operator=(this expected& self, expected<Ok, Fail> const& other) noexcept -> decltype(auto) {
        self.has_value_ = other.has_value_;
        if (self.has_value_) [[likely]] {
            self.ok_ = other.ok_;
        } else {
            self.fail_ = other.fail_;
        }
        return (self);
    }
#else
    constexpr auto operator=(expected<Ok, Fail> const& other) & noexcept -> decltype(auto) {
        this->has_value_ = other.has_value_;
        if (this->has_value_) [[likely]] {
            this->ok_ = other.ok_;
        } else {
            this->fail_ = other.fail_;
        }
        return (*this);
    }
#endif

#if __cpp_explicit_this_parameter >= 202110L
    constexpr auto operator=(this expected& self, expected<Ok, Fail>&& other) noexcept -> decltype(auto) {
        self.has_value_ = other.has_value_;
        if (self.has_value_) [[likely]] {
            self.ok_ = ::std::move(other.ok_);
        } else {
            self.fail_ = ::std::move(other.fail_);
        }
        return (self);
    }
#else
    constexpr auto operator=(expected<Ok, Fail>&& other) & noexcept -> decltype(auto) {
        this->has_value_ = other.has_value_;
        if (this->has_value_) [[likely]] {
            this->ok_ = ::std::move(other.ok_);
        } else {
            this->fail_ = ::std::move(other.fail_);
        }
        return (*this);
    }
#endif
};

template<typename T>
constexpr bool is_expected_v = false;

template<typename Ok, typename Fail>
constexpr bool is_expected_v<expected<Ok, Fail>> = true;

template<typename T>
concept is_expected = is_expected_v<::std::remove_cvref_t<T>>;

template<typename T>
    requires (is_expected<T> || is_optional<T>)
[[nodiscard]]
constexpr auto has_value(T& t) noexcept -> bool& {
    return t.has_value_;
}

template<typename T>
    requires (is_expected<T> || is_optional<T>)
[[nodiscard]]
constexpr auto has_value(T const& t) noexcept -> bool const& {
    return t.has_value_;
}

template<typename T>
    requires (is_expected<T> || is_optional<T>)
[[nodiscard]]
constexpr auto has_value(T&& t) noexcept -> bool&& {
    return ::std::move(t.has_value_);
}

template<typename T>
    requires (is_expected<T> || is_optional<T>)
[[nodiscard]]
constexpr auto has_value(T const&& t) noexcept -> bool const&& {
    return ::std::move(t.has_value_);
}

/* get value from optional or expected, if it is not, terminate the program
 * @param self: the optional or expected object
 */
template<typename T>
    requires (is_expected<T> || is_optional<T>)
[[nodiscard]]
constexpr auto get_value(T& self) noexcept -> typename T::value_type& {
#ifndef NDEBUG
    if (has_value(self) == false) [[unlikely]] {
        terminate();
    }
#endif  // !defined(NDEBUG)
    return self.ok_;
}

template<typename T>
    requires (is_expected<T> || is_optional<T>)
[[nodiscard]]
constexpr auto get_value(T const& self) noexcept -> typename T::value_type const& {
#ifndef NDEBUG
    if (has_value(self) == false) [[unlikely]] {
        terminate();
    }
#endif  // !defined(NDEBUG)
    return self.ok_;
}

template<typename T>
    requires (is_expected<T> || is_optional<T>)
[[nodiscard]]
constexpr auto get_value(T&& self) noexcept -> typename T::value_type&& {
#ifndef NDEBUG
    if (has_value(self) == false) [[unlikely]] {
        terminate();
    }
#endif  // !defined(NDEBUG)
    return ::std::move(self.ok_);
}

template<typename T>
    requires (is_expected<T> || is_optional<T>)
[[nodiscard]]
constexpr auto get_value(T const&& self) noexcept -> typename T::value_type const&& {
#ifndef NDEBUG
    if (has_value(self) == false) [[unlikely]] {
        terminate();
    }
#endif  // !defined(NDEBUG)
    return ::std::move(self.ok_);
}

/* get value from optional or expected, if it is not, return the value you passed
 * @param self: the optional or expected object
 * @param val: the value you want to return if the optional or expected is not
 * @return: the value
 * @note: implicit conversion of val is not allowed
 */
template<typename T, typename U>
    requires ((is_expected<T> || is_optional<T>) && ::std::is_same_v<U, typename T::value_type>)
[[nodiscard]]
constexpr auto value_or(T& self, U& val) noexcept -> typename T::value_type& {
    if (has_value(self) == false) [[unlikely]] {
        return val;
    }
    return self.ok_;
}

template<typename T, typename U>
    requires ((is_expected<T> || is_optional<T>) && ::std::is_same_v<U, typename T::value_type>)
[[nodiscard]]
constexpr auto value_or(T const& self, U const& val) noexcept -> typename T::value_type const& {
    if (has_value(self) == false) [[unlikely]] {
        return val;
    }
    return self.ok_;
}

template<typename T, typename U>
    requires ((is_expected<T> || is_optional<T>) && ::std::is_same_v<U, typename T::value_type>)
[[nodiscard]]
constexpr auto value_or(T&& self, U&& val) noexcept -> typename T::value_type&& {
    if (has_value(self) == false) [[unlikely]] {
        return ::std::move(val);
    }
    return ::std::move(self.ok_);
}

template<typename T, typename U>
    requires ((is_expected<T> || is_optional<T>) && ::std::is_same_v<U, typename T::value_type>)
[[nodiscard]]
constexpr auto value_or(T const&& self, U const&& val) noexcept -> typename T::value_type const&& {
    if (has_value(self) == false) [[unlikely]] {
        return ::std::move(val);
    }
    return ::std::move(self.ok_);
}

/* get the error value from an expected
 */
template<is_expected T>
[[nodiscard]]
constexpr auto get_error(T& self) noexcept -> typename T::error_type& {
#if !defined(NDEBUG)
    if (has_value(self) == true) [[unlikely]] {
        terminate();
    }
#endif  // !defined(NDEBUG)
    return self.fail_;
}

template<is_expected T>
[[nodiscard]]
constexpr auto get_error(T const& self) noexcept -> typename T::error_type const& {
#if !defined(NDEBUG)
    if (has_value(self) == true) [[unlikely]] {
        terminate();
    }
#endif  // !defined(NDEBUG)
    return self.fail_;
}

template<is_expected T>
[[nodiscard]]
constexpr auto get_error(T&& self) noexcept -> typename T::error_type&& {
#if !defined(NDEBUG)
    if (has_value(self) == true) [[unlikely]] {
        terminate();
    }
#endif  // !defined(NDEBUG)
    return ::std::move(self.fail_);
}

template<is_expected T>
[[nodiscard]]
constexpr auto get_error(T const&& self) noexcept -> typename T::error_type const&& {
#if !defined(NDEBUG)
    if (has_value(self) == true) [[unlikely]] {
        terminate();
    }
#endif  // !defined(NDEBUG)
    return ::std::move(self.fail_);
}

}  // namespace ctb::exception
