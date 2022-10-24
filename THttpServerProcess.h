//
// Created by jqq on 2022/10/24.
//

#ifndef HTTPSERVER_THTTPSERVERPROCESS_H
#define HTTPSERVER_THTTPSERVERPROCESS_H

#include "workflow/WFHttpServer.h"

using TaskProcess = std::function<void(protocol::HttpRequest *req, protocol::HttpResponse *resp)>;

class THttpServerProcess {
public:
    THttpServerProcess();
    virtual ~THttpServerProcess();

    void process(WFHttpTask * task);

    void GET(const std::string &path, const TaskProcess &process);

    void POST(const std::string &path, const TaskProcess &process);

private:
    std::map<std::string, TaskProcess> get_process_map_;
    std::map<std::string, TaskProcess> post_process_map_;
};


#endif //HTTPSERVER_THTTPSERVERPROCESS_H
