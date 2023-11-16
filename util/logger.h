#pragma once

#include <memory>
#include <string>
#include <map>

#ifndef SPDLOG_TRACE_ON
#define SPDLOG_TRACE_ON
#endif

#ifndef SPDLOG_DEBUG_ON
#define SPDLOG_DEBUG_ON
#endif

#ifndef SPDLOG_ACTIVE_LEVEL
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#endif

#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"
#include "spdlog/logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"


class Logger
{
public:
    static Logger& get_instance()
    {
        static Logger instance;
        return instance;
    }

    Logger();
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::shared_ptr<spdlog::logger>& get_logger();

    void init(std::string file, int file_size, int file_count);
    void reset_level(int level, int flush_level, int flush_interval);

private:
    std::map<std::string, std::shared_ptr<spdlog::logger>> loggers_;
};

#define DEBUG_TRACE(...) SPDLOG_LOGGER_DEBUG(Logger::get_instance().get_logger(), __VA_ARGS__)
#define INFO_TRACE(...) SPDLOG_LOGGER_INFO(Logger::get_instance().get_logger(), __VA_ARGS__)
#define WARN_TRACE(...) SPDLOG_LOGGER_WARN(Logger::get_instance().get_logger(), __VA_ARGS__)
#define ERROR_TRACE(...) SPDLOG_LOGGER_ERROR(Logger::get_instance().get_logger(), __VA_ARGS__)
