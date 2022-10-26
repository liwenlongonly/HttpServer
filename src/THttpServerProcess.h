//
// Created by jqq on 2022/10/24.
//

#ifndef HTTPSERVER_THTTPSERVERPROCESS_H
#define HTTPSERVER_THTTPSERVERPROCESS_H

#include "workflow/WFHttpServer.h"
#include "macro_definition.h"
#include <memory>

namespace spdlog {
    class logger;
}

NS_BEGIN

class AOP{
public:
    virtual ~AOP(){};
    virtual bool before(const protocol::HttpRequest *req, protocol::HttpResponse *resp) = 0;
    virtual bool after(const protocol::HttpRequest *req, protocol::HttpResponse *resp, const std::string &resp_str) = 0;
};

using TaskProcess = std::function<std::string (WFHttpTask * task, protocol::HttpRequest *req, protocol::HttpResponse *resp)>;

class THttpServerProcess {
public:
    THttpServerProcess();
    virtual ~THttpServerProcess();

    void Process(WFHttpTask * task);

    void GET(const std::string &path, const TaskProcess &process);

    void POST(const std::string &path, const TaskProcess &process);

    void STATIC(std::string path, std::string abs_path);

    void Use(std::shared_ptr<AOP> aop);

private:
    std::map<std::string, TaskProcess> get_process_map_;
    std::map<std::string, TaskProcess> post_process_map_;
    std::map<std::string, std::string> static_path_map_;
    std::shared_ptr<spdlog::logger> http_log_;
    std::shared_ptr<AOP> aop_;
};

NS_END

#endif //HTTPSERVER_THTTPSERVERPROCESS_H
