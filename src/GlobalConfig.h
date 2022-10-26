//
// Created by jqq on 2022/10/25.
//

#ifndef HTTPSERVER_GLOBALCONFIG_H
#define HTTPSERVER_GLOBALCONFIG_H

#include "Singleton.h"
#include "macro_definition.h"
#include "LogHelper.h"

NS_BEGIN

class GlobalConfig final {
public:
    ~GlobalConfig();

    int server_port{8888};

    spdlog::level::level_enum log_level{spdlog::level::debug};

    int log_max_size_mb{5};

    int log_max_count{3};

private:
    GlobalConfig();
    friend Singleton<GlobalConfig>;
};

NS_END

#endif //HTTPSERVER_GLOBALCONFIG_H
