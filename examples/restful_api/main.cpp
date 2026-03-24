// RESTful API Example
// 简单的 CRUD API 示例

#include <iostream>
#include <map>
#include <mutex>

#include "core/engine.hpp"

struct User {
    int id;
    std::string name;
    std::string email;
};

void to_json(nlohmann::json& j, const User& u) {
    j = nlohmann::json{{"id", u.id}, {"name", u.name}, {"email", u.email}};
}

void from_json(const nlohmann::json& j, User& u) {
    if (j.contains("name")) j.at("name").get_to(u.name);
    if (j.contains("email")) j.at("email").get_to(u.email);
}

class UserService {
public:
    UserService() {
        users_[1] = {1, "Alice", "alice@example.com"};
        users_[2] = {2, "Bob", "bob@example.com"};
    }

    std::vector<User> GetAll() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<User> result;
        for (const auto& [id, user] : users_) {
            result.push_back(user);
        }
        return result;
    }

    User* GetById(int id) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = users_.find(id);
        return it != users_.end() ? &it->second : nullptr;
    }

    User Create(const User& user) {
        std::lock_guard<std::mutex> lock(mutex_);
        int id = next_id_++;
        User new_user = user;
        new_user.id = id;
        users_[id] = new_user;
        return new_user;
    }

    bool Update(int id, const User& user) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = users_.find(id);
        if (it == users_.end()) return false;
        it->second.name = user.name;
        it->second.email = user.email;
        return true;
    }

    bool Delete(int id) {
        std::lock_guard<std::mutex> lock(mutex_);
        return users_.erase(id) > 0;
    }

private:
    std::map<int, User> users_;
    int next_id_ = 3;
    std::mutex mutex_;
};

int main() {
    auto engine = gin::Engine::Default();
    UserService service;

    auto api = engine->Group("/api/v1/users");

    // GET /api/v1/users - 获取所有用户
    api->Get("", [&service](gin::Context::Shared ctx) {
        auto users = service.GetAll();
        nlohmann::json j = users;
        ctx->JSON(200, {{"data", j}, {"total", users.size()}});
    });

    // GET /api/v1/users/:id - 获取单个用户
    api->Get("/:id", [&service](gin::Context::Shared ctx) {
        int id = std::stoi(ctx->Param("id"));
        auto* user = service.GetById(id);
        if (user) {
            ctx->JSON(200, {{"data", *user}});
        } else {
            ctx->JSON(404, {{"error", "User not found"}});
        }
    });

    // POST /api/v1/users - 创建用户
    api->Post("", [&service](gin::Context::Shared ctx) {
        User user;
        if (ctx->BindJSON(user)) {
            auto created = service.Create(user);
            ctx->JSON(201, {{"data", created}});
        }
    });

    // PUT /api/v1/users/:id - 更新用户
    api->Put("/:id", [&service](gin::Context::Shared ctx) {
        int id = std::stoi(ctx->Param("id"));
        User user;
        if (ctx->BindJSON(user)) {
            if (service.Update(id, user)) {
                user.id = id;
                ctx->JSON(200, {{"data", user}});
            } else {
                ctx->JSON(404, {{"error", "User not found"}});
            }
        }
    });

    // DELETE /api/v1/users/:id - 删除用户
    api->Delete("/:id", [&service](gin::Context::Shared ctx) {
        int id = std::stoi(ctx->Param("id"));
        if (service.Delete(id)) {
            ctx->JSON(200, {{"message", "User deleted"}});
        } else {
            ctx->JSON(404, {{"error", "User not found"}});
        }
    });

    std::cout << "RESTful API example starting on http://127.0.0.1:8080" << std::endl;
    std::cout << "Endpoints:" << std::endl;
    std::cout << "  GET    http://127.0.0.1:8080/api/v1/users" << std::endl;
    std::cout << "  GET    http://127.0.0.1:8080/api/v1/users/1" << std::endl;
    std::cout << "  POST   http://127.0.0.1:8080/api/v1/users" << std::endl;
    std::cout << "  PUT    http://127.0.0.1:8080/api/v1/users/1" << std::endl;
    std::cout << "  DELETE http://127.0.0.1:8080/api/v1/users/1" << std::endl;
    engine->Run(8080);

    return 0;
}
