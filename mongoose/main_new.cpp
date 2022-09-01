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
void fileUpload(mg_connection* nc, const int ev, void* data);
bool validPath(const char *path);

struct userData
{
    int index;
};

struct FileInfo
{
    FILE *fp; //打开新文件的指针
    char fileName[128]; //文件名，包含路径
    char filePath[32]; //文件路径
    size_t size; //文件大小，暂时没有用到
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

    //uri是/fileUpload 时调用函数fileUpload
    mg_register_http_endpoint(con, "/fileUpload", fileUpload);

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

//触发的事件依次为：
//#define MG_EV_HTTP_MULTIPART_REQUEST 121 /* struct http_message */
//#define MG_EV_HTTP_PART_BEGIN 122        /* struct mg_http_multipart_part */
//#define MG_EV_HTTP_PART_DATA 123         /* struct mg_http_multipart_part */
//#define MG_EV_HTTP_PART_END 124          /* struct mg_http_multipart_part */
/* struct mg_http_multipart_part */
//#define MG_EV_HTTP_MULTIPART_REQUEST_END 125

void fileUpload(mg_connection* nc, const int ev, void* data)
{
    //用户指针，用于保存文件大小，文件名
    struct FileInfo *userData = nullptr;
 
    //当事件ev是 MG_EV_HTTP_MULTIPART_REQUEST 时，data类型是http_message
    struct http_message *httpMsg = nullptr;
    if(MG_EV_HTTP_MULTIPART_REQUEST == ev)
    {
        httpMsg = (struct http_message*)data;
        //初次请求时，申请内存
        if(userData == nullptr)
        {
            userData = (struct FileInfo *)malloc(sizeof(struct FileInfo));
            memset(userData, 0, sizeof(struct FileInfo));
        }
    }
    else // 已经不是第一次请求了，nc->user_data 先前已经指向 userData，所以可以用了
    {
        userData = (struct FileInfo *)nc->user_data;
    }
 
    //当事件ev是 MG_EV_HTTP_PART_BEGIN/MG_EV_HTTP_PART_DATA/MG_EV_HTTP_PART_END 时，data类型是mg_http_multipart_part
    struct mg_http_multipart_part *httpMulMsg = nullptr;
    if(ev >= MG_EV_HTTP_PART_BEGIN && ev <= MG_EV_HTTP_PART_END)
    {
        httpMulMsg = (struct mg_http_multipart_part*)data;
    }

    switch(ev) 
    {
        case MG_EV_HTTP_MULTIPART_REQUEST:
            {   
                ///query_string为请求地址中的变量
                char filePath[32] = {0};
                std::string key("filePath"); 
                //从请求地址里获取 key 对应的值，所以这个需要和请求地址里的 key 一样
                //这里从地址中获取文件要上传到哪个路径
                if(mg_get_http_var(&httpMsg->query_string, key.c_str(), filePath, sizeof(filePath)) > 0) 
                {
                    tracef("upload file request, %s = %s\n", key.c_str(), filePath); 
                }

                if(!validPath(filePath))
                {
                    tracef("no such directory of %s\n", filePath);
                    std::string header;
                    std::string body("no suce directory");
                    header.append("HTTP/1.1 500 file fail").append("\r\n");
                    header.append("Connection: close").append("\r\n");
                    header.append("Content-Length: ").append(std::to_string(body.length())).append("\r\n").append("\r\n");
                    header.append(body).append("\r\n");

                    mg_send(nc, header.c_str(), header.length());
                    nc->flags |= MG_F_SEND_AND_CLOSE;             
                }

                //保存路径，且 nc->user_data 指向该内存，下次请求就可以直接用了
                if(userData != nullptr)
                {
                    snprintf(userData->filePath, sizeof(userData->filePath), "%s", filePath);
                    nc->user_data = (void *)userData;                 
                }
            }
 
            break;
        case MG_EV_HTTP_PART_BEGIN:  ///这一步获取文件名
            tracef("upload file begin!\n");
            if(httpMulMsg->file_name != NULL && strlen(httpMulMsg->file_name) > 0)
            {
                tracef("input fileName = %s\n", httpMulMsg->file_name);
                //保存文件名，且新建一个文件
                if(userData != nullptr)
                {
                    snprintf(userData->fileName, sizeof(userData->fileName), "%s%s", userData->filePath, httpMulMsg->file_name);
                    userData->fp = fopen(userData->fileName, "wb+");

                    //创建文件失败，回复，释放内存
                    if(userData->fp == NULL) 
                    {
                        mg_printf(nc, "%s", 
                            "HTTP/1.1 500 file fail\r\n"
                            "Content-Length: 25\r\n"
                            "Connection: close\r\n\r\n"
                            "Failed to open a file\r\n");

                        // nc->flags |= MG_F_SEND_AND_CLOSE;
                        free(userData);
                        nc->user_data = nullptr;     
                        return;
                    }                    
                }

            }
            break;
        case MG_EV_HTTP_PART_DATA:
            tracef("upload file chunk size = %lu\n", httpMulMsg->data.len);
            if(userData != nullptr && userData->fp != NULL) 
            {
                size_t ret = fwrite(httpMulMsg->data.p, 1, httpMulMsg->data.len, userData->fp);
                if(ret != httpMulMsg->data.len)
                {
                    mg_printf(nc, "%s",
                    "HTTP/1.1 500 write fail\r\n"
                    "Content-Length: 29\r\n\r\n"
                    "Failed to write to a file\r\n");

                    nc->flags |= MG_F_SEND_AND_CLOSE;
                    return;
                }     
            }
            break;
        case MG_EV_HTTP_PART_END:
            tracef("file transfer end!\n");
            if(userData != NULL && userData->fp != NULL)
            {
                mg_printf(nc,
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/plain\r\n"
                    "Connection: close\r\n\r\n"
                    "Written %ld of POST data to a file\n\n",
                (long)ftell(userData->fp));

                //设置标志，发送完成数据（如果有）并且关闭连接
                nc->flags |= MG_F_SEND_AND_CLOSE;
                
                //关闭文件，释放内存
                fclose(userData->fp);
                tracef("upload file end, free userData(%p)\n", userData);
                free(userData);
                nc->user_data = NULL;       
            } 
            else
            {
                mg_printf(nc,
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/plain\r\n"
                    "Connection: close\r\n\r\n"
                    "Written 0 of POST data to a file\n\n");                
            }       
            break;
        case MG_EV_HTTP_MULTIPART_REQUEST_END:
            tracef("http multipart request end!\n");
            break;
        default:
            break;
    }
}

bool validPath(const char *path)
{
    struct stat st;
    if(lstat(path, &st) == 0)
    {
        return true;
    }
    return false;
}
