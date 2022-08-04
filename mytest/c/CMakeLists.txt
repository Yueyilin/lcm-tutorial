cmake_minimum_required(VERSION 3.5)

project(my_lcm_c_test)

find_package(lcm REQUIRED)

add_subdirectory(mylcm)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(my_lcm_sub my_lcm_sub.c)

target_link_libraries(my_lcm_sub lcm libmylcm)


add_executable(my_lcm_pub my_lcm_pub.c)

target_link_libraries(my_lcm_pub lcm libmylcm)
