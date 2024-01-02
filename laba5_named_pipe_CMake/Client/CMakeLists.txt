cmake_minimum_required(VERSION 3.23)
project(Server)

add_executable(Client client.cpp)

set_target_properties(Client PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build")