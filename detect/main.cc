#ifdef __cplusplus
    #pragma message("[[Detect]] C++: Success")
#else
    #error "[[Detect]] C++: Failed"
#endif

#if __cpp_concepts >= 201907L
    #pragma message("[[Detect]] C++20 Concepts: Success")
#else
    #error "[[Detect]] C++20 Concepts: Failed"
#endif

#if defined(_MSC_VER) && !defined(__clang__)
    #pragma message("[[Detect]] Compiler: MSVC")
#elif defined(__GNUC__) && !defined(__clang__)
    #pragma message("[[Detect]] Compiler: GCC")
#elif defined(__clang__)
    #pragma message("[[Detect]] Compiler: Clang")
#else
    #error "[[Detect]] Compiler: Unknown"
#endif

#if defined(_MSC_VER) && !defined(__clang__)
    #if _MSVC_LANG >= 202302L
        #pragma message("[[Detect]] C++ standard: C++23")
    #elif _MSVC_LANG >= 202002L
        #pragma message("[[Detect]] C++ standard: C++20")
    #else
        #error "[[Detect]] C++ standard: less than C++20"
    #endif
#else
    #if __cplusplus >= 202302L
        #pragma message("[[Detect]] C++ standard: C++23")
    #elif __cplusplus >= 202002L
        #pragma message("[[Detect]] C++ standard: C++20")
    #else
        #error "[[Detect]] C++ standard: less than C++20"
    #endif
#endif

#if __cpp_explicit_this_parameter >= 202110L
    #pragma message("[[Detect]] explicit-this-parameter: Success")
#else
    #pragma message("[[Detect]] explicit-this-parameter: Failed")
#endif

#if __cpp_pack_indexing >= 202311L
    #pragma message("[[Detect]] pack-indexing: Success")
#else
    #pragma message("[[Detect]] pack-indexing: Failed")
#endif

int main() noexcept {
    return 0;
}
