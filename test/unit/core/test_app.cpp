#define BOOST_TEST_MODULE test_app
#include <boost/test/unit_test.hpp>

#include "core/app.hpp"


BOOST_AUTO_TEST_CASE(TestAppBase) {
    try {
        auto app = gin::App::Default();
        app.Use([](gin::Context&) {
        });

        auto group = app.Group("/api");
        group.Use([](gin::Context&) {
        });

        app.Get("/test", [](gin::Context&) {
        });

        app.Post("/test", [](gin::Context&) {
        });
    } catch (const std::exception& e) {
        BOOST_FAIL("Exception thrown: " << e.what());
    } catch (...) {
        BOOST_FAIL("Unknown exception thrown");
    }

    BOOST_CHECK(true);
}
