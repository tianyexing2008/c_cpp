#ifndef __NET_ACCEPTOR_H__
#define __NET_ACCEPTOR_H__

#include "noncopyable.h"
#include "channel.h"
#include "socket.h"

namespace net
{

class CEventLoop;
class CInetAddress;

class CAcceptor: muduo::noncopyable
{
public:
    typedef std::function<void (int sockfd, const CInetAddress&)> NewConnectionCallback;
    CAcceptor(CEventLoop *loop, const CInetAddress& listenAddr, bool reUsePort);
    ~CAcceptor();
    void setNewConnectionCallback(const NewConnectionCallback &cb)
    {
        mNewConnectionCallback = cb;
    }

    void listen();

    bool listening()const
    {
        return mListening;
    }
private:
    void handleRead();
    CEventLoop *mLoop;
    CSocket mAcceptSocket;
    CChannel mAcceptChannel;
    NewConnectionCallback mNewConnectionCallback;
    bool mListening;
    int mIdleFd;
};



}


#endif