#pragma once

#if !__cpp_concepts >= 201907L
    #error "`ctb` requires at least C++20"
#endif  // !__cpp_concepts >= 201907L

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

inline struct NullOpt_t {
    struct NullOpt_t_Construct_ {};

    constexpr NullOpt_t() noexcept = delete;
    explicit constexpr NullOpt_t(NullOpt_t const&) noexcept = delete;

    explicit constexpr NullOpt_t(NullOpt_t_Construct_ const&&) noexcept {
    }
} nullopt{NullOpt_t::NullOpt_t_Construct_{}};

template<typename T>
    requires (!::std::is_same_v<::std::remove_cvref_t<T>, NullOpt_t>)
class Optional {
    using value_type = ::std::remove_cvref_t<T>;

    union {
        char dummy_;
        value_type val_;
    };

    bool has_value_;

public:
    // use Optional{nullopt} instead
    constexpr Optional() noexcept = delete;

    constexpr ~Optional() noexcept {
        if (this->has_value()) [[likely]] {
            this->val_.~value_type();
        }
    }

    constexpr Optional(value_type const& val) noexcept
        requires (::std::is_copy_constructible_v<value_type>)
        : val_{val}, has_value_{true} {
    }

    constexpr Optional(value_type&& val) noexcept
        requires (::std::is_move_constructible_v<value_type>)
        : val_{::std::forward<decltype(val)>(val)}, has_value_{true} {
    }

    constexpr Optional(NullOpt_t&) noexcept
        : dummy_{}, has_value_{false} {
    }

    constexpr Optional(Optional const& other) noexcept
        requires (::std::is_copy_constructible_v<value_type>)
        : val_{other.val_}, has_value_{other.has_value_} {
    }

    constexpr Optional(Optional const&& other) noexcept {
        new (this) Optional{::std::move(other)};
    }

    template<typename U>
        requires (::std::is_copy_constructible_v<value_type> && ::std::is_convertible_v<U, value_type> &&
                  !::std::is_same_v<::std::remove_cvref_t<U>, NullOpt_t>)
#if __cpp_explicit_this_parameter >= 202110L
    constexpr auto operator=(this auto&& self, U&& val) noexcept -> decltype(auto) {
        if constexpr (::std::is_same_v<::std::remove_cvref_t<U>, value_type>) {
            self.val_ = ::std::forward<decltype(val)>(val);
        } else {
            self.val_ = static_cast<value_type>(::std::forward<decltype(val)>(val));
        }
        self.has_value_ = true;
        return (self);
    }
#else
    constexpr auto operator=(U&& val) noexcept -> decltype(auto) {
        if constexpr (::std::is_same_v<::std::remove_cvref_t<U>, value_type>) {
            this->val_ = ::std::forward<decltype(val)>(val);
        } else {
            this->val_ = static_cast<value_type>(::std::forward<decltype(val)>(val));
        }
        this->has_value_ = true;
        return (*this);
    }
#endif

#if __cpp_explicit_this_parameter >= 202110L
    constexpr auto operator=(this auto&& self, NullOpt_t&) noexcept -> decltype(auto) {
        self.has_value_ = false;
        return (self);
    }
#else
    constexpr auto operator=(NullOpt_t&) noexcept -> decltype(auto) {
        this->has_value_ = false;
        return (*this);
    }
#endif

#if __cpp_explicit_this_parameter >= 202110L
    constexpr auto operator=(this auto&& self, Optional const& other) noexcept -> decltype(auto)
        requires (::std::is_copy_constructible_v<value_type>)
    {
        self.val_ = other.val_;
        self.has_value_ = other.has_value_;
        return (self);
    }
#else
    constexpr auto operator=(Optional const& other) noexcept -> decltype(auto)
        requires (::std::is_copy_constructible_v<value_type>)
    {
        val_ = other.val_;
        has_value_ = other.has_value_;
        return (*this);
    }
#endif

#if __cpp_explicit_this_parameter >= 202110L
    constexpr auto operator=(this auto&& self, Optional&& other) noexcept -> decltype(auto) {
        self.val_ = ::std::move(other.val_);
        self.has_value_ = ::std::move(other.has_value_);
        return (self);
    }
#else
    constexpr auto operator=(Optional&& other) noexcept -> decltype(auto) {
        val_ = ::std::move(other.val_);
        has_value_ = ::std::move(other.has_value_);
        return (*this);
    }
#endif

    [[nodiscard]]
#if __cpp_explicit_this_parameter >= 202110L
    constexpr bool has_value(this Optional const& self) noexcept {
        return self.has_value_;
    }
#else
    constexpr bool has_value() const noexcept {
        return this->has_value_;
    }
#endif

    /* Use has_value() instead
     */
    constexpr operator bool() noexcept = delete;

    [[nodiscard]]
#if __cpp_explicit_this_parameter >= 202110L
    constexpr auto value(this Optional& self) noexcept -> decltype(auto) {
    #if !defined(NDEBUG)
        if (self.has_value() == false) [[unlikely]] {
            terminate();
        }
    #endif  // !defined(NDEBUG)
        return (self.val_);
    }
#else
    constexpr auto value() & noexcept -> decltype(auto) {
    #if !defined(NDEBUG)
        if (this->has_value() == false) [[unlikely]] {
            terminate();
        }
    #endif  // !defined(NDEBUG)
        return (this->val_);
    }
#endif

    [[nodiscard]]
#if __cpp_explicit_this_parameter >= 202110L
    constexpr auto value(this Optional const& self) noexcept -> decltype(auto) {
    #if !defined(NDEBUG)
        if (self.has_value() == false) [[unlikely]] {
            terminate();
        }
    #endif  // !defined(NDEBUG)
        return (self.val_);
    }
#else
    constexpr auto value() const& noexcept -> decltype(auto) {
    #if !defined(NDEBUG)
        if (this->has_value() == false) [[unlikely]] {
            terminate();
        }
    #endif  // !defined(NDEBUG)
        return (this->val_);
    }
#endif

    [[nodiscard]]
#if __cpp_explicit_this_parameter >= 202110L
    constexpr auto value(this Optional&& self) noexcept -> decltype(auto) {
    #if !defined(NDEBUG)
        if (self.has_value() == false) [[unlikely]] {
            terminate();
        }
    #endif  // !defined(NDEBUG)
        return ::std::move(self.val_);
    }
#else
    constexpr auto value() && noexcept -> decltype(auto) {
    #if !defined(NDEBUG)
        if (this->has_value() == false) [[unlikely]] {
            terminate();
        }
    #endif  // !defined(NDEBUG)
        return ::std::move(this->val_);
    }
#endif

    [[nodiscard]]
#if __cpp_explicit_this_parameter >= 202110L
    constexpr auto value(this Optional const&& self) noexcept -> decltype(auto) {
    #if !defined(NDEBUG)
        if (self.has_value() == false) [[unlikely]] {
            terminate();
        }
    #endif  // !defined(NDEBUG)
        return ::std::move(self.val_);
    }
#else
    constexpr auto value() const&& noexcept -> decltype(auto) {
    #if !defined(NDEBUG)
        if (this->has_value() == false) [[unlikely]] {
            terminate();
        }
    #endif  // !defined(NDEBUG)
        return ::std::move(this->val_);
    }
#endif

    template<typename U>
        requires (::std::is_copy_constructible_v<value_type> && ::std::is_convertible_v<U, value_type>)
    [[nodiscard]]
#if __cpp_explicit_this_parameter >= 202110L
    constexpr auto value_or(this Optional const& self, U&& default_value) noexcept {
        if (self.has_value()) [[likely]] {
            return self.val_;
        } else {
            return static_cast<value_type>(::std::forward<U>(default_value));
        }
    }
#else
    constexpr auto value_or(U&& default_value) const& noexcept {
        if (this->has_value()) [[likely]] {
            return this->val_;
        } else {
            return static_cast<value_type>(::std::forward<U>(default_value));
        }
    }
#endif

    template<typename U>
        requires (::std::is_copy_constructible_v<value_type> && ::std::is_convertible_v<U, value_type>)
    [[nodiscard]]
#if __cpp_explicit_this_parameter >= 202110L
    constexpr auto value_or(this Optional&& self, U&& default_value) noexcept {
        if (self.has_value()) [[likely]] {
            return ::std::move(self.val_);
        } else {
            return static_cast<value_type>(::std::forward<U>(default_value));
        }
    }
#else
    constexpr auto value_or(U&& default_value) && noexcept {
        if (this->has_value()) [[likely]] {
            return ::std::move(this->val_);
        } else {
            return static_cast<value_type>(::std::forward<U>(default_value));
        }
    }
#endif

    /* No need to support, use .value() instead
     */
    constexpr auto operator*() noexcept = delete;
    constexpr auto operator->() noexcept = delete;

#if __cpp_explicit_this_parameter >= 202110L
    constexpr auto reset(this Optional& self) noexcept {
        if (self.has_value()) {
            self.~Optional();
            self.has_value_ = false;
        }
    }
#else
    constexpr void reset() noexcept {
        if (this->has_value()) {
            this->~Optional();
            this->has_value_ = false;
        }
    }
#endif

#if __cpp_explicit_this_parameter >= 202110L
    constexpr auto swap(this Optional& self, Optional& other) noexcept {
        using ::std::swap;
        swap(self.val_, other.val_);
        swap(self.has_value_, other.has_value_);
    }
#else
    constexpr void swap(Optional& other) noexcept {
        using ::std::swap;
        swap(this->val_, other.val_);
        swap(this->has_value_, other.has_value_);
    }
#endif

    template<typename U, typename... Args>
        requires (::std::is_constructible_v<value_type, U, Args...>)
    [[nodiscard]]
#if __cpp_explicit_this_parameter >= 202110L
    constexpr auto emplace(this auto&& self, U&& value, Args&&... args) noexcept {
        if (self.has_value()) [[unlikely]] {
            self.~Optional();
        }
        new (&self.val_) value_type(::std::forward<U>(value), ::std::forward<Args>(args)...);
        self.has_value_ = true;
    }
#else
    constexpr auto emplace(U&& value, Args&&... args) noexcept {
        if (this->has_value()) [[unlikely]] {
            this->~Optional();
        }
        new (&this->val_) value_type(::std::forward<U>(value), ::std::forward<Args>(args)...);
        this->has_value_ = true;
    }
#endif
};

}  // namespace ctb::exception
