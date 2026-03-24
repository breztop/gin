# Gin++

准备增加 engine

基于 Boost.Beast 的 C++ Web 框架，参考 Go Gin 的 API 设计。

## 已完成功能

### 核心
- **路由系统** - Radix Tree 实现，支持 `:param` 和 `*wildcard` 路径参数
- **中间件系统** - Next/Abort 机制，支持全局和路由组中间件
- **请求处理** - Param, Query, PostForm, ShouldBindJSON
- **响应处理** - JSON, String, HTML, File, Redirect, Header, Cookie
- **验证绑定** - 结构体验证，错误处理
- **上下文存储** - Set/Get 任意类型数据
- **错误处理** - Error, AbortWithError, Recovery 中间件

### HTTP 方法
- GET, POST, PUT, DELETE, PATCH, OPTIONS, HEAD

### 文件处理
- 文件上传（单文件、多文件）
- 静态文件服务（Static, StaticFS）

### 中间件
- Logger - 请求日志
- Recovery - 恐慌恢复
- CORS - 跨域资源共享
- Static - 静态文件服务

### 其他
- 路由组（RouterGroup）
- 运行模式配置

## 快速开始

```cpp
#include <gin/gin.hpp>

int main() {
    auto app = gin::App::Default();
    
    app.Use(gin::middleware::Logger());
    app.Use(gin::middleware::Recovery());
    
    app.Get("/", [](gin::Context::Shared ctx) {
        ctx->JSON(200, {{"message", "Hello World"}});
    });
    
    app.Get("/users/:id", [](gin::Context::Shared ctx) {
        auto id = ctx->Param("id");
        ctx->JSON(200, {{"id", id}, {"name", "Alice"}});
    });
    
    app.Run(8080);
    return 0;
}
```

## 依赖

- Boost.Beast
- Boost.Asio
- nlohmann/json
- spdlog

## 构建

```bash
# you need boost libraries installed
# others will be automatically downloaded by CMake
mkdir build && cd build
cmake ..
make
```
