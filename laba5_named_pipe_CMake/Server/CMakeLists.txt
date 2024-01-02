cmake_minimum_required(VERSION 3.23)
project(Server)

add_executable(Server server.cpp)

set_target_properties(Server PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build")