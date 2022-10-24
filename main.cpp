#include <stdio.h>
#include "THttpServerProcess.h"

int main(){

    THttpServerProcess wrapper;

    wrapper.GET("/", [](protocol::HttpRequest *req, protocol::HttpResponse *resp){
        resp->append_output_body("<html>Hello World!</html>");
    });

    wrapper.POST("/data", [](protocol::HttpRequest *req, protocol::HttpResponse *resp){
        const void *body;
        size_t size = 0;
        req->get_parsed_body(&body, &size);
        resp->append_output_body(body, size);
    });

    WFHttpServer server(std::bind(&THttpServerProcess::process, &wrapper, std::placeholders::_1));

    if (server.start(8888) == 0) { // start server on port 8888
        printf("server.start(8888)");
        getchar(); // press "Enter" to end.
        server.stop();
    }

    return 0;
}