#pragma once

#include <utility>
#include <type_traits>

namespace ctb::utils {

template<class T, class U>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
constexpr auto&& forward_like(U&& x) noexcept {
    constexpr bool is_adding_const = std::is_const_v<std::remove_reference_t<T>>;
    if constexpr (std::is_lvalue_reference_v<T&&>) {
        if constexpr (is_adding_const) {
            return std::as_const(x);
        } else {
            return static_cast<U&>(x);
        }
    } else {
        if constexpr (is_adding_const) {
            return std::move(std::as_const(x));
        } else {
            return std::move(x);
        }
    }
}

namespace details {

#if !defined(__cpp_pack_indexing) || __cpp_pack_indexing < 202311L

template<::std::size_t I, typename First, typename... Rest>
    requires (I <= sizeof...(Rest))
struct pack_indexing_before_cxx26_ {
    using type = typename ::ctb::utils::details::pack_indexing_before_cxx26_<I - 1, Rest...>::type;
};

template<typename First, typename... Rest>
struct pack_indexing_before_cxx26_<0, First, Rest...> {
    using type = First;
};

#endif // !defined(__cpp_pack_indexing) || __cpp_pack_indexing < 202311L

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
    using type = typename ::ctb::utils::details::pack_indexing_before_cxx26_<I, Args...>::type;
#endif // ^^^ __cpp_pack_indexing < 202311L
};

} // namespace details

template<::std::size_t I, typename... Args>
    requires (I < sizeof...(Args))
using pack_indexing_t = typename ::ctb::utils::details::pack_indexing_<I, Args...>::type;

template<typename T>
struct pass_type {
    using type = T;
};

} // namespace ctb::utils
