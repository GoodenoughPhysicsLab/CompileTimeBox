
#include <ctb/bitint.hh>

using namespace ctb::bitint;

consteval void test_init() noexcept {
    [[maybe_unused]] constexpr bitint<8> a{};
    [[maybe_unused]] constexpr bitint<8> b{1};
    [[maybe_unused]] constexpr bitint<sizeof(::std::size_t) * 16> c{false, 1u, 1u};

    static_assert(ctb::vector::get_value(c.data_, 1) == 1);
}

int main() noexcept {
    return 0;
}
