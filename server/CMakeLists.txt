cmake_minimum_required (VERSION 3.2)
project (server)

add_executable(server server.cpp)

if (WIN32)
    target_link_libraries(server wsock32 ws2_32)
endif (WIN32)
