//
// Created by jqq on 2022/10/25.
//

#include "LogHelper.h"
#include "GlobalConfig.h"

NS_BEGIN

LogHelper::LogHelper() {

}

LogHelper::~LogHelper() {

}

std::shared_ptr<spdlog::logger> LogHelper::createLogger(const std::string &tag, const std::string &log_path) {

    auto config = Singleton<GlobalConfig>::instance();

    auto sink1 = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    sink1->set_pattern("[%Y-%m-%d %H:%M:%S.%e][%L][%t]->%v");
    sink1->set_level(spdlog::level::debug);

    auto sink2 = std::make_shared<spdlog::sinks::rotating_file_sink_st>(log_path, 1024 * 1024 * config->log_max_size_mb, config->log_max_count);
    sink2->set_pattern("[%Y-%m-%d %H:%M:%S.%e][%L][%t]->%v");
    sink2->set_level(spdlog::level::info);

    std::vector<spdlog::sink_ptr> sinks = { sink1,sink2 };
    auto logger = std::make_shared<spdlog::logger>(tag, sinks.begin(), sinks.end());
    logger->flush_on(spdlog::level::info);
    spdlog::register_logger(logger);


    spdlog::set_level(config->log_level);

    return logger;
}

NS_END