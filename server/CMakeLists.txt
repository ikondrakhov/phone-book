cmake_minimum_required (VERSION 3.2)
project (server)

set(SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SOURCE ${SRC}/socket.cpp 
            ${SRC}/server.cpp 
            ${SRC}/connection_processor.cpp 
            ${SRC}/contact.cpp 
            ${SRC}/file_contacts_storage.cpp)

add_executable(server main.cpp ${SOURCE})
target_include_directories(server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (WIN32)
    target_link_libraries(server wsock32 ws2_32)
endif (WIN32)
