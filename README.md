# namedtuple

in some languages like Python, `namedtuple` is a built-in feature.

So, can users use namedtuple in C++? the answer is YES!

`namedtuple` is a C++20, header-only library.

## Usage

```cpp
#include <string>
#include "namedtuple.hh"

void example() noexcept {
    constexpr auto nt = namedtuple::namedtuple<namedtuple::names<"a", "b", "hhh">, int, int, ::std::string>{1, 2, "hello"};
    static_assert(namedtuple::get<"a">(nt) == 1);
    static_assert(namedtuple::get<"hhh">(nt) == "hello");
}
```
