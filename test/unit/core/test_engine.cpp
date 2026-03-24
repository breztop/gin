#define BOOST_TEST_MODULE test_engine
#include <boost/test/unit_test.hpp>

#include "core/engine.hpp"


BOOST_AUTO_TEST_CASE(TestEngineBase) {
    try {
        auto engine = gin::Engine::Default();
        engine->Use([](gin::Context::Shared) {
        });

        auto group = engine->Group("/api");
        group->Use([](gin::Context::Shared) {
        });

        engine->Get("/test", [](gin::Context::Shared) {
        });

        engine->Post("/test", [](gin::Context::Shared) {
        });
    } catch (const std::exception& e) {
        BOOST_FAIL("Exception thrown: " << e.what());
    } catch (...) {
        BOOST_FAIL("Unknown exception thrown");
    }

    BOOST_CHECK(true);
}
