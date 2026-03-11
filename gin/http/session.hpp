#pragma once

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <memory>
#include <functional>

#include "core/context.hpp"
#include "core/router.hpp"
#include "core/types.hpp"
#include "http/connection_manager.hpp"

namespace gin {

namespace beast = boost::beast;
namespace http = beast::http;
using boost::asio::ip::tcp;

class HTTPSession : public std::enable_shared_from_this<HTTPSession> {
public:
    HTTPSession(tcp::socket socket, Router* router, ConnectionManager& manager);

    void Start();
    void Stop();

    void SetHandler(Handler handler);

private:
    void ReadRequest();
    void ProcessRequest();
    void WriteResponse();

    void HandleError(beast::error_code ec, const std::string& what);

    tcp::socket socket_;
    beast::flat_buffer buffer_{8192};
    http::request<http::dynamic_body> request_;
    http::response<http::dynamic_body> response_;
    
    Router* router_;
    ConnectionManager& connection_manager_;
    Handler handler_;
    std::shared_ptr<Context> ctx_;
};

}  // namespace gin
