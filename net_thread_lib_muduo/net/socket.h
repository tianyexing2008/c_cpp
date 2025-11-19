#ifndef __NET_SOCKET_H__
#define __NET_SOCKET_H__

#include "noncopyable.h"

struct tcp_info;
namespace net
{

class CInetAddress;

class CSocket: muduo::noncopyable
{
public:
    CSocket(int sockfd):mSockFd(sockfd)
    {
        
    }
    ~CSocket();

    int fd()const {return mSockFd;}
    bool getTcpInfo(struct tcp_info*)const;
    bool getTcpInfoString(char *buf, int len)const;
    void bindAddress(const CInetAddress& localAddr);
    void listen();
    int accept(CInetAddress *peerAddr);
    void shutdownWrite();
    void setTcpNoDelay(bool on);
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);

private:
    const int mSockFd;
};


}
#endif