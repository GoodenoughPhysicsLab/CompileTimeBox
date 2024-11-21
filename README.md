# CompileTimeBox

This repository contains some useful compile-time tools for C++.

every feature is header-only, requires C++20, tested on `MSVC`, `g++`, `clang++`. (even freestanding? I'm not sure, but it's accually possible)

## namedtuple

in some languages like Python, `namedtuple` is a built-in feature.

So, can users use namedtuple in C++? the answer is YES!

usage:
```cpp
#include <string_view>
#include <ctb/namedtuple.hh>

void example() noexcept {
    constexpr auto nt = ctb::namedtuple::make_namedtuple<"a", "b", u8"hhh">(1, 2, "hello");
    static_assert(ctb::namedtuple::get<"a">(nt) == 1);
    static_assert(::std::string_view{ctb::namedtuple::get<u8"hhh">(nt)} == "hello");
    auto [a, b, hhh] = nt;
}
```

show more examples in [test_namedtuple](./test/test_namedtuple.cc).

## string
To support use string in compile time (even template), I wrote `string`.
```cpp
#include <ctb/string.hh>

void example() noexcept {
    constexpr auto str1 = ctb::string::string{"hello"};
    constexpr auto str2 = ctb::string::string{"world"};
    static_assert(str1 == "hello"); // also support ::std::string, ::std::string_view and other encoding
    static_assert(ctb::string::concat(str1, str2) == "helloworld");
}
```

show more examples in [test_string](./test/test_string.cc).
