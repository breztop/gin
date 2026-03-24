#include <boost/test/unit_test.hpp>

#include "core/context.hpp"
#include "core/router.hpp"


BOOST_AUTO_TEST_CASE(TestRouterStaticRoute) {
    gin::Router router;
    bool handler_called = false;

    router.AddRoute("GET", "/hello", [&handler_called](gin::Context::Shared) {
        handler_called = true;
    });

    auto result = router.Match("GET", "/hello");
    BOOST_CHECK(result.handler != nullptr);

    result = router.Match("GET", "/notfound");
    BOOST_CHECK(result.handler == nullptr);
}


BOOST_AUTO_TEST_CASE(TestRouterParamRoute) {
    gin::Router router;
    router.AddRoute("GET", "/users/:id", [](gin::Context::Shared) {
    });

    auto result = router.Match("GET", "/users/123");
    BOOST_CHECK(result.handler != nullptr);
    BOOST_CHECK_EQUAL(result.params["id"], "123");
}

BOOST_AUTO_TEST_CASE(TestRouterCatchallRoute) {
    gin::Router router;
    router.AddRoute("GET", "/files/*", [](gin::Context::Shared) {
    });

    auto result = router.Match("GET", "/files/path/to/file.txt");
    BOOST_CHECK(result.handler != nullptr);
}
