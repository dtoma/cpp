cmake_minimum_required(VERSION 3.7)
project(string)

enable_testing()

include(FetchContent)

FetchContent_Declare(googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.10.0
)

# Otherwise we get big long LNK2038 errors on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 6.1.2
)

FetchContent_MakeAvailable(googletest fmt)

include(GoogleTest)

add_subdirectory(string)
add_subdirectory(printers)