# namedtuple

in some languages like Python, `namedtuple` is a built-in feature.

So, can users use namedtuple in C++? the answer is YES!

`namedtuple` is a C++20, header-only library. Support `MSVC`, `g++`, `clang++`.

## Usage

```cpp
#include <string_view>
#include "namedtuple.hh"

void example() noexcept {
    constexpr auto nt = namedtuple::make_namedtuple<"a", "b", "hhh">(1, 2, "hello");
    static_assert(namedtuple::get<"a">(nt) == 1);
    static_assert(::std::string_view{namedtuple::get<"hhh">(nt)} == "hello");
}
```
