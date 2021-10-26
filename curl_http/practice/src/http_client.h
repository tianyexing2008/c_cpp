#include <stdio.h>
#include <stdlib.h>
#include <string>

typedef void (*onDownloadCb)(int64_t dataLength);

class CHttpClient
{
public:
    int32_t postData(const std::string &url, const std::string &body, std::string &response, std::string &format, int32_t timeout = 60);

    int32_t downloadFile(const std::string &url, const std::string &fileName, onDownloadCb callback, int32_t timeout = 10);
};