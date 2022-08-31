#include <string.h>
#include "httpResponse.h"
#include "../logFormatPrt/log.h"

CHttpResponse::CHttpResponse()
{
    mBody = nullptr;
    mLen = 0;
    setVersion();
}

CHttpResponse::~CHttpResponse()
{
    if(mBody != nullptr)
    {
        delete []mBody;
        mBody = nullptr;
        mLen = 0;
    }
}

void CHttpResponse::setHead(const char *head, const char *value)
{
    mHeadMap.insert(std::pair<std::string, std::string>(head, value));
}

bool CHttpResponse::getHead(std::string &heads)
{
    if(mHeadMap.size() != 0)
    {
        for(auto ite = mHeadMap.begin(); ite != mHeadMap.end(); ite++)
        {
            heads.append(ite->first).append(":").append(ite->second).append("\r\n");
        }
        return true;
    }

    return false;
}

void CHttpResponse::getContent(std::string &content)
{
    std::string head;
    head.append("HTTP/1.1 200 ok").append("\r\n");
    getHead(head);
    if(mBody != nullptr)
    {
        std::string content(mBody, mLen);
        tracef("send head: %s\n", head.c_str());
        head.append("\r\n").append(content).append("\r\n");
    }
    else
    {
        head.append("\r\n");
    }

    content = head;
}

bool CHttpResponse::setBody(const char *fileName)
{
    FILE *fp = nullptr;
    fp = fopen(fileName, "r");
    fseek(fp, 0, SEEK_END);
    mLen = ftell(fp);
    
    if(mLen != 0)
    {
        fseek(fp, 0, SEEK_SET);
        mBody = new char[mLen];
        fread(mBody, mLen, 1, fp);        
    }

    //添加响应报文长度，以让客户端快速读取消息
    setHead("Content-Length", std::to_string(mLen).c_str());
    
    std::string fileType;
    if(parseFileType(fileName, fileType))
    {
        setHead("Content-Type", fileType.c_str());
    }

    return true;
}

void CHttpResponse::setProtocol(const char *head, const char *value)
{

}

void CHttpResponse::setVersion()
{
    mHeadMap.insert(std::pair<std::string, std::string>("webVersion", "1.0"));
}

bool CHttpResponse::parseFileType(const char *fileName, std::string &contentType)
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
        if(parseStr.find("HTML") != std::string::npos)
        {
            contentType = "text/html";
        }
        else if(parseStr.find("text") != std::string::npos)
        {
            contentType = "text/plain";
            if(parseStr.find("UTF-8") != std::string::npos)
            {
                contentType.append(";charset=utf-8");
            }
        }
        else if(parseStr.find("ELF") != std::string::npos ||
            parseStr.find("PE32") != std::string::npos)
        {
            contentType = "application/octet-stream";
        }
        else if(parseStr.find("image") != std::string::npos)
        {
            contentType = "image";
            if(parseStr.find("PNG") != std::string::npos)
            {
                contentType.append("/png");
            }
            else if(parseStr.find("JPEG") != std::string::npos)
            {
                contentType.append("/jpeg");
            }
        }
        else if(parseStr.find("icon") != std::string::npos)
        {
            contentType.append("image/x-icon");
        }
        else if(parseStr.find("bitmap") != std::string::npos)
        {
            contentType = "image/bmp";
        }
        else if(parseStr.find("MPEG") != std::string::npos)
        {
            contentType = "audio/mpeg";
        }
        else if(parseStr.find("MPEG v4") != std::string::npos)
        {
            contentType = "video/mp4";
        }
        else if(parseStr.find("empty") != std::string::npos)
        {
            contentType = "text/plain";
        }
        else  //默认为不确定的格式进行处理
        {
            contentType = "application/octet-stream";
        }

        return true;
    }

    return false;
}