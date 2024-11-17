# metastr & namedtuple

in some languages like Python, `namedtuple` is a built-in feature.

So, can users use namedtuple in C++? the answer is YES!

`namedtuple` is a C++20, header-only library. Support `MSVC`, `g++`, `clang++`.

## Usage
```cpp
#include <string_view>
#include <namedtuple.hh>

void example() noexcept {
    constexpr auto nt = namedtuple::make_namedtuple<"a", "b", u8"hhh">(1, 2, "hello");
    static_assert(namedtuple::get<"a">(nt) == 1);
    static_assert(::std::string_view{namedtuple::get<u8"hhh">(nt)} == "hello");
    auto [a, b, hhh] = nt;
}
```
show more usage in [test_namedtuple.hh](./test/test_namedtuple.hh).

## metastr
To support use string in compile time (even template), I wrote `metastr`.
```cpp
#include <metastr.hh>

void example() noexcept {
    constexpr auto str1 = metastr::metastr{"hello"};
    constexpr auto str2 = metastr::metastr{"world"};
    static_assert(str1 == "hello"); // also support ::std::string, ::std::string_view and other encoding
    static_assert(metastr::concat(str1, str2) == "helloworld");
}
```
show more usage in [test_metastr.hh](./test/test_metastr.hh).
