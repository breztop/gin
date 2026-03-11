#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/fmt/fmt.h>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace gin {

class Logger {
public:
    static Logger& Instance() {
        static Logger instance;
        if (!instance.initialized_) {
            instance.Init();
        }
        return instance;
    }

    void Init(std::string_view name = "gin",
              std::string_view level = "info",
              std::string_view log_file = "",
              bool console = true) {
        if (initialized_) {
            return;
        }
        initialized_ = true;
        name_ = std::string(name);

        std::vector<spdlog::sink_ptr> sinks;

        if (console) {
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%n] %v");
            sinks.push_back(console_sink);
        }

        if (!log_file.empty()) {
            try {
                auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                    std::string(log_file), 1024 * 1024 * 10, 3);
                file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%n] %v");
                sinks.push_back(file_sink);
            } catch (const spdlog::spdlog_ex& ex) {
                std::cerr << "Failed to create file sink: " << ex.what() << std::endl;
            }
        }

        if (!sinks.empty()) {
            logger_ = std::make_shared<spdlog::logger>(name_, begin(sinks), end(sinks));
            logger_->set_level(spdlog::level::trace);
            logger_->flush_on(spdlog::level::warn);
            spdlog::register_logger(logger_);
        } else {
            logger_ = spdlog::stdout_color_mt(std::string(name));
        }

        SetLevel(level);
    }

    void SetLevel(std::string_view level) {
        if (logger_) {
            if (level == "trace") {
                logger_->set_level(spdlog::level::trace);
            } else if (level == "debug") {
                logger_->set_level(spdlog::level::debug);
            } else if (level == "info") {
                logger_->set_level(spdlog::level::info);
            } else if (level == "warn") {
                logger_->set_level(spdlog::level::warn);
            } else if (level == "error") {
                logger_->set_level(spdlog::level::err);
            } else if (level == "critical") {
                logger_->set_level(spdlog::level::critical);
            }
        }
    }

    void Trace(std::string_view msg) {
        if (logger_) {
            logger_->trace("{}", msg);
        }
    }

    void Debug(std::string_view msg) {
        if (logger_) {
            logger_->debug("{}", msg);
        }
    }

    void Info(std::string_view msg) {
        if (logger_) {
            logger_->info("{}", msg);
        }
    }

    void Warn(std::string_view msg) {
        if (logger_) {
            logger_->warn("{}", msg);
        }
    }

    void Error(std::string_view msg) {
        if (logger_) {
            logger_->error("{}", msg);
        }
    }

    void Critical(std::string_view msg) {
        if (logger_) {
            logger_->critical("{}", msg);
        }
    }

    template <typename... Args>
    void Trace(std::string_view fmt, Args&&... args) {
        if (logger_) {
            logger_->trace(fmt::runtime(fmt), std::forward<Args>(args)...);
        }
    }

    template <typename... Args>
    void Debug(std::string_view fmt, Args&&... args) {
        if (logger_) {
            logger_->debug(fmt::runtime(fmt), std::forward<Args>(args)...);
        }
    }

    template <typename... Args>
    void Info(std::string_view fmt, Args&&... args) {
        if (logger_) {
            logger_->info(fmt::runtime(fmt), std::forward<Args>(args)...);
        }
    }

    template <typename... Args>
    void Warn(std::string_view fmt, Args&&... args) {
        if (logger_) {
            logger_->warn(fmt::runtime(fmt), std::forward<Args>(args)...);
        }
    }

    template <typename... Args>
    void Error(std::string_view fmt, Args&&... args) {
        if (logger_) {
            logger_->error(fmt::runtime(fmt), std::forward<Args>(args)...);
        }
    }

    template <typename... Args>
    void Critical(std::string_view fmt, Args&&... args) {
        if (logger_) {
            logger_->critical(fmt::runtime(fmt), std::forward<Args>(args)...);
        }
    }

    void Flush() {
        if (logger_) {
            logger_->flush();
        }
    }

private:
    Logger() : initialized_(false) {}
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    bool initialized_;
    std::shared_ptr<spdlog::logger> logger_;
    std::string name_;
};

#define LOG_TRACE(...) ::gin::Logger::Instance().Trace(__VA_ARGS__)
#define LOG_DEBUG(...) ::gin::Logger::Instance().Debug(__VA_ARGS__)
#define LOG_INFO(...) ::gin::Logger::Instance().Info(__VA_ARGS__)
#define LOG_WARN(...) ::gin::Logger::Instance().Warn(__VA_ARGS__)
#define LOG_ERROR(...) ::gin::Logger::Instance().Error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::gin::Logger::Instance().Critical(__VA_ARGS__)

}  // namespace gin
