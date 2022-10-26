#include <stdio.h>
#include "THttpServerProcess.h"
#include "workflow/WFFacilities.h"
#include "LogHelper.h"
#include "GlobalConfig.h"

USING_NS

class HttpAop : public AOP{
public:
    HttpAop(){};
    virtual ~HttpAop(){};

    virtual bool before(const protocol::HttpRequest *req, protocol::HttpResponse *resp) override{
        auto httpLogger = spdlog::get(HTTP_LOG_NAME);
        httpLogger->info("uri: {}" ,req->get_request_uri());
        std::string method = req->get_method();
        if(method == "POST"){
            const void *body;
            size_t size = 0;
            req->get_parsed_body(&body, &size);
            httpLogger->info("req: {}", std::string((char *)body, size));
        }
        return true;
    };

    virtual bool after(const protocol::HttpRequest *req, protocol::HttpResponse *resp, const std::string &resp_str) override{
        auto httpLogger = spdlog::get(HTTP_LOG_NAME);
        httpLogger->info("resp: {}",resp_str);
        return true;
    }
};

__attribute__((constructor)) void init(void){
    LogHelper::createLogger(MIAN_LOG_NAME, "Logs/mianLog.log");
    LogHelper::createLogger(HTTP_LOG_NAME, "Logs/httpLog.log");
}

int main(){

    auto config = Singleton<GlobalConfig>::instance();

    auto mianLogger = spdlog::get(MIAN_LOG_NAME);

    static WFFacilities::WaitGroup wait_group(1);

    THttpServerProcess wrapper;

    wrapper.GET("/", [](WFHttpTask * task, protocol::HttpRequest *req, protocol::HttpResponse *resp)->std::string {
        std::string respStr = "<html>Hello World!</html>";
        return respStr;
    });

    wrapper.POST("/data", [](WFHttpTask * task, protocol::HttpRequest *req, protocol::HttpResponse *resp)->std::string {
        const void *body;
        size_t size = 0;
        req->get_parsed_body(&body, &size);
        return std::string((char *)body, size);
    });

    wrapper.STATIC("/static", "/resource/");

    auto aop = std::make_shared<HttpAop>();

    wrapper.Use(aop);

    WFHttpServer server(std::bind(&THttpServerProcess::Process, &wrapper, std::placeholders::_1));

    if (server.start(config->server_port) == 0) { // start server on port 8888
        mianLogger->info("server start with port: {}", config->server_port);
    }

    wait_group.wait();
    server.stop();
    return 0;
}