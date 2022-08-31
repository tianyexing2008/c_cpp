#ifndef __IHTTP_H__
#define __IHTTP_H__

#include <string>

class IHttp
{
public:
    IHttp()
    {

    }
    virtual ~IHttp()
    {

    }
    virtual void setHead(const char *head, const char *value) = 0;

    virtual bool getHead(std::string &heads) = 0;

    virtual void getContent(std::string &content) = 0;
};

#endif
