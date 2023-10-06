cmake_minimum_required(VERSION 3.10)
project(ray_tracying)

set(CMAKE_CXX_STANDARD 17)


# 设置编译输出的路径
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Wall -march=native ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -march=native")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -march=native")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -march=native")

if (NOT APPLE AND ${CMAKE_CXX_COMPILER} MATCHES "(C|c?)lang")
    add_link_options(-stdlib=libc++)
    add_compile_options(-stdlib=libc++)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/stbi)

add_executable(zsw_one main.cpp)