#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string>
#include <map>
#include <mutex>
#include "mongoose.h"
#include "../json/json.h"
#include "../logFormatPrt/log.h"
 
void eventHandler(struct mg_connection *nc, int event, void *eventData);
void handleRequest(struct mg_connection *connection, int32_t event, void *data);
void httpResponse(struct mg_connection *connection, Json::Value &body);
void httpFileResponse(struct mg_connection *con, const char *fileName);
bool hasHeader(const char *header);
void setHeader(const char *header, const char *value);
void* addConnection(struct mg_connection *nc);
void delConnection(void *usrData);
void webStart();
void fileUpload(mg_connection* nc, const int ev, void* data);
bool isFile(const char *file);
 
struct userData
{
    int index;
};
 
struct FileInfo
{
    FILE *fp;
    char fileName[32];
    char filePath[32];
    size_t size;
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

std::map<std::string, std::string> g_headerMap;
std::map<int, void *> g_connectionMap; //连接映射表，当多线程时，用于找到对应连接进行应答
std::string g_protocol; //协议，如http /1.1
std::string g_stateStr; //状态描述，用于返回应答
 
int g_status = 200;  //应答状态
int g_connectId = 0; //连接id的分配
 
std::mutex g_mutex;
 
//用postman 测试，linux需要关闭防火墙，否则收不到数据
int main(int argc, char *argv[])
{   
    // if(argc < 2)
    // {
    //     errorf("Usage: %s port\n", argv[0]);
    //     return 1;
    // }
 
    // int port = atoi(argv[1]);
 
    // webStart();
 
    struct userData uData{100};
    struct mg_mgr mgr;
 
    //mgr里的user_data指针将会指向第二个参数，当有连接过来时
    //mg_connection中的mgr中的user_data指向的就是第二个参数
    mg_mgr_init(&mgr, &uData);
 
    int port = 8190;
    char buf[5] = {0};
    snprintf(buf, sizeof(buf), "%d", port);
    struct mg_connection *con = mg_bind(&mgr, buf, eventHandler);
 
    if(con == NULL) {
        errorf("mg_bind fail\n");
        return -1;
    }
 
    mg_set_protocol_http_websocket(con);
    uData.index = 200;
    infof("listen ip[%s], port[%d]....\n", inet_ntoa(con->sa.sin.sin_addr), port); 

    //uri是/fileUpload 时调用函数fileUpload
    // mg_register_http_endpoint(con, "/fileUpload", fileUpload);
 
    while (1)
    {
        mg_mgr_poll(&mgr, 100);
    }
    
    mg_mgr_free(&mgr);
    return 0;
}
 
void fileUpload(mg_connection* nc, const int ev, void* data)
{
    struct FileInfo *userData = (struct FileInfo*)(nc->user_data);
 
    //当事件ev是MG_EV_HTTP_MULTIPART_REQUEST时，data类型是http_message
    struct http_message *hp = (struct http_message*)data;
 
    //当事件ev是MG_EV_HTTP_PART_BEGIN/MG_EV_HTTP_PART_DATA/MG_EV_HTTP_PART_END时，data类型是mg_http_multipart_part
    struct mg_http_multipart_part *mp = (struct mg_http_multipart_part*)data;
 
    std::string value;
    switch(ev) {
        case MG_EV_HTTP_MULTIPART_REQUEST:
            {   ///query_string为请求地址中的变量
                std::string queryStr(hp->query_string.p, hp->query_string.len);
                tracef("upload file request, queryStr = %s\n", queryStr.c_str());
                char fileName[32];
                std::string key("file_name");
                if(mg_get_http_var(&hp->query_string, key.c_str(), fileName, sizeof(fileName)) > 0) {
                    tracef("upload file request, %s = %s\n", key.c_str(), fileName); 
                }
                mg_str *var;
                userData = (struct FileInfo *)malloc(sizeof(struct FileInfo));
                var = mg_get_http_header(hp, "Content-Length");
                std::string fileLen(var->p, var->len);
                tracef("userData = %p\n", userData);
                userData->size = std::atoi(fileLen.c_str());
                nc->user_data = (void *)userData;
            }
 
            break;
        case MG_EV_HTTP_PART_BEGIN:  ///这一步获取文件名
            tracef("upload file begin!\n");
            if(mp->file_name != NULL && strlen(mp->file_name) > 0) {
                tracef("input fileName = %s, userData->filePath = %s\n", mp->file_name, userData->filePath);
                snprintf(userData->fileName, sizeof(userData->fileName), "%s", mp->file_name);
                userData->fp = fopen(userData->fileName, "wb+");
                if(userData->fp == NULL) {
                    mg_printf(nc, "%s", 
                        "HTTP/1.1 500 file fail\r\n"
                        "Content-Length: 25\r\n"
                        "Connection: close\r\n\r\n"
                        "Failed to open a file\r\n");
                    nc->flags |= MG_F_SEND_AND_CLOSE;
                    free(userData);       
                    return;
                }
            }
            break;
        case MG_EV_HTTP_PART_DATA:
            tracef("upload file push data!\n");
            if(mp->file_name != NULL && strlen(mp->file_name) > 0) {
                if(userData != NULL && userData->fp != NULL) {
                    if(fwrite(mp->data.p, 1, mp->data.len, userData->fp) != mp->data.len) {
                        mg_printf(nc, "%s",
                        "HTTP/1.1 500 write fail\r\n"
                        "Content-Length: 29\r\n\r\n"
                        "Failed to write to a file\r\n");
 
                        nc->flags |= MG_F_SEND_AND_CLOSE;
                        return;
                    }                
                } else {
                    mg_printf(nc, "%s",
                        "HTTP/1.1 500 write fail\r\n"
                        "Content-Length: 29\r\n\r\n"
                        "Failed to write to a file\r\n");
                    errorf("send response to client in push data\n");          
                }
                ///var_name 为form-data类型时的key, data里就为key对应的value
            } else if(mp->var_name != NULL && strlen(mp->var_name) > 0 && strcmp(mp->var_name, "path") == 0) {
                std::string filePath(mp->data.p, mp->data.len);
                tracef("var_name = %s, value = %s\n", mp->var_name, filePath.c_str());
                snprintf(userData->filePath, sizeof(userData->filePath), "%s", filePath.c_str());
            }
 
            nc->flags |= MG_F_SEND_AND_CLOSE;
            break;
        case MG_EV_HTTP_PART_END:
            tracef("file transfer end!\n");
            break;
        case MG_EV_HTTP_MULTIPART_REQUEST_END:
            if(userData != NULL && userData->fp != NULL) {
                mg_printf(nc,
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/plain\r\n"
                    "Connection: close\r\n\r\n"
                    "Written %ld of POST data to a file\n\n",
                (long)ftell(userData->fp));
 
                nc->flags |= MG_F_SEND_AND_CLOSE;
                
                fclose(userData->fp);
                tracef("upload file end, free userData(%p)\n", userData);
                free(userData);
                nc->user_data = NULL;           
            }        
        default:
            break;
    }
}
 
bool isFile(const char *file) 
{
    if(file == nullptr) 
    {
        errorf("file is null\n");
        return false;
    }
 
    const char *pFile = file + 1;
    FILE *fp = nullptr;
    fp = fopen(pFile, "rb");
    if(fp == nullptr) 
    {
        errorf("no such file of %s\n", pFile);
        return false;
    }
 
    fclose(fp);
    return true;
}
 
void eventHandler(struct mg_connection *nc, int event, void *eventData)
{
    switch (event)
    {
        case MG_EV_ACCEPT:
            // tracef("a new accept!!\n");
            // nc->user_data = addConnection(nc); //delConnection时用uc进行connctionID的传递
            break;
        case MG_EV_HTTP_REQUEST:
            // tracef("a new http request!!\n");      
            handleRequest(nc, event, eventData);
            break;
        case MG_EV_CLOSE:
            // tracef("request close!!\n");
            delConnection(nc->user_data);
            nc->user_data = nullptr;
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
 
    //请求正文
    std::string body(msg->body.p, msg->body.len);
    if(body.length() > 0)
    {
        tracef("body size: %lu, %s\n", body.length(), body.c_str());
    }
 
    //请求的地址，不包括ip地址和端口号
    std::string uri(msg->uri.p, msg->uri.len);
    tracef("connect from ip: %s, uri = %s\n", inet_ntoa(connection->sa.sin.sin_addr), uri.c_str());
 
    //设置资源根目录
    if(uri == "/")
    {
        // tracef("url size: %lu, %s\n", uri.length(), url.c_str());
        struct mg_serve_http_opts opts;
        memset(&opts, 0, sizeof(opts));
        opts.enable_directory_listing = "yes";
        opts.document_root = "./";
        mg_serve_http(connection, msg, opts);
    }
 
    //请求的方法，GET、POST、PUT等
    std::string method(msg->method.p, msg->method.len);
    tracef("request method: %s\n", method.c_str());
 
    if(connection->mgr->user_data != NULL)
    {
        struct userData *data = (userData*)connection->mgr->user_data;
        tracef("mgr->user_data is not null, content = %d\n", data->index);
    }
 
    //保存头部信息，用于应答
    for (int32_t index = 0; index < MG_MAX_HTTP_HEADERS; ++index) 
    {
        if (msg->header_names[index].p == nullptr || msg->header_names[index].len == 0
            || std::string(msg->header_names[index].p,msg->header_names[index].len).find("Accept-")) 
        {
            continue;
        }
 
        std::string header(msg->header_names[index].p, msg->header_names[index].len);
        std::string value(msg->header_values[index].p, msg->header_values[index].len);
        // tracef("head[%s] - value[%s]\n", header.c_str(), value.c_str());
        g_headerMap.insert(std::pair<std::string, std::string>(header, value));
    }
 
    //添加版本信息，应答使用
    g_headerMap.insert(std::pair<std::string, std::string>("web", "1.0"));
 
    //保存协议信息
    g_protocol = std::string(msg->proto.p, msg->proto.len);
 
    if(uri.length() > 1 && method == "GET" && isFile(uri.c_str())) 
    {
        std::string file;
        // file.assign(uri, );
        httpFileResponse(connection, uri.c_str());
    } 
    else 
    {
        //尝试用webServer处理，获取处理结果进行返回
        Json::Value bodyData;
        // controllerPtr handler = CWebServer::instance()->getController(uri);
        // if(handler != nullptr)
        // {
        //     handler->process(method.c_str(), uri.c_str(), bodyData);
        // }
        // else
        // {
        //     tracef("no find route[%s]\n", uri.c_str());
        //     bodyData["data"] = "404";
        // }
 
        //应答
        httpResponse(connection, bodyData);        
    }
 
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
 
/*严格按照http应答头格式进行填写：如果有正文，需要在头部字段里写明
** 协议版本|空格|状态码|空格|状态码描述|回车|换行
** 头部字段名|冒号|字段值|回车|换行
** 头部字段名|冒号|字段值|回车|换行
** 头部字段名|冒号|字段值|回车|换行
** .....
** 回车|换行
** 响应正文
*/
void httpResponse(struct mg_connection *connection, Json::Value &body) 
{
    std::string head(g_protocol);
    head.append(" ").append(std::to_string(g_status)).append(" ");
    if(g_stateStr.empty())
    {
        g_stateStr = mg_status_message(g_status); //获取对应的状态描述符
    }
    head.append(g_stateStr).append("\r\n");
    
    // Json::Value data;
    // setContent(data); 
    std::string content = body.toStyledString();
 
    //添加响应报文长度，以让客户端快速读取消息
    setHeader("Content-Length", std::to_string(content.length()).c_str());
 
    //添加头部
    for(auto ite = g_headerMap.begin(); ite != g_headerMap.end(); ite++)
    {
        head.append(ite->first).append(":").append(ite->second).append("\r\n");
    }
 
    //添加正文
    head.append("\r\n").append(content).append("\r\n");
    // tracef("send response: \n%s\n", head.c_str());
    
    mg_send(connection, head.c_str(), head.length());
}

bool parseFileType(const char *fileName, std::string &fileType)
{
    char command[32] = {0};
    snprintf(command, sizeof(command), "%s %s", "file", fileName);
    FILE *fp = popen(command, "r");
    if(fp != NULL)
    {
        char buf[1024] = {0};
        fread(buf, sizeof(buf) - 1 , 1, fp);
        // infof("buf string: %s\n", buf);
        std::string parseStr(buf, strlen(buf));
        if(parseStr.find("text") != std::string::npos)
        {
            fileType = "text/html";
        }
        else if(parseStr.find("ELF") != std::string::npos ||
            parseStr.find("PE32") != std::string::npos)
        {
            fileType = "application/octet-stream";
        }
        else if(parseStr.find("image") != std::string::npos)
        {
            fileType = "image";
            if(parseStr.find("PNG") != std::string::npos)
            {
                fileType.append("/png");
            }
            else if(parseStr.find("JPEG") != std::string::npos)
            {
                fileType.append("/jpeg");
            }
        }
        else if(parseStr.find("bitmap") != std::string::npos)
        {
            fileType = "image/bmp";
        }
        else if(parseStr.find("MPEG") != std::string::npos)
        {
            fileType = "audio/mpeg";
        }
        else if(parseStr.find("MPEG v4") != std::string::npos)
        {
            fileType = "video/mp4";
        }
        else  //默认为不确定的格式进行处理
        {
            fileType = "application/octet-stream";
        }

        return true;
    }

    return false;
}

void httpFileResponse(struct mg_connection *con, const char *fileName) {
    std::string head(g_protocol);
    head.append(" ").append(std::to_string(g_status)).append(" ");
    if(g_stateStr.empty())
    {
        g_stateStr = mg_status_message(g_status); //获取对应的状态描述符
    }
    head.append(g_stateStr).append("\r\n");
    
    //正文
    std::string content;
 
    const char *pFile = fileName + 1;
    FILE *fp = nullptr;
    fp = fopen(pFile, "r");
    if(fp == nullptr) 
    {
        errorf("no such file of %s\n", pFile);
        setHeader("Content-Length", std::to_string(0).c_str());
    } 
    else 
    {
        fseek(fp, 0, SEEK_END);
        size_t len = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        char *buffer = new char[len];
        fread(buffer, len, 1, fp);
        content.assign(buffer, len);
        //添加响应报文长度，以让客户端快速读取消息
        setHeader("Content-Length", std::to_string(len).c_str());
        delete []buffer;
    }
 
    //添加头部
    for(auto ite = g_headerMap.begin(); ite != g_headerMap.end(); ite++)
    {
        head.append(ite->first).append(":").append(ite->second).append("\r\n");
    }

    // int type = getFileType(pFile);
    // infof("file type = %d\n", type);
    std::string contentType;
    if(parseFileType(pFile, contentType))
    {
        head.append("Content-Type: ").append(contentType).append("\r\n");
    }
        
    //添加正文
    head.append("\r\n").append(content).append("\r\n");
 
    
    mg_send(con, head.c_str(), head.length());
}
 
bool hasHeader(const char *header)
{
    return g_headerMap.find(header) != g_headerMap.end();
}
 
void setHeader(const char *header, const char *value)
{
    g_headerMap[header] = value;
}
 
