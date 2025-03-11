#pragma once

#if __cpp_concepts < 201907L
    #error "`ctb` requires at least C++20"
#endif // __cpp_concepts < 201907L

#include "tuple.hh"
#include <type_traits>

#ifdef CTB_N_STL_SUPPORT
    #include "string.hh"
#else
    #define CTB_N_STL_SUPPORT
    #include "string.hh"
    #undef CTB_N_STL_SUPPORT
#endif

namespace ctb::namedtuple::details {

template<string::string First, string::string... Rest>
struct names : names<Rest...> {
    static constexpr auto current_val{First};
    using next_name = names<Rest...>;
};

template<string::string Str>
struct names<Str> {
    static constexpr auto current_val{Str};
    using next_name = void;
};

template<typename>
constexpr bool is_names_ = false;

template<string::string... Str>
constexpr bool is_names_<names<Str...>> = true;

template<typename T>
concept is_names = is_names_<::std::remove_cvref_t<T>>;

template<::std::size_t N, is_names Names, ::std::size_t index_ = 0>
[[nodiscard]]
consteval auto get_name() noexcept {
    if constexpr (index_ == N) {
        return Names::current_val;
    } else {
        static_assert(!::std::is_void_v<typename Names::next_name>, "index out of range");
        return get_name<N, typename Names::next_name, index_ + 1>();
    }
}

template<is_names Names, ::std::size_t counter = 0>
[[nodiscard]]
consteval ::std::size_t get_size_() noexcept {
    if constexpr (::std::is_void_v<typename Names::next_name>) {
        return counter + 1;
    } else {
        return get_size_<typename Names::next_name, counter + 1>();
    }
}

template<is_names Names>
[[nodiscard]]
consteval ::std::size_t get_size() noexcept {
    return get_size_<Names>();
}

} // namespace ctb::namedtuple::details

namespace ctb::namedtuple {

template<string::string... Args>
using names = details::names<Args...>;

template<details::is_names Names, typename... Args>
    requires (details::get_size<Names>() == sizeof...(Args))
struct namedtuple {
    using names = Names;
    tuple::tuple<Args...> tuple;

    constexpr namedtuple(Args&&... args) {
#if defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wmissing-braces"
#endif
        this->tuple = tuple::tuple{::std::forward<Args>(args)...};
#if defined(__clang__)
    #pragma clang diagnostic pop
#endif
    }

    constexpr ~namedtuple() noexcept = default;
};

template<string::string... Str, typename... Args>
    requires (sizeof...(Str) == sizeof...(Args))
[[nodiscard]]
constexpr auto make_namedtuple(Args&&... args) noexcept {
    return namedtuple<names<Str...>, ::std::decay_t<Args>...>{::std::forward<Args>(args)...};
}

/* get namedtuple element by name
 *
 * Usage: get<"name">(nt)
 */
template<string::string str, ::std::size_t index = 0, details::is_names Names, typename... Args>
[[nodiscard]]
constexpr auto get(namedtuple<Names, Args...> nt) noexcept {
    static_assert(index < details::get_size<Names>(), "index out of range");
    if constexpr (details::get_name<index, Names>() == str) {
        return tuple::get<index>(nt.tuple);
    } else {
        return get<str, index + 1>(nt);
    }
}

/* get namedtuple element by index
 *
 * Usage: get<1>(nt)
 */
template<::std::size_t N, details::is_names Names, typename... Args>
[[nodiscard]]
constexpr auto get(namedtuple<Names, Args...> nt) noexcept {
    return tuple::get<N>(nt.tuple);
}

} // namespace ctb::namedtuple

/* C++17 structured binding support
 */
template<::ctb::namedtuple::details::is_names Names, typename... Args>
struct std::tuple_size<::ctb::namedtuple::namedtuple<Names, Args...>>
    : public ::std::integral_constant<::std::size_t, ::ctb::namedtuple::details::get_size<Names>()> {};

template<::std::size_t N, ::ctb::namedtuple::details::is_names Names, typename... Args>
struct std::tuple_element<N, ::ctb::namedtuple::namedtuple<Names, Args...>> {
    using type = decltype(::ctb::tuple::get<N>(::std::declval<::ctb::namedtuple::namedtuple<Names, Args...>>().tuple));
};
