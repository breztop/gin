# Basic examples
add_executable(hello_world hello_world/main.cpp)
target_include_directories(hello_world PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(hello_world gin)

add_executable(file_upload file_upload/main.cpp)
target_include_directories(file_upload PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(file_upload gin)
add_custom_command(TARGET file_upload POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/examples/file_upload
    ${CMAKE_SOURCE_DIR}/run/examples/file_upload
)

# Cookie example
add_executable(cookie_example cookie/main.cpp)
target_include_directories(cookie_example PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(cookie_example gin)
set_target_properties(cookie_example PROPERTIES OUTPUT_NAME cookie)

# Versioning example
add_executable(versioning_example versioning/main.cpp)
target_include_directories(versioning_example PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(versioning_example gin)
set_target_properties(versioning_example PROPERTIES OUTPUT_NAME versioning)

# Group routes example
add_executable(group_routes_example group_routes/main.cpp)
target_include_directories(group_routes_example PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(group_routes_example gin)
set_target_properties(group_routes_example PROPERTIES OUTPUT_NAME group_routes)

# Form binding example
add_executable(form_binding_example form_binding/main.cpp)
target_include_directories(form_binding_example PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(form_binding_example gin)
set_target_properties(form_binding_example PROPERTIES OUTPUT_NAME form_binding)

# Middleware example
add_executable(middleware_example middleware/main.cpp)
target_include_directories(middleware_example PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(middleware_example gin)
set_target_properties(middleware_example PROPERTIES OUTPUT_NAME middleware)

# RESTful API example
add_executable(restful_api_example restful_api/main.cpp)
target_include_directories(restful_api_example PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(restful_api_example gin)
set_target_properties(restful_api_example PROPERTIES OUTPUT_NAME restful_api)

# Query parameters example
add_executable(query_params_example query_params/main.cpp)
target_include_directories(query_params_example PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(query_params_example gin)
set_target_properties(query_params_example PROPERTIES OUTPUT_NAME query_params)

# JSON API example
add_executable(json_api_example json_api/main.cpp)
target_include_directories(json_api_example PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(json_api_example gin)
set_target_properties(json_api_example PROPERTIES OUTPUT_NAME json_api)

# Error handling example
add_executable(error_handling_example error_handling/main.cpp)
target_include_directories(error_handling_example PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(error_handling_example gin)
set_target_properties(error_handling_example PROPERTIES OUTPUT_NAME error_handling)
