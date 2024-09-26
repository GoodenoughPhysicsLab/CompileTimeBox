#include <algorithm>
#include <string_view>
#include <tuple>
#include <type_traits>

namespace namedtuple::details {

template<::std::size_t N> // TODO: support other encoding like utf-8
struct template_str {
    char str[N]{};
    constexpr template_str(char const(&arr)[N]) {
        ::std::copy(arr, arr + N, str);
    }

    constexpr bool operator==(template_str const& other) const noexcept {
        return ::std::equal(str, str + N, other.str);
    }

    constexpr bool operator==(::std::string_view other) const noexcept {
        return other == str;
    }
};

template<::std::size_t N>
constexpr bool operator==(::std::string_view lhs, template_str<N> const& rhs) noexcept {
    return rhs == lhs;
}

template<typename>
constexpr bool is_tuple = false;

template<typename... Args>
constexpr bool is_tuple<::std::tuple<Args...>> = true;

namespace names {

template<details::template_str First, details::template_str... Rest>
struct names : names<Rest...> {
    static constexpr ::std::string_view current_val{First.str};
    using next_name = names<Rest...>;
};

template<details::template_str Str>
struct names<Str> {
    static constexpr ::std::string_view current_val{Str.str};
    using next_name = void;
};

template<typename>
constexpr bool is_names = false;

template<details::template_str... Str>
constexpr bool is_names<names<Str...>> = true;

template<::std::size_t N, typename Names, ::std::size_t index = 0>
    requires (is_names<Names>)
consteval ::std::string_view get_name() noexcept {
    if constexpr (index == N) {
        return Names::current_val;
    } else {
        static_assert(!::std::is_void_v<typename Names::next_name>, "index out of range");
        return get_name<N, typename Names::next_name, index + 1>();
    }
}

template<typename Names, ::std::size_t counter = 0>
    requires (is_names<Names>)
consteval ::std::size_t get_size() noexcept {
    if constexpr (::std::is_void_v<typename Names::next_name>) {
        return counter + 1;
    } else {
        return get_size<typename Names::next_name, counter + 1>();
    }
}

} // namespace names

} // namespace namedtuple::details

namespace namedtuple {

template<details::template_str... Args>
using names = details::names::names<Args...>;

template<typename Names, typename... Args>
    requires (details::names::is_names<Names>)
struct named_tuple {
    using names = Names;
    ::std::tuple<Args...> tuple_;

    constexpr named_tuple(Args&&... args) {
        this->tuple_ = ::std::make_tuple(::std::forward<Args>(args)...);
    }
};

template<details::template_str str, ::std::size_t index = 0, typename Names, typename... Args>
    requires (details::names::is_names<Names>)
consteval auto get(named_tuple<Names, Args...> nt) noexcept {
    if constexpr (details::names::get_name<index, Names>() == str) {
        return ::std::get<index>(nt.tuple_);
    } else {
        return get<str, index + 1>(nt);
    }
}

} // namespace namedtuple
