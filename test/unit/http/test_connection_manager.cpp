#include <boost/test/unit_test.hpp>

#include "http/connection_manager.hpp"


BOOST_AUTO_TEST_CASE(TestConnectionManagerBase) {
    gin::ConnectionManager manager;
    manager.StopAll();
    BOOST_CHECK(true);
}
