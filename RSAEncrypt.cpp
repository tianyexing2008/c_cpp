#include <openssl/pem.h>
#include <cstring>
#include <iostream>
#include <json.h>
#include "RSAEncrypt.h"

static const std::string base64Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static inline bool isBase64(uint8_t c) {
    return (isalnum(c) || (c == '+') || (c == '/'));
}


std::string base64Encode(uint8_t const* src, uint32_t len) {

    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char charArray3[3] = {0};
    unsigned char charArray4[4] = {0};

    while (len--) {
        charArray3[i++] = *(src++);
        if (i == 3) {
            charArray4[0] = (charArray3[0] & 0xfc) >> 2;
            charArray4[1] = ((charArray3[0] & 0x03) << 4) + ((charArray3[1] & 0xf0) >> 4);
            charArray4[2] = ((charArray3[1] & 0x0f) << 2) + ((charArray3[2] & 0xc0) >> 6);
            charArray4[3] = charArray3[2] & 0x3f;

            for (i = 0; (i < 4); i++) {
                ret += base64Chars[charArray4[i]];
            }
                
            i = 0;
        }
    }

    if (i)
    {
        for (j = i; j < 3; j++) {
            charArray3[j] = '\0';    
        }

        charArray4[0] = (charArray3[0] & 0xfc) >> 2;
        charArray4[1] = ((charArray3[0] & 0x03) << 4) + ((charArray3[1] & 0xf0) >> 4);
        charArray4[2] = ((charArray3[1] & 0x0f) << 2) + ((charArray3[2] & 0xc0) >> 6);

        for (j = 0; (j < i + 1); j++) {
            ret += base64Chars[charArray4[j]];    
        }

        while ((i++ < 3)) {
            ret += '=';    
        }
    }

    return ret;
}


std::string base64Decode(std::string const& src) {
    int32_t srcPosition = src.size();
    uint8_t charArray4[4] = {0};
    uint8_t charArray3[3] = {0};
    std::string ret;

    int32_t i = 0;
    int32_t j = 0;
    int32_t srcIndex = 0;
    while (srcPosition-- && (src[srcIndex] != '=') && isBase64(src[srcIndex])) {
        charArray4[i++] = src[srcIndex]; srcIndex++;
        if (i == 4) {
            for (i = 0; i < 4; i++) {
                charArray4[i] = base64Chars.find(charArray4[i]);
            }

            charArray3[0] = (charArray4[0] << 2) + ((charArray4[1] & 0x30) >> 4);
            charArray3[1] = ((charArray4[1] & 0xf) << 4) + ((charArray4[2] & 0x3c) >> 2);
            charArray3[2] = ((charArray4[2] & 0x3) << 6) + charArray4[3];

            for (i = 0; i < 3; i++) {
                ret += charArray3[i];
            }

            i = 0;
        }
    }

    if (i) {
        for (j = 0; j < i; j++) {
            charArray4[j] = base64Chars.find(charArray4[j]);
        }

        charArray3[0] = (charArray4[0] << 2) + ((charArray4[1] & 0x30) >> 4);
        charArray3[1] = ((charArray4[1] & 0xf) << 4) + ((charArray4[2] & 0x3c) >> 2);

        for (j = 0; j < i - 1; j++) {
            ret += charArray3[j];
        }
    }

    return ret;
}

CRSAEnCrypt* CRSAEnCrypt::getInstance() 
{
    static CRSAEnCrypt instance;
    return &instance;
}

//
void CRSAEnCrypt::initkey(const char *privateKey, const char *publicKey)
{
    if(m_pubKey.empty())
    {
        m_pubKey = publicKey;
    }

    if(m_privateKey.empty())
    {
        m_privateKey = privateKey;
    }
}

std::string CRSAEnCrypt::rsaPublicEncrypt(const std::string &clearText)
{
    std::string strRet;
    BIO *keybio = BIO_new_mem_buf((unsigned char *)m_pubKey.c_str(), -1);

    RSA* rsa = RSA_new();
    rsa = PEM_read_bio_RSA_PUBKEY(keybio, NULL, NULL, NULL);
    if (!rsa)
    {
        BIO_free_all(keybio);
        return std::string("");
    }

    int len = RSA_size(rsa);
    //int len = 1028;
    char *encryptedText = (char *)malloc(len + 1);
    memset(encryptedText, 0, len + 1);

    // 加密函数 
    int ret = RSA_public_encrypt(clearText.length(), (const unsigned char*)clearText.c_str(), (unsigned char*)encryptedText, rsa, RSA_PKCS1_PADDING);
    if (ret >= 0)
    {
        strRet = std::string(encryptedText, ret);
    }

    // 释放内存 
    free(encryptedText);
    BIO_free_all(keybio);
    RSA_free(rsa);

    return strRet;
}

//公钥加密 + 分片
std::string CRSAEnCrypt::rsaSliptPublicEncrypt(const std::string &clearText)
{
    std::string result;
    std::string input;
    result.clear();

    for(int i = 0 ; i< clearText.length() / 117; i++)
    {  
        input.clear();
        input.assign(clearText.begin() + i*117, clearText.begin() + i*117 + 117);

        std::string tmp;
        tmp = rsaPublicEncrypt(input);
        result = result + tmp;//base64Encode((uint8_t*)tmp.c_str(), tmp.length());

    }

    if(clearText.length() % 117 != 0)
    {
        int tem1 = clearText.length() / 117 * 117;
        input.clear();
        input.assign(clearText.begin()+ tem1, clearText.end());

        std::string tmp;
        tmp = rsaPublicEncrypt(input);
        result = result + tmp;//base64Encode((uint8_t*)tmp.c_str(), tmp.length());
    }

    printf("before base64 len: %lu\n", result.length());
    std::string encode_str = base64Encode((uint8_t*)result.c_str(), result.length());
    printf("after base64 len: %lu\n", encode_str.length());
    return encode_str;
}

// 公钥解密
std::string CRSAEnCrypt::rsaPublicDecrypt(const std::string &clearText)
{
    std::string strRet;
    BIO *keybio = BIO_new_mem_buf((unsigned char *)m_pubKey.c_str(), -1);
    RSA* rsa = RSA_new();
    rsa = PEM_read_bio_RSA_PUBKEY(keybio, NULL, NULL, NULL);
    if (!rsa)
    {
        BIO_free_all(keybio);
        return std::string("");
    }

    int len = RSA_size(rsa);
    //int len = 1028;
    char *encryptedText = (char *)malloc(len + 1);
    memset(encryptedText, 0, len + 1);
    //解密
    int ret = RSA_public_decrypt(clearText.length(), (const unsigned char*)clearText.c_str(), (unsigned char*)encryptedText, rsa, RSA_PKCS1_PADDING);
    if (ret >= 0)
    strRet = std::string(encryptedText, ret);

    // 释放内存
    free(encryptedText);
    BIO_free_all(keybio);
    RSA_free(rsa);
    return strRet;
}

//私钥解密
std::string CRSAEnCrypt::rsaPrivateDecrypt(const std::string &clearText)
{
    std::string strRet;
    BIO *keybio = BIO_new_mem_buf((unsigned char *)m_privateKey.c_str(), -1);
    RSA* rsa = RSA_new();
    rsa = PEM_read_bio_RSAPrivateKey(keybio, NULL, NULL, NULL);
    if (!rsa)
    {
        BIO_free_all(keybio);
        return std::string("");
    }

    int len = RSA_size(rsa);
    char *encryptedText = (char *)malloc(len + 1);
    memset(encryptedText, 0, len + 1);

    //解密
    int ret = RSA_private_decrypt(clearText.length(), (const unsigned char*)clearText.c_str(), (unsigned char*)encryptedText, rsa, RSA_PKCS1_PADDING);
    if (ret >= 0)
    {
       strRet = std::string(encryptedText, ret); 
    }
    
    // 释放内存
    free(encryptedText);
    BIO_free_all(keybio);
    RSA_free(rsa);

    return strRet;
}

//公钥解密 + 分片
std::string CRSAEnCrypt::rsaSliptPublicDecrypt(const std::string &data)
{
    printf("before base64Decode len: %lu\n", data.length());
    std::string clearText = base64Decode(data);
    printf("after base64Decode len: %lu\n", clearText.length());
    std::string result;
    std::string input;
    result.clear();
    for(int i = 0 ; i< clearText.length() / 128; i++)
    {
        input.clear();
        input.assign(clearText.begin() + i * 128, clearText.begin() + i * 128 + 128);
        result = result + rsaPublicDecrypt(input);
    }

    if(clearText.length() % 128 != 0)
    {
        int tem1 = clearText.length()/128 * 128;
        input.clear();
        input.assign(clearText.begin()+ tem1, clearText.end());
        result = result + rsaPublicDecrypt(input);
    }

    return result;
}

//私钥解密 + 分片
std::string CRSAEnCrypt::rsaSliptPrivateDecrypt(const std::string &Text)
{
    printf("before base64Decode len: %lu\n", Text.length());
    std::string clearText = base64Decode(Text);
    printf("after base64Decode len: %lu\n", clearText.length());
    
    //printf("BaseDecode len: %lu\n", clearText.length());
    std::string result;
    std::string input;
    result.clear();
    for(int i = 0 ; i< clearText.length() / 128; i++)
    {
        input.clear();
        input.assign(clearText.begin() + i * 128, clearText.begin() + i * 128 + 128);
        result = result + rsaPrivateDecrypt(input);
    }

    if(clearText.length() % 128 != 0)
    {
        int tem1 = clearText.length()/128 * 128;
        input.clear();
        input.assign(clearText.begin()+ tem1, clearText.end());
        result = result + rsaPrivateDecrypt(input);
    }

    return result;    
}



const char *publicKey = "-----BEGIN PUBLIC KEY-----\n\
MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCZHFgCmrguBaZ2prKYIkZm1OWO\n\
VrwCa3OWO/5puCEuDPdrS4cqX0N6FBvni6mXS9zuyI8wya7nWhvttn1tybMO4wv5\n\
m6LnuWmcfOn0lT6Mz9DS+Cy5YPn8OeFy0A6Ys/B5hBng1Sw3OclduP5X8QDM3Xza\n\
ExfdGuqiVWWugtI4zwIDAQAB\n\
-----END PUBLIC KEY-----";

const char *privateKey = "-----BEGIN RSA PRIVATE KEY-----\n\
MIICdwIBADANBgkqhkiG9w0BAQEFAASCAmEwggJdAgEAAoGBAJkcWAKauC4Fpnam\n\
spgiRmbU5Y5WvAJrc5Y7/mm4IS4M92tLhypfQ3oUG+eLqZdL3O7IjzDJrudaG+22\n\
fW3Jsw7jC/mboue5aZx86fSVPozP0NL4LLlg+fw54XLQDpiz8HmEGeDVLDc5yV24\n\
/lfxAMzdfNoTF90a6qJVZa6C0jjPAgMBAAECgYAGZFiIQ01NHo9EhNEP6N5njJvI\n\
xXYz46h/rSGB6F36PjBWGmEaM7/taMmBcSMzXcdrcJQJxWG35tsjoWq7GqCO/f3l\n\
uYlSoewy3WprA+Chs3e4ouYQO7ctlSQdjPeolUYpv3q4WGr1xQoLBMdpdcZBiQ6P\n\
bX3CgvFvKjcipXyHAQJBANCnrvDcC4VKf1wg67Yqkkx/D1Co+8Z+XptJcPcADUhb\n\
yhTJ8swQJJmHW2sQgkiBbHd6FW8ZFASyDfLTzRxfrzkCQQC72jaCceKjA7fspFDj\n\
+gGdYpi+F6h6Wc9eD4V9P23EBo34oAovuW9x3bMR0Y4P0odjAfloYBhdtiMDqiCX\n\
7aBHAkEArQGcaFHLq6VtnLIfP1hlHdBsnnC+8oJtZ0ypwePlH44cLMiV7OWlszcs\n\
ccWqgPvvN9GeXBPrKUmJj0JW26Pq4QJBAIQnQnvIVLFr10OCYWnQorwu9dedWygf\n\
8HNype1z5uul1NDY/fGPGejYF7bsXm2hJR+w7t3P5LRggwd78wwO3tcCQFVDg4Yf\n\
Ef9lgFYflqiSAE9TOuyz7uOR+LgtJ1A+c8PnaqC2zbotuuYpnLFb26fZ3DQsZpWY\n\
ME+8N7vjifjYXW0=\n\
-----END RSA PRIVATE KEY-----";

std::string jsonToString(const Json::Value &value)
{

    Json::StreamWriterBuilder build;
    build.settings_["indentation"] = "";
    std::unique_ptr<Json::StreamWriter> writer(build.newStreamWriter());
    std::ostringstream os;
    writer->write(value,&os);
    return os.str();
}



int main()
{
    Json::Value postMsg;
    postMsg["userName"] = "zgdxgzt";
    postMsg["password"] = "xcq123456";
    postMsg["tenantId"] = "5fca019cb091f5b7e44c0eec";
    postMsg["client_id"] = "5cf600563f4c000053007383";
    postMsg["client_secret"] = "U2FsdGVkX1+oesjvWS3Z8q7ziFCu+p4tU7OQjl+5m21l7XMbBBJtof4fAL1S8/tJNrGpqIAqtTwXOeiZJFFtHw==";
    postMsg["scope"] = "read";  //固定值
    postMsg["grant_type"] = "password"; //固定值


    CRSAEnCrypt::getInstance()->initkey(privateKey, publicKey);
    std::string sourceStr = jsonToString(postMsg);
    std::string rsaEncryptStr = CRSAEnCrypt::getInstance()->rsaSliptPublicEncrypt(sourceStr);
    printf("after rsaEncryptStr len: %lu\n", rsaEncryptStr.length());

    std::string rsaDecryptStr = CRSAEnCrypt::getInstance()->rsaSliptPrivateDecrypt(rsaEncryptStr);
    printf("after rsaDecryptStr: %lu, %s\n", rsaDecryptStr.length(), rsaDecryptStr.c_str());
    
    //单独测试base64加密、解密
    // std::string encodeStr = base64Encode((uint8_t *)sourceStr.c_str(), sourceStr.length());
    // printf("decodeStr len: %lu\n", encodeStr.length());
    // std::string decodeStr = base64Decode(encodeStr);
    // printf("decodeStr len: %lu, %s\n", decodeStr.length(), decodeStr.c_str());




    return 0;
}