#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/types.hpp"

namespace gin {

struct RouteNode {
    std::string path;
    bool is_wildcard = false;
    bool is_catchall = false;
    std::string param_name;

    std::unordered_map<std::string, std::unique_ptr<RouteNode>> children;
    std::unique_ptr<RouteNode> wildcard_child;
    std::unique_ptr<RouteNode> catchall_child;

    std::vector<Middleware> middlewares;
    Handler handler;

    RouteNode* AddChild(const std::string& path_segment);
    RouteNode* GetChild(const std::string& path_segment) const;
};

class Router {
public:
    Router();

    void AddRoute(const std::string& method, const std::string& path, Handler handler,
                  std::vector<Middleware> middlewares = {});

    void SetNoRouteHandler(Handler handler);
    void SetNoMethodHandler(Handler handler);

    struct MatchResult {
        Handler handler;
        std::vector<Middleware> middlewares;
        std::unordered_map<std::string, std::string> params;
        bool method_not_allowed = false;
    };

    MatchResult Match(const std::string& method, const std::string& path) const;

    bool HasPath(const std::string& path) const;
    bool HasMethod(const std::string& method) const;
    std::string FindAlternativePath(const std::string& path) const;

    Handler GetNoRouteHandler() const { return no_route_handler_; }
    Handler GetNoMethodHandler() const { return no_method_handler_; }

    std::vector<RouteInfo> RoutesInfo() const;

private:
    std::unordered_map<std::string, std::unique_ptr<RouteNode>> roots_;
    Handler no_route_handler_;
    Handler no_method_handler_;

    std::vector<std::string> SplitPath(const std::string& path) const;
    void InsertRoute(const std::string& method, const std::string& path, Handler handler,
                     std::vector<Middleware> middlewares);
    RouteNode* FindNode(const std::vector<std::string>& segments, const std::string& method) const;
    void MatchNode(RouteNode* node, const std::vector<std::string>& segments, size_t index,
                   MatchResult& result) const;
};

}  // namespace gin
