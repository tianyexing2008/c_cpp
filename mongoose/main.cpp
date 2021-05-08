#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <map>
#include "mongoose.h"
#include "json.h"

void ev_handler(struct mg_connection *nc, int event, void *eventData);
void handleRequest(struct mg_connection *connection, int32_t event, void *data);
void httpResponse(struct mg_connection *connection);
bool hasHeader(const char *header);
void setHeader(const char *header, const char *value);

std::map<std::string, std::string> g_headerMap;
std::string g_protocol;
std::string g_stateStr;
int g_status = 200;

int main()
{
    struct mg_mgr mgr;
    mg_mgr_init(&mgr, NULL);

    struct mg_connection *con = mg_bind(&mgr, "8189", ev_handler);
    mg_set_protocol_http_websocket(con);

    int times = 30;
    while (times-- > 0)
    {
        mg_mgr_poll(&mgr, 1000);
        sleep(1);
    }
    
    mg_mgr_free(&mgr);
    return 0;
}

void ev_handler(struct mg_connection *nc, int event, void *eventData)
{
    switch (event)
    {
        case MG_EV_ACCEPT:
            printf("a new accept!!\n");
            break;
        case MG_EV_HTTP_REQUEST:
            printf("a new http request!!\n");      
            handleRequest(nc, event, eventData);
            break;
        case MG_EV_CLOSE:
            printf("request close!!\n");
            break;
        default:
            break;
    }
}

void handleRequest(struct mg_connection *connection, int32_t event, void *data)
{
    struct http_message* msg = (struct http_message*)data;

    //请求正文
    std::string body(msg->body.p, msg->body.len);
    if(body.length() > 0)
    {
        printf("body size: %lu, %s\n", body.length(), body.c_str());
    }

    std::string url(msg->uri.p, msg->uri.len);
    if(url == "/")
    {
        printf("url size: %lu, %s\n", url.length(), url.c_str());
        struct mg_serve_http_opts opts;
        memset(&opts, 0, sizeof(opts));
        opts.enable_directory_listing = "yes";
        opts.document_root = "./";
        mg_serve_http(connection, msg, opts);
    }

    //保存头部信息，用于应答
    for (int32_t index = 0; index < MG_MAX_HTTP_HEADERS; ++index) 
    {
        if (msg->header_names[index].p == nullptr || msg->header_names[index].len == 0) {
            continue;
        }

        std::string header(msg->header_names[index].p, msg->header_names[index].len);
        std::string value(msg->header_values[index].p, msg->header_values[index].len);
        g_headerMap.insert(std::pair<std::string, std::string>(header, value));
    }

    //添加版本信息，应答使用
    g_headerMap.insert(std::pair<std::string, std::string>("webServer", "0.1"));

    //保存协议信息
    g_protocol = std::string(msg->proto.p, msg->proto.len);

    //应答
    httpResponse(connection);
}

void setContent(Json::Value &content)
{
    Json::Value msg;
    msg["code"] = "0000111";
    msg["message"] = "success";
    msg["data"]["httpPort"] = 8080;
    msg["data"]["httpsPort"] = 8090;
    msg["data"]["rtspPort"] = 554;
    content = msg;
}

/*严格按照http应答头格式进行填写：

** 协议版本|空格|状态码|空格|状态码描述|回车|换行
** 头部字段名|冒号|字段值|回车|换行
** 头部字段名|冒号|字段值|回车|换行
** 头部字段名|冒号|字段值|回车|换行
** .....
** 回车|换行
** 响应正文

*/
void httpResponse(struct mg_connection *connection) 
{
    std::string head(g_protocol);
    head.append(" ").append(std::to_string(g_status)).append(" ");
    if(g_stateStr.empty())
    {
        g_stateStr = mg_status_message(g_status); //获取对应的状态描述符
    }
    head.append(g_stateStr).append("\r\n");

    Json::Value data;
    setContent(data); 
    std::string content = data.toStyledString();
    if(!content.empty() && hasHeader("Content-Length"))
    {
        setHeader("Content-Length", std::to_string(content.length()).c_str());
    }

    //添加头部
    for(auto ite = g_headerMap.begin(); ite != g_headerMap.end(); ite++)
    {
        head.append(ite->first).append(":").append(ite->second).append("\r\n");
    }

    //添加正文
    head.append("\r\n").append(content);
    // printf("send response: \n%s\n", head.c_str());
    
    mg_send(connection, head.c_str(), head.length());
}

bool hasHeader(const char *header)
{
    return g_headerMap.find(header) != g_headerMap.end();
}

void setHeader(const char *header, const char *value)
{
    g_headerMap[header] = value;
}