add_executable(hello_world
    hello_world/main.cpp
)

target_include_directories(hello_world PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(hello_world gin)

add_executable(file_upload
    file_upload/main.cpp
)

target_include_directories(file_upload PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(file_upload gin)

# 把 examples/file_upload 复制到 run 目录下，保持与可执行文件同名
add_custom_command(TARGET file_upload POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/examples/file_upload
    ${CMAKE_SOURCE_DIR}/run/examples/file_upload
)