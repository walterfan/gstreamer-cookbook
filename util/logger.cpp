#include <unistd.h>
#include <memory>
#include <iostream>
#include <filesystem>
#include "logger.h"

using namespace spdlog;
using namespace sinks;

#define MODULE "gst_cookbook"

Logger::Logger()
{
    loggers_.clear();
}

Logger::~Logger()
{
    spdlog::drop_all();    
}

void Logger::init(std::string file, int file_size, int file_count)
{
    if(spdlog::get(MODULE) != nullptr) {
        return;
    }
    // pattern
    std::string pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [%t] [%s:%#] (%!) %v");
    // console sink
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::debug);
    console_sink->set_pattern(pattern);
    // file sink
    auto dir = std::filesystem::current_path();
    std::string log_dir = std::string(dir.string()) + "/log";
    //chdir(log_dir.c_str());
    std::string log_path = log_dir + "/" + file;
    std::cout << log_path << std::endl;
    auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(log_path.c_str(), 1024 * 1024 * file_size, file_count);
    file_sink->set_level(spdlog::level::debug);
    file_sink->set_pattern(pattern);
    // register patter
    spdlog::sinks_init_list sinks{ console_sink, file_sink };
    std::shared_ptr<spdlog::logger> logger = std::make_shared<spdlog::logger>(MODULE, sinks);
    logger->set_pattern(pattern);
    // register logger
    spdlog::register_logger(logger);
    // set level
    logger->set_level(spdlog::level::trace);
    logger->flush_on(spdlog::level::err);
    loggers_.insert(std::pair<std::string, std::shared_ptr<spdlog::logger>>(MODULE, logger));
    spdlog::flush_every(std::chrono::seconds(3));
}

std::shared_ptr<spdlog::logger>& Logger::get_logger()
{
    return loggers_.find(MODULE)->second;
}

spdlog::level::level_enum int_to_log_level(int l)
{
    spdlog::level::level_enum level;
    switch (l)
    {
        case SPDLOG_LEVEL_TRACE:
            level = spdlog::level::trace;
            break;
        case SPDLOG_LEVEL_DEBUG:
            level = spdlog::level::debug;
            break;
        case SPDLOG_LEVEL_INFO:
            level = spdlog::level::info;
            break;
        case SPDLOG_LEVEL_WARN:
            level = spdlog::level::warn;
            break;
        case SPDLOG_LEVEL_ERROR:
            level = spdlog::level::err;
            break;
        case SPDLOG_LEVEL_CRITICAL:
            level = spdlog::level::critical;
            break;
        case SPDLOG_LEVEL_OFF:
            level = spdlog::level::off;
            break;
        default:
            level = spdlog::level::n_levels;
            break;
    }
    return level;
}

void Logger::reset_level(int level, int flush_level, int flush_interval)
{
    auto logger =loggers_.find(MODULE)->second;
    if (logger != nullptr) {
        std::vector<sink_ptr> sinks = logger->sinks();
        auto it = sinks.begin();
        for (; it != sinks.end(); it++)
        {
            (*it)->set_level(int_to_log_level(level));
        }
        logger->set_level(int_to_log_level(level));
        logger->flush_on(int_to_log_level(flush_level));
    }
    spdlog::flush_every(std::chrono::seconds(flush_interval));
}