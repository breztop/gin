cmake_minimum_required(VERSION 3.20)

# Set policies to avoid Boost-related warnings
if(POLICY CMP0144)
    cmake_policy(SET CMP0144 NEW)
endif()
if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

project(Gin VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/third_party)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/run)

include(cmake/option.cmake)
include(cmake/compile_option.cmake)
include(cmake/generate_build_info.cmake)

# 存在 cmake/platform.cmake 文件才包含
if (EXISTS "${CMAKE_SOURCE_DIR}/cmake/platform.cmake")
    include(cmake/platform.cmake)
endif()

include(cmake/lib.cmake)

# 添加子目录
add_subdirectory(gin)
add_subdirectory(test)
add_subdirectory(examples)



