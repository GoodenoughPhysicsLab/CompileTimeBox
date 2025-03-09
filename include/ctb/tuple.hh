#pragma once

#if __cpp_concepts < 201907L
    #error "`ctb` requires at least C++20"
#endif // __cpp_concepts < 201907L

#include <cstddef>
#include <type_traits>
#include <utility>

namespace ctb::tuple {

namespace details {

#if __cpp_pack_indexing < 202311L

template<::std::size_t I, typename First, typename... Rest>
    requires (0 <= I && I <= sizeof...(Rest))
struct pack_indexing_before_cxx26_ {
    using type = typename pack_indexing_before_cxx26_<I - 1, Rest...>::type;
};

template<typename First, typename... Rest>
struct pack_indexing_before_cxx26_<0, First, Rest...> {
    using type = First;
};

#endif // __cpp_pack_indexing < 202311L

template<::std::size_t I, typename... Args>
struct pack_indexing_ {
#if __cpp_pack_indexing >= 202311L
    #if defined(__clang__)
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wc++26-extensions"
    #endif
    using type = Args...[I];
    #if defined(__clang__)
        #pragma clang diagnostic pop
    #endif
#else // ^^^ __cpp_pack_indexing >= 202311L / vvv __cpp_pack_indexing < 202311L
    using type = typename pack_indexing_before_cxx26_<I, Args...>::type;
#endif // __cpp_pack_indexing < 202311L
};

template<::std::size_t I, typename... Args>
    requires (I < sizeof...(Args))
using pack_indexing_t_ = typename pack_indexing_<I, Args...>::type;

template<::std::size_t I, typename... Args>
    requires (I < sizeof...(Args))
struct tuple_element_impl_ {
    pack_indexing_t_<I, Args...> val_;
};

template<typename T>
struct pass_type_ {
    using type = T;
};

template<typename... Args, ::std::size_t... Index>
    requires (sizeof...(Args) == sizeof...(Index))
[[nodiscard]]
constexpr auto get_tuple_impl_(::std::index_sequence<Index...>) noexcept {
    struct tuple_impl_ : tuple_element_impl_<Index, Args...>... {};

    return pass_type_<tuple_impl_>();
}

} // namespace details

template<typename... Args>
struct tuple : ::std::remove_cvref_t<typename decltype(details::get_tuple_impl_<Args...>(
                   ::std::make_index_sequence<sizeof...(Args)>{}))::type> {};

template<>
struct tuple<> {};

template<::std::size_t I, typename... Args>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
[[nodiscard]]
constexpr auto get(tuple<Args...> const& self) noexcept -> decltype(auto) {
    return static_cast<details::tuple_element_impl_<I, Args...> const&>(self).val_;
}

template<::std::size_t I, typename... Args>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
[[nodiscard]]
constexpr auto get(tuple<Args...> const&& self) noexcept -> decltype(auto) {
    return ::std::move(static_cast<details::tuple_element_impl_<I, Args...> const&&>(self).val_);
}

template<typename... Args>
tuple(Args&&...) -> tuple<Args...>;

namespace details {

template<typename T>
constexpr bool is_tuple_ = false;

template<typename... Args>
constexpr bool is_tuple_<tuple<Args...>> = true;

} // namespace details

template<typename T>
concept is_tuple = details::is_tuple_<::std::remove_cvref_t<T>>;

} // namespace ctb::tuple

template<::std::size_t I, typename... Args>
struct std::tuple_element<I, ::ctb::tuple::tuple<Args...>> {
    using type = ::ctb::tuple::details::pack_indexing_t_<I, Args...>;
};

template<typename... Args>
struct std::tuple_size<::ctb::tuple::tuple<Args...>> {
    static constexpr ::std::size_t value = sizeof...(Args);
};
