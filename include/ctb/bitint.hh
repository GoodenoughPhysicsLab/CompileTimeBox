#pragma once

#if __cpp_concepts < 201907L
    #error "`ctb` requires at least C++20"
#endif  // __cpp_concepts < 201907L

#include <cstddef>
#include <type_traits>
#include "vector.hh"

namespace ctb::bitint {

template<::std::size_t N>
struct bitint {
    static_assert(N > 0);

    bool flag_{};
    ctb::vector::vector<::std::size_t, N / (8 * sizeof(::std::size_t)) + 1> data_{};

    constexpr bitint() noexcept = default;

    constexpr bitint(::std::ptrdiff_t val) noexcept
        : flag_{val < 0}, data_{val} {
    }

    template<typename... Args>
        requires (::std::is_convertible_v<Args, ::std::size_t> && ...)
    constexpr bitint(bool flag_, Args const&... args) noexcept
        : flag_{flag_}, data_{static_cast<::std::size_t>(args)...} {
        exception::assert_true(((args >= 0) && ...));
    }
};

}  // namespace ctb::bitint
