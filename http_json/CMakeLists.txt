add_executable(HttpJson main.cpp)
target_link_libraries(HttpJson PRIVATE nlohmann_json::nlohmann_json Boost::boost)