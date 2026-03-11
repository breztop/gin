#include <boost/test/unit_test.hpp>
#include <nlohmann/json.hpp>

#include "core/context.hpp"


BOOST_AUTO_TEST_CASE(TestContextQueryParam) {
    gin::Context ctx;
    ctx.request.query_string = "name=test&value=123";
    ctx.request.ParseQueryParams();
    BOOST_CHECK_EQUAL(ctx.Query("name"), "test");
    BOOST_CHECK_EQUAL(ctx.Query("value"), "123");
    BOOST_CHECK_EQUAL(ctx.Query("nonexistent"), "");
}

BOOST_AUTO_TEST_CASE(TestContextPostForm) {
    gin::Context ctx;
    ctx.request.body = "username=admin&password=secret";
    ctx.request.ParsePostForm();
    BOOST_CHECK_EQUAL(ctx.PostForm("username"), "admin");
    BOOST_CHECK_EQUAL(ctx.PostForm("password"), "secret");
}

BOOST_AUTO_TEST_CASE(TestContextAbort) {
    gin::Context ctx;
    BOOST_CHECK(!ctx.IsAborted());
    ctx.Abort();
    BOOST_CHECK(ctx.IsAborted());
}

BOOST_AUTO_TEST_CASE(TestContextSetAndGet) {
    gin::Context ctx;
    ctx.Set("key1", std::string("value1"));

    auto val = ctx.Get("key1");
    BOOST_CHECK(val.has_value());
}

BOOST_AUTO_TEST_CASE(TestContextResponse) {
    gin::Context ctx;
    ctx.String(200, "Hello World");
    BOOST_CHECK_EQUAL(ctx.response.status_code, 200);
    BOOST_CHECK_EQUAL(ctx.response.body, "Hello World");
}

BOOST_AUTO_TEST_CASE(TestContextJSONWithString) {
    gin::Context ctx;
    nlohmann::json j = nlohmann::json::parse("{\"name\":\"test\"}");
    ctx.JSON(200, j);
    BOOST_CHECK_EQUAL(ctx.response.status_code, 200);
    BOOST_CHECK_EQUAL(ctx.response.headers["Content-Type"], "application/json");
    BOOST_CHECK_EQUAL(ctx.response.body, "{\"name\":\"test\"}");
}

BOOST_AUTO_TEST_CASE(TestContextJSONWithNlohmann) {
    gin::Context ctx;
    nlohmann::json j = {{"name", "test"}, {"value", 123}};
    ctx.JSON(200, j);
    BOOST_CHECK_EQUAL(ctx.response.status_code, 200);
    BOOST_CHECK_EQUAL(ctx.response.headers["Content-Type"], "application/json");
    BOOST_CHECK_EQUAL(ctx.response.body, "{\"name\":\"test\",\"value\":123}");
}

BOOST_AUTO_TEST_CASE(TestContextShouldBindJSON) {
    gin::Context ctx;
    ctx.request.body = "{\"name\":\"test\",\"value\":123}";

    nlohmann::json j;
    bool success = ctx.ShouldBindJSON(j);

    BOOST_CHECK(success);
    BOOST_CHECK_EQUAL(j["name"], "test");
    BOOST_CHECK_EQUAL(j["value"], 123);
}

BOOST_AUTO_TEST_CASE(TestContextShouldBindJSONInvalid) {
    gin::Context ctx;
    ctx.request.body = "invalid json";

    nlohmann::json j;
    bool success = ctx.ShouldBindJSON(j);

    BOOST_CHECK(!success);
}

BOOST_AUTO_TEST_CASE(TestContextHTML) {
    gin::Context ctx;
    ctx.HTML(200, "<html><body>Hello</body></html>");
    BOOST_CHECK_EQUAL(ctx.response.status_code, 200);
    BOOST_CHECK_EQUAL(ctx.response.headers["Content-Type"], "text/html; charset=utf-8");
}

BOOST_AUTO_TEST_CASE(TestContextRedirect) {
    gin::Context ctx;
    ctx.Redirect(302, "/new-location");
    BOOST_CHECK_EQUAL(ctx.response.status_code, 302);
    BOOST_CHECK_EQUAL(ctx.response.headers["Location"], "/new-location");
}

BOOST_AUTO_TEST_CASE(TestContextHeader) {
    gin::Context ctx;
    ctx.Header("X-Custom-Header", "custom-value");
    BOOST_CHECK_EQUAL(ctx.response.headers["X-Custom-Header"], "custom-value");
}

BOOST_AUTO_TEST_CASE(TestContextError) {
    gin::Context ctx;
    ctx.Error(500, "Internal Server Error");
    BOOST_CHECK_EQUAL(ctx.response.status_code, 500);
    BOOST_CHECK_EQUAL(ctx.response.body, "Internal Server Error");
}

BOOST_AUTO_TEST_CASE(TestContextAbortWithError) {
    gin::Context ctx;
    ctx.AbortWithError(404, "Not Found");
    BOOST_CHECK(ctx.IsAborted());
    BOOST_CHECK_EQUAL(ctx.response.status_code, 404);
}
