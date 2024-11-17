#ifdef NDEBUG
#   undef NDEBUG
#endif

#include "test_metastr.hh"
#include "test_namedtuple.hh"

int main() noexcept {
    runtime_test_metastr_eq();
    return 0;
}
