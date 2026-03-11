#include <boost/test/unit_test.hpp>

#include "core/router.hpp"
#include "http/server.hpp"

BOOST_AUTO_TEST_CASE(TestServerBase) {
    gin::Router router;
    gin::Server server("127.0.0.1", 8081, &router);
    server.Stop();
    BOOST_CHECK(true);
}
