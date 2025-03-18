#pragma once

#if __cpp_concepts < 201907L
    #error "`ctb` requires at least C++20"
#endif // __cpp_concepts < 201907L

#include <new>
#include <utility>
#include <type_traits>
#include <concepts>
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

template<bool ndebug = false>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
[[noreturn]]
inline void unreachable() noexcept {
    if constexpr (ndebug) {
#if defined(_MSC_VER) && !defined(__clang__) // MSVC
        __assume(false);
#else // GCC, Clang
        __builtin_unreachable();
#endif
    } else {
        ::ctb::exception::terminate();
    }
}

template<bool ndebug = false>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
constexpr void assert_true(bool cond) noexcept {
    if constexpr (!ndebug) {
        if (cond == false) [[unlikely]] {
            ::ctb::exception::terminate();
        }
    }
}

template<bool ndebug = false>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
constexpr void assert_false(bool cond) noexcept {
    if constexpr (!ndebug) {
        if (cond == true) [[unlikely]] {
            ::ctb::exception::terminate();
        }
    }
}

template<typename T>
struct unexpected {
    using value_type = ::std::remove_cvref_t<T>;
    value_type val_{};

    constexpr unexpected() noexcept = default;

    constexpr unexpected(T const& val) noexcept
        requires (::std::is_copy_constructible_v<value_type>)
        : val_{val} {
    }

    constexpr unexpected(T&& val) noexcept
        requires (::std::is_move_constructible_v<value_type>)
        : val_{::std::move(val)} {
    }

    constexpr unexpected(unexpected<T> const& other) noexcept
        requires (::std::is_copy_constructible_v<value_type>)
        : val_{other.val_} {
    }

    constexpr unexpected(unexpected<T>&& other) noexcept
        requires (::std::is_move_constructible_v<value_type>)
        : val_{::std::move(other.val_)} {
    }

    constexpr void operator=(unexpected<T> const& other) noexcept
        requires (::std::is_copy_assignable_v<value_type>)
    {
        this->val_ = other.val_;
    }

    constexpr void operator=(unexpected<T>&& other) noexcept
        requires (::std::is_move_assignable_v<value_type>)
    {
        this->val_ = ::std::move(other.val_);
    }
};

namespace details {

template<typename T>
constexpr bool is_unexpected_v = false;

template<typename T>
constexpr bool is_unexpected_v<unexpected<T>> = true;

} // namespace details

template<typename T>
concept is_unexpected = ::ctb::exception::details::is_unexpected_v<::std::remove_cvref_t<T>>;

template<typename Ok, typename Fail>
class expected {
public:
    using value_type = ::std::remove_cvref_t<Ok>;
    using error_type = ::std::remove_cvref_t<Fail>;

private:
    union {
        value_type ok_;
        error_type fail_;
    };

    bool has_value_;

public:
    constexpr expected() noexcept = delete;

    constexpr expected(Ok const& ok) noexcept
        requires (::std::is_copy_constructible_v<Ok>)
        : ok_{ok},
          has_value_{true} {
    }

    constexpr expected(Ok&& ok) noexcept
        requires (::std::is_move_constructible_v<Ok>)
        : ok_{::std::move(ok)},
          has_value_{true} {
    }

    constexpr expected(unexpected<Fail> const& fail) noexcept
        requires (::std::is_copy_constructible_v<Fail>)
        : fail_{fail.val_},
          has_value_{false} {
    }

    constexpr expected(unexpected<Fail>&& fail) noexcept
        requires (::std::is_move_constructible_v<Fail>)
        : fail_{::std::move(fail.val_)},
          has_value_{false} {
    }

    constexpr expected(expected<Ok, Fail> const& other) noexcept
        : has_value_{other.has_value_} {
        if (this->has_value()) [[likely]] {
            new (&this->ok_) value_type(other.ok_);
        } else {
            new (&this->fail_) error_type(other.fail_);
        }
    }

    constexpr expected(expected<Ok, Fail>&& other) noexcept
        : has_value_{::std::move(other.has_value_)} {
        if (this->has_value()) [[likely]] {
            new (&this->ok_) value_type(::std::move(other.ok_));
        } else {
            new (&this->fail_) error_type(::std::move(other.fail_));
        }
    }

    constexpr ~expected() noexcept {
        if (this->has_value()) [[likely]] {
            this->ok_.~value_type();
        } else {
            this->fail_.~error_type();
        }
    }

    template<typename T>
        requires (::std::same_as<::std::remove_cvref_t<T>, Ok> &&
                  (::std::is_copy_assignable_v<T> || ::std::is_move_assignable_v<T>))
    constexpr auto&& operator=(T&& ok) & noexcept {
        if (this->has_value()) [[likely]] {
            this->ok_ = ::std::forward<T>(ok);
        } else {
            this->fail_.~error_type();
            new (&this->ok_) value_type(::std::forward<T>(ok));
            this->has_value_ = true;
        }
        return (*this);
    }

    template<is_unexpected T>
    constexpr auto&& operator=(T const& fail) & noexcept {
        this->has_value_ = false;
        this->fail_ = fail.val_;
        return *this;
    }

    template<is_unexpected T>
    constexpr auto&& operator=(T&& fail) & noexcept {
        this->has_value_ = false;
        this->fail_ = ::std::move(fail.val_);
        return *this;
    }

    constexpr auto&& operator=(expected<Ok, Fail> const& other) & noexcept {
        this->has_value_ = other.has_value_;
        if (this->has_value()) [[likely]] {
            this->ok_ = other.ok_;
        } else {
            this->fail_ = other.fail_;
        }
        return *this;
    }

    constexpr auto&& operator=(expected<Ok, Fail>&& other) & noexcept {
        this->has_value_ = other.has_value_;
        if (this->has_value()) [[likely]] {
            this->ok_ = ::std::move(other.ok_);
        } else {
            this->fail_ = ::std::move(other.fail_);
        }
        return *this;
    }

    template<typename T>
        requires (::std::same_as<::std::remove_cvref_t<T>, expected<Ok, Fail>> && ::std::is_move_assignable_v<Ok> &&
                  ::std::is_move_assignable_v<Fail>)
    constexpr void swap(T&& other) & noexcept {
        if (this->has_value()) [[likely]] {
            if (other.has_value()) [[likely]] {
                Ok tmp{::std::move(this->ok_)};
                this->ok_ = ::std::move(other.ok_);
                other.ok_ = ::std::move(tmp);
            } else {
                Ok tmp{::std::move(this->ok_)};
                this->ok_ = ::std::move(other.fail_);
                other.fail_ = ::std::move(tmp);
                this->has_value_ = false;
                other.has_value_ = true;
            }
        } else {
            if (other.has_value()) [[likely]] {
                Fail tmp{::std::move(this->fail_)};
                this->fail_ = ::std::move(other.ok_);
                other.fail_ = ::std::move(tmp);
                this->has_value_ = true;
                other.has_value_ = false;
            } else {
                Fail tmp{::std::move(this->fail_)};
                this->fail_ = ::std::move(other.fail_);
                other.fail_ = ::std::move(tmp);
            }
        }
    }

#if __has_cpp_attribute(__gnu__::__always_inline__)
    [[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
    [[msvc::forceinline]]
#endif
    [[nodiscard]]
    constexpr auto&& has_value() const& noexcept {
        return this->has_value_;
    }

#if __has_cpp_attribute(__gnu__::__always_inline__)
    [[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
    [[msvc::forceinline]]
#endif
    [[nodiscard]]
    constexpr auto&& has_value() const&& noexcept {
        return ::std::move(this->has_value_);
    }

    /**
     * @brief get value from optional or expected, if it is not, terminate the program
     * @param self: the optional or expected object
     */
    template<bool ndebug = false>
#if __has_cpp_attribute(__gnu__::__always_inline__)
    [[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
    [[msvc::forceinline]]
#endif
    [[nodiscard]]
    constexpr auto&& value() const& noexcept {
        ::ctb::exception::assert_true<ndebug>(this->has_value());
        return this->ok_;
    }

    template<bool ndebug = false>
#if __has_cpp_attribute(__gnu__::__always_inline__)
    [[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
    [[msvc::forceinline]]
#endif
    [[nodiscard]]
    constexpr auto&& value() const&& noexcept {
        ::ctb::exception::assert_true<ndebug>(this->has_value());
        return ::std::move(this->ok_);
    }

    /**
     * @brief get the error value from an expected
     */
    template<bool ndebug = false>
#if __has_cpp_attribute(__gnu__::__always_inline__)
    [[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
    [[msvc::forceinline]]
#endif
    [[nodiscard]]
    constexpr auto&& error() const& noexcept {
        ::ctb::exception::assert_false<ndebug>(this->has_value());
        return this->fail_;
    }

    template<bool ndebug = false>
#if __has_cpp_attribute(__gnu__::__always_inline__)
    [[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
    [[msvc::forceinline]]
#endif
    [[nodiscard]]
    constexpr auto&& error() const&& noexcept {
        ::ctb::exception::assert_false<ndebug>(this->has_value());
        return ::std::move(this->fail_);
    }

    /**
     * @brief get value from optional or expected, if it is not, return the value you passed
     * @param self: the optional or expected object
     * @param val: the value you want to return if the optional or expected is not
     * @return: the value
     * @note: implicit conversion of val is not allowed
     */
    template<typename U>
        requires (::std::same_as<U, value_type>)
    [[nodiscard]]
    constexpr auto value_or(U& val) & noexcept -> value_type& {
        if (this->has_value() == false) [[unlikely]] {
            return val;
        }
        return this->ok_;
    }

    template<typename U>
        requires (::std::same_as<U, value_type>)
    [[nodiscard]]
    constexpr auto value_or(U const& val) const& noexcept -> value_type const& {
        if (this->has_value() == false) [[unlikely]] {
            return val;
        }
        return this->ok_;
    }

    template<typename U>
        requires (::std::same_as<U, value_type>)
    [[nodiscard]]
    constexpr auto value_or(U&& val) && noexcept -> value_type&& {
        if (this->has_value() == false) [[unlikely]] {
            return ::std::move(val);
        }
        return ::std::move(this->ok_);
    }

    template<typename U>
        requires (::std::same_as<U, value_type>)
    [[nodiscard]]
    constexpr auto value_or(U const&& val) const&& noexcept -> value_type const&& {
        if (this->has_value() == false) [[unlikely]] {
            return ::std::move(val);
        }
        return ::std::move(this->ok_);
    }
};

struct nullopt_t {};

template<typename T>
using optional = ::ctb::exception::expected<T, ::ctb::exception::nullopt_t>;

inline constexpr ::ctb::exception::unexpected<::ctb::exception::nullopt_t> nullopt{};

namespace details {

template<typename T>
constexpr bool is_expected_ = false;

template<typename Ok, typename Fail>
constexpr bool is_expected_<expected<Ok, Fail>> = true;

template<typename T>
constexpr bool is_optional_ = false;

template<typename T>
constexpr bool is_optional_<optional<T>> = true;

} // namespace details

template<typename T>
concept is_expected = ::ctb::exception::details::is_expected_<::std::remove_cvref_t<T>>;

template<typename T>
concept is_optional = ::ctb::exception::details::is_optional_<::std::remove_cvref_t<T>>;

} // namespace ctb::exception
