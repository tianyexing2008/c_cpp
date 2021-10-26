#include "http_client.h"
#include "json.h"

extern bool stringToJson(const std::string &data, Json::Value &out);

void postData()
{
    std::string url("https://xiaoqu.xcuni.com/cert/login");
    std::string userName = "zgdxgzt";
    std::string password = "xcq123456";
    std::string tenantId = "5fca019cb091f5b7e04c0eec";
    std::string client_id = "5cf600563f4c000053007383";
    std::string client_secret = "U2FsdGVkX1%2BoesjvWS3Z8q7ziFCu%2Bp4tU7OQjl%2B5m21l7XMbBBJtof4fAL1S8%2FtJNrGpqIAqtTwXOeiZJFFtHw%3D%3D";
    std::string scope = "read"; //固定值
    std::string grant_type = "password"; //固定值
    std::string cipherText = "FbnsElIS24W33icdY6o9UWrHFjqc6638ZST02duW1%2FjoEx8KUA34q%2FKWRYdfP0%2F4eFTV6teUOryvG1Dm5TC5Tm%2BRDdD%2FAzZLj5IokSud7lFSutNH%2BSTt%2BNGstSRR2VBlOd%2Bxi4NyYUQXBfPBgddkE%2BxAuYFTLV0JDReYbcrqVdA%3D";
    
    std::string post_data = "userName=" + userName + "&password=" + password + "&tenantId=" + tenantId + "&client_id=" + client_id + "&client_secret=" + client_secret +
                        "&scope=" + scope + "&grant_type=" + grant_type + "&ciphertext=" + cipherText;    
    
    std::string recvData;
    Json::Value recvJson;
    CHttpClient httpClient;
    std::string format("Content-Type: application/x-www-form-urlencoded");
    int32_t ret = httpClient.postData(url, post_data, recvData, format);
    stringToJson(recvData, recvJson);
    // printf("postData, ret: %d, data: %s\n", ret, recvJson.toStyledString().c_str());
}

void downloadFile(const std::string &url, const std::string &fileName, onDownloadCb callback, int32_t timeout)
{
    CHttpClient httpClient;
    if(httpClient.downloadFile(url, fileName, callback, timeout) != 0)
    {
        printf("download file fail!\n");
    }

}

void callback(int64_t length)
{
    printf("file length = %ld\n", length);
}

int main(void)
{
    // postData();
    
    std::string url = "https://gimg2.baidu.com/image_search/src=http%3A%2F%2Fb-ssl.duitang.com%2Fuploads%2Fitem%2F201812%2F14%2F20181214195858_VrcEZ.thumb.700_0.jpeg&refer=http%3A%2F%2Fb-ssl.duitang.com&app=2002&size=f9999,10000&q=a80&n=0&g=0n&fmt=jpeg?sec=1627464466&t=77481768ea751c5d494bd1a5f5311e64";
    std::string fileName = "url_download_pic.jpeg";
    downloadFile(url, fileName, /*callback*/nullptr, 10);
    return 0;
}

