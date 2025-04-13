cmake_minimum_required(VERSION 3.24)

project(mono)

#set(BUILD_SUBDIR "MSVC" CACHE STRING "Build subdirectory for MSVC-R-2022-amd64")

# if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC 19.41.34120.0")
#     set(BUILD_SUBDIR "MSVC" CACHE STRING "Build subdirectory for MSVC-R-2022-amd64")
# elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU 9.2.0")
#     set(BUILD_SUBDIR "GCC" CACHE STRING "Build subdirectory for GCC-9.2.0-mge32")
# else()
#     set(BUILD_SUBDIR "default" CACHE STRING "Default build subdirectory")
# endif()

add_executable(mono src/main.cpp)