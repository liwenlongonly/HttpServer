//
// Created by jqq on 2022/10/24.
//

#include "THttpServerProcess.h"
#include "workflow/URIParser.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <utility>
#include <string>

#define MAX_SIZE (PATH_MAX+1)

static std::string getCurrentRealPath(){
    char current_absolute_path[MAX_SIZE] = {0};
    //获取当前目录绝对路径
    if (NULL == realpath("./", current_absolute_path)){
        printf("get real path error!\n");
    }
    strcat(current_absolute_path, "/");
    return current_absolute_path;
}

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
            process->second(task, req, resp);
        } else{
            for (auto &item:static_path_map_) {
                if(route.rfind(item.first, 0) == 0){
                    route = item.first;
                    break;
                }
            }
            auto process = get_process_map_.find(route);
            if(process != get_process_map_.end()){
                process->second(task, req, resp);
            } else{
                char msg[128] = {};
                sprintf(msg, "http get path: %s not find", route.c_str());
                resp->append_output_body(msg);
                return;
            }
        }
    } else if(method == "POST"){
        auto process = post_process_map_.find(route);
        if(process != post_process_map_.end()){
            process->second(task, req, resp);
        } else{
            char msg[128] = {};
            sprintf(msg, "http post path: %s not find", route.c_str());
            resp->append_output_body(msg);
            return;
        }
    }
}

void THttpServerProcess::GET(const std::string &path, const TaskProcess &process) {
    get_process_map_[path] = std::move(process);
}

void  THttpServerProcess::POST(const std::string &path, const TaskProcess &process) {
    post_process_map_[path] = std::move(process);
}

void THttpServerProcess::STATIC(std::basic_string<char> path, std::basic_string<char> abs_path) {

    static_path_map_[path] = abs_path;

    auto process = [path, abs_path](WFHttpTask * server_task, protocol::HttpRequest *req, protocol::HttpResponse *resp){

        const std::string &host = "172.0.0.1";
        std::string request_uri = "http://" + host + req->get_request_uri();  // or can't parse URI
        ParsedURI uri;
        if (URIParser::parse(request_uri, uri) < 0){
            resp->append_output_body("url parse error!");
            return;
        }
        std::string rpath(uri.path);
        rpath = rpath.replace(rpath.find(path.c_str()), path.length(), abs_path.c_str());
        if (rpath.back() == '/') {
            rpath += "index.html";
        }
#ifdef DEBUG
        rpath = getCurrentRealPath() + ".." + rpath;
#else
        rpath = getCurrentRealPath() + rpath;
#endif
        resp->add_header_pair("Server", "Turing C++ Workflow Server");
        int fd = open(rpath.c_str(), O_RDONLY);
        if (fd >= 0){
            size_t size = lseek(fd, 0, SEEK_END);
            void *buf = malloc(size); /* As an example, assert(buf != NULL); */
            WFFileIOTask *pread_task = WFTaskFactory::create_pread_task(fd, buf, size, 0, [](WFFileIOTask * task){
                FileIOArgs *args = task->get_args();
                long ret = task->get_retval();
                protocol::HttpResponse *resp = (protocol::HttpResponse *)task->user_data;

                close(args->fd);
                if (task->get_state() != WFT_STATE_SUCCESS || ret < 0)
                {
                    resp->set_status_code("503");
                    resp->append_output_body("<html>503 Internal Server Error.</html>");
                }
                else /* Use '_nocopy' carefully. */
                    resp->append_output_body_nocopy(args->buf, ret);
            });
            /* To implement a more complicated server, please use series' context
             * instead of tasks' user_data to pass/store internal data. */
            pread_task->user_data = resp;	/* pass resp pointer to pread task. */
            server_task->user_data = buf;	/* to free() in callback() */
            server_task->set_callback([](WFHttpTask *t){ free(t->user_data); });
            series_of(server_task)->push_back(pread_task);
        }else{
            resp->set_status_code("404");
            resp->append_output_body("<html>404 Not Found.</html>");
        }
    };
    get_process_map_[path] = std::move(process);
}
