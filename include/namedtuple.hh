#pragma once

#if !__cpp_concepts >= 201907L
    #error "namedtuple requires at least c++20"
#endif

#include <tuple>
#include <type_traits>

#include "metastr.hh"

namespace namedtuple::details {

template<metastr::metastr First, metastr::metastr... Rest>
struct names : names<Rest...> {
    static constexpr ::std::basic_string_view<typename decltype(First)::value_type> current_val{First.str};
    using next_name = names<Rest...>;
};

template<metastr::metastr Str>
struct names<Str> {
    static constexpr ::std::basic_string_view<typename decltype(Str)::value_type> current_val{Str.str};
    using next_name = void;
};

template<typename>
constexpr bool is_names_ = false;

template<metastr::metastr... Str>
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

}  // namespace namedtuple::details

namespace namedtuple {

template<metastr::metastr... Args>
using names = details::names<Args...>;

template<details::is_names Names, typename... Args>
    requires (details::get_size<Names>() == sizeof...(Args))
struct namedtuple {
    using names = Names;
    ::std::tuple<Args...> tuple;

    constexpr namedtuple(Args&&... args) {
        this->tuple = ::std::make_tuple(::std::forward<Args>(args)...);
    }
};

template<metastr::metastr... Str, typename... Args>
    requires (sizeof...(Str) == sizeof...(Args))
[[nodiscard]]
constexpr auto make_namedtuple(Args&&... args) noexcept {
    return namedtuple<names<Str...>, ::std::decay_t<Args>...>{::std::forward<Args>(args)...};
}

/* get namedtuple element by name
 *
 * Usage: get<"name">(nt)
 */
template<metastr::metastr str, ::std::size_t index = 0, details::is_names Names, typename... Args>
[[nodiscard]]
constexpr auto get(namedtuple<Names, Args...> nt) noexcept {
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
constexpr auto get(namedtuple<Names, Args...> nt) noexcept {
    return ::std::get<N>(nt.tuple);
}

}  // namespace namedtuple

/* C++17 structured binding support
 */
namespace std {

template<::namedtuple::details::is_names Names, typename... Args>
struct tuple_size<::namedtuple::namedtuple<Names, Args...>>
    : public ::std::integral_constant<::std::size_t, ::namedtuple::details::get_size<Names>()> {};

template<::std::size_t N, namedtuple::details::is_names Names, typename... Args>
struct tuple_element<N, ::namedtuple::namedtuple<Names, Args...>> {
    using type = decltype(::std::get<N>(::std::declval<::namedtuple::namedtuple<Names, Args...>>().tuple));
};

}  // namespace std
