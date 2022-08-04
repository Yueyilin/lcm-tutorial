cmake_minimum_required(VERSION 3.1)

project(my_lcm_test)

find_package(lcm REQUIRED)

add_executable(my_lcm_sub my_lcm_sub.cpp)

add_executable(my_lcm_pub my_lcm_pub.cpp)

add_executable(my_lcm_readlog my_lcm_readlog.cpp)

target_link_libraries(my_lcm_sub lcm)

target_link_libraries(my_lcm_pub lcm)

target_link_libraries(my_lcm_readlog lcm)
