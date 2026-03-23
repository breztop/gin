#include "core/router.hpp"

#include <algorithm>
#include <sstream>
#include <utility>

namespace gin {

RouteNode* RouteNode::AddChild(const std::string& path_segment) {
    if (path_segment.empty()) {
        return this;
    }

    if (path_segment == "*") {
        catchall_child = std::make_unique<RouteNode>();
        catchall_child->is_catchall = true;
        return catchall_child.get();
    }

    if (path_segment[0] == ':') {
        wildcard_child = std::make_unique<RouteNode>();
        wildcard_child->is_wildcard = true;
        wildcard_child->param_name = path_segment.substr(1);
        return wildcard_child.get();
    }

    auto it = children.find(path_segment);
    if (it != children.end()) {
        return it->second.get();
    }

    auto node = std::make_unique<RouteNode>();
    node->path = path_segment;
    auto* ptr = node.get();
    children[path_segment] = std::move(node);
    return ptr;
}

RouteNode* RouteNode::GetChild(const std::string& path_segment) const {
    auto it = children.find(path_segment);
    if (it != children.end()) {
        return it->second.get();
    }
    if (wildcard_child) {
        return wildcard_child.get();
    }
    if (catchall_child) {
        return catchall_child.get();
    }
    return nullptr;
}

Router::Router() = default;

void Router::AddRoute(const std::string& method, const std::string& path, Handler handler,
                      std::vector<Middleware> middlewares) {
    InsertRoute(method, path, std::move(handler), std::move(middlewares));
}

void Router::SetNoRouteHandler(Handler handler) { no_route_handler_ = std::move(handler); }

void Router::SetNoMethodHandler(Handler handler) { no_method_handler_ = std::move(handler); }

std::vector<std::string> Router::SplitPath(const std::string& path) const {
    std::vector<std::string> segments;
    std::istringstream ss(path);
    std::string segment;

    while (std::getline(ss, segment, '/')) {
        if (!segment.empty()) {
            segments.push_back(segment);
        }
    }

    return segments;
}

void Router::InsertRoute(const std::string& method, const std::string& path, Handler handler,
                         std::vector<Middleware> middlewares) {
    auto segments = SplitPath(path);

    if (roots_.find(method) == roots_.end()) {
        roots_[method] = std::make_unique<RouteNode>();
    }

    RouteNode* node = roots_[method].get();
    for (const auto& seg : segments) {
        node = node->AddChild(seg);
    }

    node->handler = std::move(handler);
    node->middlewares = std::move(middlewares);
}

Router::MatchResult Router::Match(const std::string& method, const std::string& path) const {
    MatchResult result;

    auto it = roots_.find(method);
    if (it == roots_.end()) {
        return result;
    }

    auto segments = SplitPath(path);
    MatchNode(it->second.get(), segments, 0, result);

    return result;
}

RouteNode* Router::FindNode(const std::vector<std::string>& segments,
                            const std::string& method) const {
    auto it = roots_.find(method);
    if (it == roots_.end()) {
        return nullptr;
    }

    RouteNode* node = it->second.get();
    for (const auto& seg : segments) {
        node = node->GetChild(seg);
        if (!node) {
            return nullptr;
        }
    }

    return node;
}

void Router::MatchNode(RouteNode* node, const std::vector<std::string>& segments, size_t index,
                       MatchResult& result) const {
    if (!node || index >= segments.size()) {
        if (node && node->handler) {
            result.handler = node->handler;
            result.middlewares = node->middlewares;
        }
        return;
    }

    const auto& segment = segments[index];

    auto it = node->children.find(segment);
    if (it != node->children.end()) {
        MatchNode(it->second.get(), segments, index + 1, result);
    }

    if (node->wildcard_child) {
        result.params[node->wildcard_child->param_name] = segment;
        MatchNode(node->wildcard_child.get(), segments, index + 1, result);
    }

    if (node->catchall_child) {
        std::string remaining;
        for (size_t i = index; i < segments.size(); ++i) {
            if (i > index) remaining += "/";
            remaining += segments[i];
        }
        result.params["path"] = remaining;
        MatchNode(node->catchall_child.get(), segments, segments.size(), result);
    }
}

bool Router::HasPath(const std::string& path) const {
    for (const auto& [method, root] : roots_) {
        auto segments = SplitPath(path);
        MatchResult result;
        MatchNode(root.get(), segments, 0, result);
        if (result.handler) {
            return true;
        }
    }
    return false;
}

bool Router::HasMethod(const std::string& method) const {
    return roots_.find(method) != roots_.end();
}

std::string Router::FindAlternativePath(const std::string& path) const {
    std::string alt_path;
    if (path.back() == '/') {
        alt_path = path.substr(0, path.size() - 1);
    } else {
        alt_path = path + "/";
    }

    if (HasPath(alt_path)) {
        return alt_path;
    }
    return "";
}

void CollectRoutes(const RouteNode* node, const std::string& prefix, const std::string& method,
                   std::vector<RouteInfo>& routes) {
    if (!node) return;

    if (node->handler) {
        RouteInfo info;
        info.method = method;
        info.path = prefix.empty() ? "/" : prefix;
        info.middlewares = node->middlewares;
        routes.push_back(std::move(info));
    }

    for (const auto& [segment, child] : node->children) {
        CollectRoutes(child.get(), prefix + "/" + segment, method, routes);
    }

    if (node->wildcard_child) {
        CollectRoutes(node->wildcard_child.get(), prefix + "/:" + node->wildcard_child->param_name,
                      method, routes);
    }

    if (node->catchall_child) {
        CollectRoutes(node->catchall_child.get(), prefix + "/*", method, routes);
    }
}

std::vector<RouteInfo> Router::RoutesInfo() const {
    std::vector<RouteInfo> routes;
    for (const auto& [method, root] : roots_) {
        CollectRoutes(root.get(), "", method, routes);
    }
    return routes;
}

}  // namespace gin
