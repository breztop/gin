add_executable(hello_world
    hello_world/main.cpp
)

target_include_directories(hello_world PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(hello_world gin)
