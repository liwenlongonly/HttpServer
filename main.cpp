#include <stdio.h>
#include "THttpServerProcess.h"
#include "workflow/WFFacilities.h"

int main(){

    static WFFacilities::WaitGroup wait_group(1);

    THttpServerProcess wrapper;

    wrapper.GET("/", [](WFHttpTask * task, protocol::HttpRequest *req, protocol::HttpResponse *resp){
        resp->append_output_body("<html>Hello World!</html>");
    });

    wrapper.POST("/data", [](WFHttpTask * task, protocol::HttpRequest *req, protocol::HttpResponse *resp){
        const void *body;
        size_t size = 0;
        req->get_parsed_body(&body, &size);
        resp->append_output_body(body, size);
    });

    wrapper.STATIC(std::string("/static"), std::string("/resource/"));

    WFHttpServer server(std::bind(&THttpServerProcess::process, &wrapper, std::placeholders::_1));

    if (server.start(8888) == 0) { // start server on port 8888
        printf("server.start(8888)");
    }

    wait_group.wait();
    server.stop();
    return 0;
}