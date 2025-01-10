#pragma once

#if !__cpp_concepts >= 201907L
    #error "`ctb` requires at least C++20"
#endif  // !__cpp_concepts >= 201907L

#include <tuple>
#include <type_traits>

#ifdef CTB_N_STL_SUPPORT
    #include "string.hh"
#else
    #define CTB_N_STL_SUPPORT
    #include "string.hh"
    #undef CTB_N_STL_SUPPORT
#endif

namespace ctb::namedtuple::details {

template<string::String First, string::String... Rest>
struct names : names<Rest...> {
    static constexpr auto current_val{First};
    using next_name = names<Rest...>;
};

template<string::String Str>
struct names<Str> {
    static constexpr auto current_val{Str};
    using next_name = void;
};

template<typename>
constexpr bool is_names_ = false;

template<string::String... Str>
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
consteval ::std::size_t get_size() noexcept {
    if constexpr (::std::is_void_v<typename Names::next_name>) {
        return counter + 1;
    } else {
        return get_size<typename Names::next_name, counter + 1>();
    }
}

}  // namespace ctb::namedtuple::details

namespace ctb::namedtuple {

template<string::String... Args>
using names = details::names<Args...>;

template<details::is_names Names, typename... Args>
    requires (details::get_size<Names>() == sizeof...(Args))
struct NamedTuple {
    using names = Names;
    ::std::tuple<Args...> tuple;

    constexpr NamedTuple(Args&&... args) {
        this->tuple = ::std::make_tuple(::std::forward<Args>(args)...);
    }
};

template<string::String... Str, typename... Args>
    requires (sizeof...(Str) == sizeof...(Args))
[[nodiscard]]
constexpr auto make_namedtuple(Args&&... args) noexcept {
    return NamedTuple<names<Str...>, ::std::decay_t<Args>...>{::std::forward<Args>(args)...};
}

/* get namedtuple element by name
 *
 * Usage: get<"name">(nt)
 */
template<string::String str, ::std::size_t index = 0, details::is_names Names, typename... Args>
[[nodiscard]]
constexpr auto get(NamedTuple<Names, Args...> nt) noexcept {
    static_assert(index < details::get_size<Names>(), "index out of range");
    if constexpr (details::get_name<index, Names>() == str) {
        return ::std::get<index>(nt.tuple);
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
constexpr auto get(NamedTuple<Names, Args...> nt) noexcept {
    return ::std::get<N>(nt.tuple);
}

}  // namespace ctb::namedtuple

/* C++17 structured binding support
 */
namespace std {

template<::ctb::namedtuple::details::is_names Names, typename... Args>
struct tuple_size<::ctb::namedtuple::NamedTuple<Names, Args...>>
    : public ::std::integral_constant<::std::size_t, ::ctb::namedtuple::details::get_size<Names>()> {};

template<::std::size_t N, ::ctb::namedtuple::details::is_names Names, typename... Args>
struct tuple_element<N, ::ctb::namedtuple::NamedTuple<Names, Args...>> {
    using type = decltype(::std::get<N>(::std::declval<::ctb::namedtuple::NamedTuple<Names, Args...>>().tuple));
};

}  // namespace std
