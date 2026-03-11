
enable_testing()

include_directories(../)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)

# unit test
add_executable(test_unit
    unit/core/test_app.cpp
    unit/core/test_context.cpp
    unit/core/test_router.cpp
    unit/core/test_middleware_chain.cpp
    unit/http/test_connection_manager.cpp
    unit/http/test_server.cpp
)

target_link_libraries(test_unit PRIVATE gin)
target_link_libraries(test_unit PRIVATE Boost::unit_test_framework)

include(CTest)
