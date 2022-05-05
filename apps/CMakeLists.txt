add_executable(part2 part2.cpp)
target_link_libraries(part2 rtow pthread)
set_property(TARGET part2 PROPERTY CXX_STANDARD 20)

add_executable(part4 part4.cpp)
target_link_libraries(part4 rtow pthread)
set_property(TARGET part4 PROPERTY CXX_STANDARD 20)

add_executable(part5 part5.cpp)
target_link_libraries(part5 rtow pthread)
set_property(TARGET part5 PROPERTY CXX_STANDARD 20)