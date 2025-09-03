// SPDX-License-Identifier: GPL-2.0-only
#pragma once
#include <print>

enum LogLevel {
    FATAL,
    WARN,
    INFO,
};

constexpr const char *log_level_string(LogLevel level)
{
    switch (level) {
    case LogLevel::FATAL:
        return "fatal";
    case LogLevel::WARN:
        return "warn";
    case LogLevel::INFO:
        return "info";
    default:
        return "unknown";
    }
}

template<typename... Args>
static inline void _log(LogLevel level, std::format_string<Args...> fmt, Args &&...args)
{
    std::string msg = std::vformat(fmt.get(), std::make_format_args(args...));
    switch (level) {
    case LogLevel::FATAL:
        msg = std::string("\033[1;31m") + log_level_string(level) + ": " + msg + "\033[0m";
        break;
    case LogLevel::WARN:
        msg = std::string("\033[1;33m") + log_level_string(level) + ": " + msg + "\033[0m";
        break;
    case LogLevel::INFO:
        msg = std::string("\033[1;32m") + log_level_string(level) + ": " + msg + "\033[0m";
        break;
    default:
        break;
    }
    std::println(stderr, "{}", msg);
}

#define die(fmt, ...)                                                                             \
    {                                                                                             \
        _log(LogLevel::FATAL, "[{}:{}] {}", __FILE__, __LINE__, std::format(fmt, ##__VA_ARGS__)); \
        exit(EXIT_FAILURE);                                                                       \
    }

#define warn(fmt, ...)                                                                           \
    {                                                                                            \
        _log(LogLevel::WARN, "[{}:{}] {}", __FILE__, __LINE__, std::format(fmt, ##__VA_ARGS__)); \
    }

#define info(fmt, ...)                                                                           \
    {                                                                                            \
        _log(LogLevel::INFO, "[{}:{}] {}", __FILE__, __LINE__, std::format(fmt, ##__VA_ARGS__)); \
    }
