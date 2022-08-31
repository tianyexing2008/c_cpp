#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string>
#include <map>
#include <mutex>
#include <sstream>
#include <iostream>
#include "mongoose.h"
#include "../json/json.h"
#include "../logFormatPrt/log.h"
#include "httpResponse.h"

void eventHandler(struct mg_connection *nc, int event, void *eventData);
void handleRequest(struct mg_connection *connection, int32_t event, void *data);
void* addConnection(struct mg_connection *nc);
void delConnection(void *usrData);
bool isGiantFile(const char *fileName);
void sendGiantFile(struct mg_connection *connection, const char *fileName);

struct userData
{
    int index;
};

enum FILE_TYPE
{
    FILE_TYPE_REGULAR,
    FILE_TYPE_DIR,
    FILE_TYPE_CHR, //字符设备文件
    FILE_TYPE_BLK, //块设备文件
    FILE_TYPE_FIFO,
    FILE_TYPE_LINK,
    FILE_TYPE_SOCKET,
    FILE_TYPE_UNKNOW,
    FILE_TYPE_MAX
};

std::map<int, void *> g_connectionMap; //连接映射表，当多线程时，用于找到对应连接进行应答
std::string g_protocol; //协议，如http /1.1
std::string g_stateStr; //状态描述，用于返回应答
 
int g_status = 200;  //应答状态
int g_connectId = 0; //连接id的分配
 
std::mutex g_mutex;
 
//用postman 测试，linux需要关闭防火墙，否则收不到数据
int main(int argc, char *argv[])
{   
    struct mg_mgr mgr;
 
    mg_mgr_init(&mgr, nullptr);
 
    int port = 8190;
    char buf[5] = {0};
    snprintf(buf, sizeof(buf), "%d", port);
    struct mg_connection *con = mg_bind(&mgr, buf, eventHandler);
 
    if(con == NULL) {
        errorf("mg_bind fail\n");
        return -1;
    }
 
    mg_set_protocol_http_websocket(con);
    infof("listen ip[%s], port[%d]....\n", inet_ntoa(con->sa.sin.sin_addr), port); 

    while (1)
    {
        mg_mgr_poll(&mgr, 100);
    }
    
    mg_mgr_free(&mgr);
    return 0;
}
 
void eventHandler(struct mg_connection *nc, int event, void *eventData)
{
    switch (event)
    {
        case MG_EV_ACCEPT:
            break;
        case MG_EV_HTTP_REQUEST:    
            handleRequest(nc, event, eventData);
            break;
        case MG_EV_CLOSE:
            delConnection(nc->user_data);
            break;
        default:
            break;
    }
}
 
// 添加一个连接
void* addConnection(struct mg_connection *nc)
{
    userData *data = new userData; //此内存在delConnection时进行释放
    std::lock_guard<std::mutex> lockGuard(g_mutex);
    g_connectId += 1;
    data->index = g_connectId;
    g_connectionMap.insert(std::pair<int, void*>(g_connectId, nc));
 
    return data;
}
 
// 删除一个连接
void delConnection(void *usrData)
{
    userData *data = (userData*)usrData;
    if(data == NULL)
    {
        return;
    }
 
    if(g_connectionMap.find(data->index) != g_connectionMap.end())
    {
        tracef("delete connection id: %d\n", data->index);
        g_connectionMap.erase(data->index);
    }
    delete data; //释放addConnection时申请的内存
}

void handleRequest(struct mg_connection *connection, int32_t event, void *data)
{
    struct http_message* msg = (struct http_message*)data;
 
    std::string uri(msg->uri.p, msg->uri.len);
    tracef("connect from ip: %s, uri = %s\n", inet_ntoa(connection->sa.sin.sin_addr), uri.c_str());

    //设置资源根目录
    if(uri == "/")
    {
        struct mg_serve_http_opts opts;
        memset(&opts, 0, sizeof(opts));
        opts.enable_directory_listing = "yes";
        opts.document_root = "./";
        mg_serve_http(connection, msg, opts);

        //第一次请求根目录时，可以不往下处理了
        return;
    }
 
    CHttpResponse response;

    //保存请求头，用于应答
    for (int32_t index = 0; index < MG_MAX_HTTP_HEADERS; ++index) 
    {
        if (msg->header_names[index].p == nullptr || msg->header_names[index].len == 0
            || std::string(msg->header_names[index].p,msg->header_names[index].len).find("Accept") != std::string::npos) 
        {
            continue;
        }
 
        std::string header(msg->header_names[index].p, msg->header_names[index].len);
        std::string value(msg->header_values[index].p, msg->header_values[index].len);

        response.setHead(header.c_str(), value.c_str());
    }

    //请求的方法，GET、POST、PUT等
    std::string method(msg->method.p, msg->method.len);
    tracef("request method: %s\n", method.c_str());
    
    std::string fileName = uri.substr(1);
    if(uri.length() > 1 && method == "GET") 
    {   
        if(!isGiantFile(fileName.c_str()))
        {
            response.setBody(fileName.c_str());
            std::string content;
            response.getContent(content);
            mg_send(connection, content.c_str(), content.length());            
        }
        else 
        {
            //处理大文件，采用分块传输的方式
            std::string headers;
            headers.append("HTTP/1.1 200 ok").append("\r\n");
            response.getHead(headers);
            headers.append("Content-Type: application/octet-stream").append("\r\n");
            headers.append("Transfer-Encoding: chunked").append("\r\n").append("\r\n");
            mg_send(connection, headers.c_str(), headers.length());
            tracef("giant file send header: %s\n", headers.c_str());

            sendGiantFile(connection, fileName.c_str());
        }
    }
}

bool isGiantFile(const char *fileName)
{
    size_t len = 0;
    FILE *fp = fopen(fileName, "r");
    if(fp != NULL)
    {
        fseek(fp, 0, SEEK_END);
        len = ftell(fp);
        rewind(fp);
        fclose(fp);
    }

    bool ret = (len >= 1024 * 1024);

    return ret;
}

void sendGiantFile(struct mg_connection *connection, const char *fileName)
{
    size_t fileLen = 0;
    int readLen = 0;
    int hex = 0;
    char hexLen[8] = {0};
    char buf[512] = {0};
    FILE *fp = fopen(fileName, "r");
    fseek(fp, 0, SEEK_END);
    fileLen = ftell(fp);
    rewind(fp);


    while(fileLen > 0)
    {   
        std::string body;
        char hexLen[8] = {0};

        readLen = fread(buf, 1, sizeof(buf), fp);
        mg_send_http_chunk(connection, buf, readLen);
        fileLen -= readLen;
    }

    fclose(fp);

    //最后０结尾
    tracef("send tail 0 chunked\n");
    mg_send_http_chunk(connection, "", 0);
    // std::string tail("0");
    // tail.append("\r\n");
    // mg_send(connection, tail.c_str(), tail.length());    
}