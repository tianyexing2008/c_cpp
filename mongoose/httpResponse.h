#ifndef __HTTP_RESPONSE_H__
#define __HTTP_RESPONSE_H__

#include "../json/json.h"
#include "http.h"
#include <map>

class CHttpResponse: public IHttp
{
    typedef std::map<std::string, std::string> HEADMAP;
public:
    CHttpResponse();
    ~CHttpResponse();

    void setHead(const char *head, const char *value);

    bool getHead(std::string &heads);

    void getContent(std::string &content);

    bool setBody(const char *fileName);

    void setBody(const Json::Value &Json);

    void setProtocol(const char *head, const char *value);

    void setVersion();
    
    bool parseFileType(const char *fileName, std::string &fileType);
private:
    HEADMAP mHeadMap;
    char    *mBody;
    int     mLen;
};

#endif