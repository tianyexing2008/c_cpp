#include <string>


class CRSAEnCrypt 
{
public:

	static CRSAEnCrypt *getInstance();

	void initkey(const char *privateKey, const char *publicKey);

	//公钥加密 + 分片,1024位，明文最大长度为1024/8 - 11 = 117
	//所以需要分片处理
	std::string rsaSliptPublicEncrypt(const std::string &clearText);

	//公钥解密 + 分片
	std::string rsaSliptPublicDecrypt(const std::string &clearText);

    //私钥解密 + 分片
    std::string rsaSliptPrivateDecrypt(const std::string &clearText);

private:
    //私钥解密
    std::string rsaPrivateDecrypt(const std::string &clearText);

	//公钥加密
	std::string rsaPublicEncrypt(const std::string &clearText);

	//公钥解密
	std::string rsaPublicDecrypt(const std::string &clearText);

private:
    std::string m_privateKey;
	std::string m_pubKey;
};



