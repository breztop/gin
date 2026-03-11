#include "core/router.hpp"

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

}  // namespace gin
