
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <string>
#include "curl.h"
#include "http_client.h"
#include "json.h"

bool stringToJson(const std::string &data, Json::Value &out)
{
    Json::CharReaderBuilder readerBuild;
    Json::CharReader *reader = readerBuild.newCharReader();
    std::string error;
    reader->parse(data.c_str(), data.c_str() + data.length(), &out, &error);

    delete reader;
    return true;
}

typedef struct {
    char*       memory;
    size_t      size;
} MemoryStruct;

typedef struct {
    FILE*           fpFile;
    onDownloadCb    downloadCB;
} DownloadStruct;

static size_t downloadFileCallback(void *ptr, size_t size, size_t nmemb, void *use) 
{
    DownloadStruct* download = reinterpret_cast<DownloadStruct*>(use);
    ::fwrite(ptr, size, nmemb, download->fpFile);

    int64_t dataSize = size * nmemb;
    
    if(download->downloadCB) 
    {
        download->downloadCB(dataSize);
    }

    return dataSize;
}

static size_t httpResponseCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    MemoryStruct *mem = reinterpret_cast<MemoryStruct *>(userp);

    char *ptr = (char*)realloc(mem->memory, mem->size + realsize + 1);
    if(ptr == nullptr) {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    printf("data size: %lu\n", mem->size);
    return realsize;
}

int32_t CHttpClient::postData(const std::string &url, const std::string &params, std::string &response, std::string &format, int32_t timeout)
{
    MemoryStruct chunk;
    memset(&chunk, 0x00, sizeof(MemoryStruct));

    chunk.memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */

    CURL *curl = curl_easy_init();
    if (nullptr == curl) 
    {
        printf("get data curl_easy_init failed !!!\n");
        free(chunk.memory);
        return -1;
    }

    struct curl_slist* headers = nullptr;
    // headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    headers = curl_slist_append(headers, format.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, httpResponseCallback);
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, params.c_str());
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);

    CURLcode curlCode = curl_easy_perform(curl);
    if (curlCode != CURLE_OK) 
    {
        printf("post data curl_easy_perform() url:%s, failed: %s, \n", url.c_str(), curl_easy_strerror(curlCode));
        free(chunk.memory);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl); return -1;
    }
    curl_slist_free_all(headers);

    response = chunk.memory;
    curl_easy_cleanup(curl);
    printf("recv data size: %lu\n", chunk.size);
    free(chunk.memory);
    return 0;   
}

int32_t CHttpClient::downloadFile(const std::string &url, const std::string &fileName, onDownloadCb callback, int32_t timeout)
{
    //create new file
    FILE *fp = fopen(fileName.c_str(), "w");
    if(fp == NULL)
    {
        printf("fopen file fail!\n");
        return -1;
    }

    DownloadStruct download;
    memset(&download, 0, sizeof(download));
    download.fpFile = fp;
    download.downloadCB = callback;

    CURLcode retCode = CURLE_OK;
    CURL *curl = curl_easy_init();

    /**增加下载中长时间没有数据的处理逻辑问题*/
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 1 ); //bytes
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 10 ); //s

    curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);

    /// enable TCP keep-alive for this transfer
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

    /// keep-alive idle time to 120 seconds
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPIDLE, 120L);

    /// interval time between keep-alive probes: 60 seconds
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPINTVL, 60L);

    ///当HTTP返回值大于等于400的时候，请求失败, 404错误
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);

    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);   

    ///设置连接超时时间
    retCode = curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10);

    ///设置请求链接
    retCode = curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    ///设置 CURLOPT_WRITEFUNCTION 的最后一个参数值,　将作为回调参数传入
    retCode = curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&download);

    ///设置接收到数据之后的回调函数
    retCode = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, downloadFileCallback);

    ///https 去除证书校验
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    ///开始
    int32_t responseCode = 0;
    retCode = curl_easy_perform(curl);
    if (retCode == CURLE_OK) 
    {
        //获取返回信息
        retCode = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
    }

    curl_easy_cleanup(curl);
    ::fclose(fp);

    if (retCode == CURLE_OK) 
    {
        return 0;
    }

    printf("download file remoteUrl[%s] localFilePath[%s] failed: %s, \n", url.c_str(), fileName.c_str(), curl_easy_strerror(retCode));
    return -1;    
}

