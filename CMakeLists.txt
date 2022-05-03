cmake_minimum_required(VERSION 3.13)

project(RTOW LANGUAGES CXX)
set(CXX_STANDARD_REQUIRED 17)

SET(SRCS )
add_library(rtow INTERFACE ${SRCS})
# target_link_libraries()
target_include_directories(rtow INTERFACE include)
set_property(TARGET rtow PROPERTY CXX_STANDARD 20)

add_executable(test_vec3 test/test_vec3.cpp)
target_link_libraries(test_vec3 rtow)
set_property(TARGET test_vec3 PROPERTY CXX_STANDARD 20)







