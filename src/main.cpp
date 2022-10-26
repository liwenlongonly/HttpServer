#include <stdio.h>
#include "THttpServerProcess.h"
#include "workflow/WFFacilities.h"
#include "LogHelper.h"
#include "GlobalConfig.h"
#include "jansson-2.14/jansson.h"

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
    auto httpLogger = spdlog::get(HTTP_LOG_NAME);

    static WFFacilities::WaitGroup wait_group(1);

    THttpServerProcess wrapper;

    wrapper.GET("/", [](WFHttpTask * task, protocol::HttpRequest *req, protocol::HttpResponse *resp)->std::string {
        std::string respStr = "<html>Hello World!</html>";
        return respStr;
    });

    wrapper.POST("/data", [httpLogger](WFHttpTask * task, protocol::HttpRequest *req, protocol::HttpResponse *resp)->std::string {
        const void *body;
        size_t size = 0;
        req->get_parsed_body(&body, &size);

        // 解析参数
        json_error_t error;
        json_t *pRoot = json_loads(std::string((char *)body, size).c_str(), JSON_REJECT_DUPLICATES, &error);
        if (pRoot != nullptr ){
            json_t* obj =  json_object_get(pRoot, "name");
            if(obj && json_is_string(obj)){
                std::string name = json_string_value(obj);
                httpLogger->info("req name: {}", name);
            }
            json_decref(pRoot);
        }
        // 返回数据
        char *result;
        json_t* jsonObject = json_object();
        json_object_set_new(jsonObject, "name", json_string("ilong"));
        json_object_set_new(jsonObject, "age", json_string("18"));
        json_object_set_new(jsonObject, "sex", json_integer(1));
        result = json_dumps(jsonObject, JSON_PRESERVE_ORDER);
        std::string jsonStr(result);
        free(result);
        json_decref(jsonObject);

        return jsonStr;
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