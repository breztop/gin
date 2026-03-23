#define BOOST_TEST_MODULE test_engine
#include <boost/test/unit_test.hpp>

#include "core/engine.hpp"


BOOST_AUTO_TEST_CASE(TestEngineBase) {
    try {
        auto engine = gin::Engine::Default();
        engine.Use([](gin::Context&) {
        });

        auto group = engine.Group("/api");
        group.Use([](gin::Context&) {
        });

        engine.Get("/test", [](gin::Context&) {
        });

        engine.Post("/test", [](gin::Context&) {
        });
    } catch (const std::exception& e) {
        BOOST_FAIL("Exception thrown: " << e.what());
    } catch (...) {
        BOOST_FAIL("Unknown exception thrown");
    }

    BOOST_CHECK(true);
}
