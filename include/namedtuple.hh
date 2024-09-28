#if !__cpp_concepts >= 201907L
#   error "`namedtuple` requires at least c++20"
#endif

#include <algorithm>
#include <string_view>
#include <tuple>
#include <type_traits>

namespace namedtuple::details {

template<typename Char>
concept is_char =
    std::is_same_v<Char, char>
#if __cpp_char8_t >= 201811L
    || ::std::is_same_v<Char, char8_t>
#endif
    || std::is_same_v<Char, char16_t>
    || std::is_same_v<Char, char32_t>
    || std::is_same_v<Char, wchar_t>;


template<::std::size_t N, is_char Char>
struct template_str {
    using char_type = Char;
    Char str[N]{};

    constexpr template_str(Char const(&arr)[N]) {
        ::std::copy(arr, arr + N, str);
    }

    template<::std::size_t N_other, is_char Char_other>
    constexpr bool operator==(template_str<N_other, Char_other> const& other) const noexcept {
        if constexpr (N != N_other) {
            return false;
        } else {
            return ::std::equal(str, str + N, other.str);
        }
    }

    template<is_char Char_r>
    constexpr bool operator==(::std::basic_string_view<Char_r> const other) const noexcept {
        if constexpr (!::std::is_same_v<Char, Char_r>) {
            return false;
        } else {
            return other == str;
        }
    }
};

template<::std::size_t N, is_char Char_l, is_char Char_r>
constexpr bool operator==(::std::basic_string_view<Char_l> lhs, template_str<N, Char_r> const& rhs) noexcept {
    return rhs == lhs;
}

namespace names {

template<details::template_str First, details::template_str... Rest>
struct names : names<Rest...> {
    static constexpr ::std::basic_string_view<typename decltype(First)::char_type> current_val{First.str};
    using next_name = names<Rest...>;
};

template<details::template_str Str>
struct names<Str> {
    static constexpr ::std::basic_string_view<typename decltype(Str)::char_type> current_val{Str.str};
    using next_name = void;
};

template<typename>
constexpr bool is_names_ = false;

template<details::template_str... Str>
constexpr bool is_names_<names<Str...>> = true;

template<typename T>
concept is_names = is_names_<T>;

template<::std::size_t N, is_names Names, ::std::size_t index = 0>
consteval auto get_name() noexcept {
    if constexpr (index == N) {
        return Names::current_val;
    } else {
        static_assert(!::std::is_void_v<typename Names::next_name>, "index out of range");
        return get_name<N, typename Names::next_name, index + 1>();
    }
}

template<is_names Names, ::std::size_t counter = 0>
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

template<details::names::is_names Names, typename... Args>
    requires (details::names::get_size<Names>() == sizeof...(Args))
struct named_tuple {
    using names = Names;
    ::std::tuple<Args...> tuple;

    constexpr named_tuple(Args&&... args) {
        this->tuple = ::std::make_tuple(::std::forward<Args>(args)...);
    }
};

template<details::template_str... Str, typename... Args>
    requires (sizeof...(Str) == sizeof...(Args))
constexpr auto make_namedtuple(Args&&... args) noexcept {
    return named_tuple<names<Str...>, ::std::decay_t<Args>...>{::std::forward<Args>(args)...};
}

template<details::template_str str, ::std::size_t index = 0, details::names::is_names Names, typename... Args>
consteval auto get(named_tuple<Names, Args...> nt) noexcept {
    static_assert(index < details::names::get_size<Names>(), "index out of range");
    if constexpr (details::names::get_name<index, Names>() == str) {
        return ::std::get<index>(nt.tuple);
    } else {
        return get<str, index + 1>(nt);
    }
}

template<::std::size_t N, details::names::is_names Names, typename... Args>
consteval auto get(named_tuple<Names, Args...> nt) noexcept {
    return ::std::get<N>(nt.tuple);
}

} // namespace namedtuple

