cmake_minimum_required(VERSION 3.17)

project(main)

add_executable(main glad/glad.c main.cpp)

target_link_libraries(main GL dl glfw)
