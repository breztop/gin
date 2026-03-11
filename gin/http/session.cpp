#include "http/session.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#include "utils/logger.hpp"

namespace gin {

namespace beast = boost::beast;
namespace http = beast::http;

HTTPSession::HTTPSession(tcp::socket socket, Router* router, ConnectionManager& manager)
    : socket_(std::move(socket)), router_(router), connection_manager_(manager) {}

void HTTPSession::Start() {
    connection_manager_.Start(shared_from_this());
    LOG_DEBUG("Session started: {}", socket_.remote_endpoint().address().to_string());
    ReadRequest();
}

void HTTPSession::Stop() {
    beast::error_code ec;
    socket_.close(ec);
    connection_manager_.Stop(shared_from_this());
    LOG_DEBUG("Session stopped");
}

void HTTPSession::SetHandler(Handler handler) {
    handler_ = std::move(handler);
}

void HTTPSession::ReadRequest() {
    auto self = shared_from_this();
    
    http::async_read(socket_, buffer_, request_,
        [self](beast::error_code ec, std::size_t bytes_transferred) {
            boost::ignore_unused(bytes_transferred);
            if (!ec) {
                self->ProcessRequest();
            } else {
                self->HandleError(ec, "read");
            }
        });
}

void HTTPSession::ProcessRequest() {
    ctx_ = std::make_shared<Context>();
    ctx_->request.method = std::string(request_.method_string());
    ctx_->request.path = std::string(request_.target());
    
    LOG_DEBUG("Incoming request: {} {}", ctx_->request.method, ctx_->request.path);
    
    for (auto& header : request_.base()) {
        ctx_->request.headers[header.name_string()] = header.value();
    }
    
    auto result = router_->Match(ctx_->request.method, ctx_->request.path);
    
    if (result.handler) {
        ctx_->SetParams(std::move(result.params));
        
        std::vector<Handler> handlers = result.middlewares;
        handlers.push_back(result.handler);
        ctx_->SetHandlers(std::move(handlers));
        
        LOG_DEBUG("Route matched: {} {}", ctx_->request.method, ctx_->request.path);
        ctx_->Next();
    } else {
        LOG_WARN("Route not found: {} {}", ctx_->request.method, ctx_->request.path);
        ctx_->AbortWithError(404, "Not Found");
    }
    
    WriteResponse();
}

void HTTPSession::WriteResponse() {
    auto self = shared_from_this();
    
    response_.result(ctx_->response.status_code);
    response_.keep_alive(request_.keep_alive());
    
    for (auto& [key, value] : ctx_->response.headers) {
        response_.set(key, value);
    }
    
    beast::ostream(response_.body()) << ctx_->response.body;
    
    response_.content_length(ctx_->response.body.size());
    
    http::async_write(socket_, response_,
        [self](beast::error_code ec, std::size_t) {
            self->Stop();
        });
}

void HTTPSession::HandleError(beast::error_code ec, const std::string& what) {
    LOG_ERROR("Session error: {} - {}", what, ec.message());
    Stop();
}

}  // namespace gin
