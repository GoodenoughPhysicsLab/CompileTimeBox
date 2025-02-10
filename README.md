# CompileTimeBox

This repository contains some useful compile-time tools for C++.

every feature is header-only, requires at least C++20, support `cl`, `g++`, `clang++`, `Windows`, `Linux`, `MacOS`.
(even freestanding? I'm not sure, but it's accually possible)

## namedtuple

in some languages like Python, `namedtuple` is a built-in feature.

So, can users use namedtuple in C++? the answer is YES!

usage:
```cpp
#include <string_view>
#include <ctb/namedtuple.hh>

using namespace ctb::namedtuple;

void example() noexcept {
    constexpr auto nt = make_namedtuple<"a", "b", u8"hhh">(1, 2, "hello");
    static_assert(get<"a">(nt) == 1);
    static_assert(::std::string_view{get<u8"hhh">(nt)} == "hello");
    auto [a, b, hhh] = nt;
}
```

show more examples in [test_namedtuple](./test/namedtuple.cc).

## vector
show more examples in [test_vector](./test/vector.cc).

## string
To support use string in compile time (even template), I wrote `string`.
```cpp
#include <ctb/string.hh>

using namespace ctb::string;

void example() noexcept {
    constexpr auto str1 = string{"hello"};
    constexpr auto str2 = string{"world"};
    static_assert(str1 == "hello"); // also support ::std::string, ::std::string_view and other encoding
    static_assert(concat(str1, str2) == "helloworld");
}
```

show more examples in [test_string](./test/string.cc).
