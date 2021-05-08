#include <string>


class CRSAEnCrypt 
{
public:

	static CRSAEnCrypt *getInstance();

	void initkey(const char *privateKey, const char *publicKey);

	//公钥加密, 1024位，明文最大长度为1024/8 - 11 = 117
	//所以需要分段处理
	std::string rsaPublicKeyEncryptSplit(const std::string &clearText);

	//公钥解密，分段处理
	std::string rsaPublicKeyDecryptSplit(const std::string &clearText);

    //私钥解密，分段处理
    std::string rsaPrivateKeyDecryptSplit(const std::string &clearText);

private:
    //私钥解密
    std::string rsaPrivateKeyDecrypt(const std::string &clearText);

	//公钥加密
	std::string rsaPublicKeyEncrypt(const std::string &clearText);

	//公钥解密
	std::string rsaPublicKeyDecrypt(const std::string &clearText);

private:
    std::string m_privateKey;
	std::string m_pubKey;
};



