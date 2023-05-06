cmake_minimum_required(VERSION 2.8...3.13)

project(vits-cli)

set(CMAKE_CXX_STANDARD 20)
add_executable(vits-cli vitsdemo.cpp utils.cpp)
target_link_libraries(vits-cli ${CMAKE_SOURCE_DIR}/../build/libvits-ncnn.so)
