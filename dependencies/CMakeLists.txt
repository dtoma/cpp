include(FetchContent)

FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.10.0
)

FetchContent_Declare(fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 6.1.2
)

FetchContent_Declare(benchmark
    GIT_REPOSITORY https://github.com/google/benchmark
)

FetchContent_Declare(regex
    GIT_REPOSITORY https://github.com/google/re2
    GIT_TAG 2020-04-01
)

# Otherwise we get big long LNK2038 errors on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

set(BENCHMARK_ENABLE_TESTING OFF)

# This doesn't work:
# set(RE2_BUILD_TESTING OFF)
# We have the following error message, and the tests still run:
#   CMake Warning (dev) at build/_deps/regex-src/CMakeLists.txt:21 (option):
#     Policy CMP0077 is not set: option() honors normal variables.  Run "cmake
#     --help-policy CMP0077" for policy details.  Use the cmake_policy command to
#     set the policy and suppress this warning.
#
#     For compatibility with older versions of CMake, option is clearing the
#     normal variable 'RE2_BUILD_TESTING'.
#   This warning is for project developers.  Use -Wno-dev to suppress it.
# This works:
set(RE2_BUILD_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(
    googletest
    benchmark
    fmt
    regex
)
