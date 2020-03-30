add_executable(hexDump hexdump.cpp)
target_compile_features(hexDump PRIVATE cxx_std_17)
target_link_libraries(hexDump fmt::fmt)
