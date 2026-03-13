add_library(gin STATIC
    core/app.cpp
    core/context.cpp
    core/cookie.cpp
    core/form_file.cpp
    core/middleware_chain.cpp
    core/request.cpp
    core/response.cpp
    core/router.cpp
    core/validator.cpp
    http/connection_manager.cpp
    http/session.cpp
    http/server.cpp
)

target_include_directories(gin PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

link_boost_header(gin)

link_nlohmann_json(gin)
link_spdlog(gin)
# link_breutil(gin)

add_iwyu(gin)
