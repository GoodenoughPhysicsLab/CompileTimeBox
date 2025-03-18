#pragma once

#if __cpp_concepts < 201907L
    #error "`ctb` requires at least C++20"
#endif // __cpp_concepts < 201907L

#include <cstddef>
#include <concepts>
#include <utility>
#include "utils.hh"

namespace ctb::tuple {

namespace details {

template<::std::size_t I, typename T>
struct tuple_element_impl_ {
#if __has_cpp_attribute(msvc::no_unique_address)
    [[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address)
    [[no_unique_address]]
#endif
    T val_;
};

template<typename... Args, ::std::size_t... Index>
    requires (sizeof...(Args) == sizeof...(Index))
[[nodiscard]]
constexpr auto get_tuple_impl_(::std::index_sequence<Index...>) noexcept {
    struct tuple_impl_ : tuple_element_impl_<Index, Args>... {};

    return ::ctb::utils::pass_type<tuple_impl_>();
}

}

template<typename... Args>
struct tuple : ::std::remove_cvref_t<typename decltype(::ctb::tuple::details::get_tuple_impl_<Args...>(
                   ::std::make_index_sequence<sizeof...(Args)>{}))::type> {};

template<>
struct tuple<> {};

template<typename... Args>
tuple(Args&&...) -> tuple<Args...>;

template<::std::size_t I, typename... Args>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
[[nodiscard]]
constexpr auto&& get(::ctb::tuple::tuple<Args...> const& self) noexcept {
    return static_cast<::ctb::tuple::details::tuple_element_impl_<
        I, ::ctb::utils::pack_indexing_t<I, Args...>> const&>(self)
        .val_;
}

template<::std::size_t I, typename... Args>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
[[nodiscard]]
constexpr auto&& get(::ctb::tuple::tuple<Args...> const&& self) noexcept {
    return ::std::move(
        static_cast<
            ::ctb::tuple::details::tuple_element_impl_<I, ::ctb::utils::pack_indexing_t<I, Args...>> const&&>(
            self)
            .val_);
}

namespace details {

template<typename T, ::std::size_t I, typename Current, typename... Args>
constexpr auto get_tuple_element_by_type_() noexcept {
    if constexpr (::std::same_as<T, Current>) {
        return ::ctb::utils::pass_type<tuple_element_impl_<I, Current>>{};
    } else {
        return ::ctb::tuple::details::get_tuple_element_by_type_<T, I + 1, Args...>();
    }
}

} // namespace details

template<typename T, typename... Args>
    requires ((::std::same_as<T, Args> + ...) == 1)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
[[nodiscard]]
constexpr auto&& get(::ctb::tuple::tuple<Args...> const& self) noexcept {
    return static_cast<decltype(::ctb::tuple::details::get_tuple_element_by_type_<T, 0, Args...>())::type const&>(self)
        .val_;
}

template<typename T, typename... Args>
    requires ((::std::same_as<T, Args> + ...) == 1)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
[[nodiscard]]
constexpr auto&& get(::ctb::tuple::tuple<Args...> const&& self) noexcept {
    return ::std::move(
        static_cast<decltype(::ctb::tuple::details::get_tuple_element_by_type_<T, 0, Args...>())::type const&&>(self)
            .val_);
}

namespace details {

template<typename T>
constexpr bool is_tuple_ = false;

template<typename... Args>
constexpr bool is_tuple_<tuple<Args...>> = true;

} // namespace details

template<typename T>
concept is_tuple = ::ctb::tuple::details::is_tuple_<::std::remove_cvref_t<T>>;

template<typename... Args>
[[nodiscard]]
constexpr auto forward_as_tuple(Args&&... args) {
#if defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wmissing-braces"
#endif
    return ::ctb::tuple::tuple<Args&&...>(::std::forward<Args>(args)...);
#if defined(__clang__)
    #pragma clang diagnostic pop
#endif
}

} // namespace ctb::tuple

template<::std::size_t I, typename... Args>
struct std::tuple_element<I, ::ctb::tuple::tuple<Args...>> {
    using type = ::ctb::utils::pack_indexing_t<I, Args...>;
};

template<typename... Args>
struct std::tuple_size<::ctb::tuple::tuple<Args...>> {
    static constexpr ::std::size_t value = sizeof...(Args);
};
