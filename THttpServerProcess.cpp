//
// Created by jqq on 2022/10/24.
//

#include "THttpServerProcess.h"
#include "workflow/URIParser.h"

THttpServerProcess::THttpServerProcess() {

}

THttpServerProcess::~THttpServerProcess() {

}

void THttpServerProcess::process(WFHttpTask *task) {
    protocol::HttpRequest *req = task->get_req();
    protocol::HttpResponse *resp = task->get_resp();

    const std::string method(req->get_method());

    const std::string &host = "172.0.0.1";

    std::string request_uri = "http://" + host + req->get_request_uri();  // or can't parse URI
    ParsedURI uri;
    if (URIParser::parse(request_uri, uri) < 0)
    {
        resp->append_output_body("url parse error!");
        return;
    }

    std::string route;

    if (uri.path && uri.path[0])
        route = uri.path;
    else
        route = "/";

    if(method == "GET"){
        auto process = get_process_map_.find(route);
        if(process != get_process_map_.end()){
            process->second(req, resp);
        } else{
            char msg[128] = {};
            sprintf(msg, "http get path: %s not find", route.c_str());
            resp->append_output_body(msg);
            return;
        }
    } else if(method == "POST"){
        auto process = post_process_map_.find(route);
        if(process != post_process_map_.end()){
            process->second(req, resp);
        } else{
            char msg[128] = {};
            sprintf(msg, "http post path: %s not find", route.c_str());
            resp->append_output_body(msg);
            return;
        }
    }
}

void THttpServerProcess::GET(const std::string &path, const TaskProcess &process) {
    get_process_map_[path] = process;
}

void  THttpServerProcess::POST(const std::string &path, const TaskProcess &process) {
    post_process_map_[path] = process;
}
