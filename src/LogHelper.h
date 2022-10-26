//
// Created by jqq on 2022/10/25.
//

#ifndef HTTPSERVER_LOGHELPER_H
#define HTTPSERVER_LOGHELPER_H
#include "macro_definition.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"

#define MIAN_LOG_NAME "MianLog"
#define HTTP_LOG_NAME "HttpLog"

NS_BEGIN

class LogHelper {
public:
    LogHelper();
    ~LogHelper();
    static std::shared_ptr<spdlog::logger> createLogger(const std::string &tag, const std::string &log_path);
};

NS_END

#endif //HTTPSERVER_LOGHELPER_H
