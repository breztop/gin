#include <boost/test/unit_test.hpp>

#include "core/context.hpp"
#include "core/middleware_chain.hpp"


BOOST_AUTO_TEST_CASE(TestMiddlewareChainRun) {
    gin::MiddlewareChain chain;
    bool middleware_called = false;

    chain.Use([&middleware_called](gin::Context& ctx) {
        middleware_called = true;
        ctx.Next();
    });

    gin::Context ctx;
    chain.Run(ctx);

    BOOST_CHECK(middleware_called);
}

BOOST_AUTO_TEST_CASE(TestMiddlewareChainAbort) {
    gin::MiddlewareChain chain;

    chain.Use([](gin::Context& ctx) {
        ctx.Abort();
    });

    gin::Context ctx;
    chain.Run(ctx);

    BOOST_CHECK(ctx.IsAborted());
}

BOOST_AUTO_TEST_CASE(TestMiddlewareChainMultiple) {
    gin::MiddlewareChain chain;
    int call_order = 0;

    chain.Use([&call_order](gin::Context& ctx) {
        call_order = 1;
        ctx.Next();
    });

    chain.Use([&call_order](gin::Context& ctx) {
        call_order = 2;
        ctx.Next();
    });

    gin::Context ctx;
    chain.Run(ctx);

    BOOST_CHECK_EQUAL(call_order, 2);
}
