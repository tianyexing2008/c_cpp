#ifndef __NET_TCPSERVER_H__
#define __NET_TCPSERVER_H__

#include "noncopyable.h"
#include "inetAddress.h"
#include "callback.h"
#include "Atomic.h"
#include <functional>
#include <map>
#include <memory>

namespace net
{

class CAcceptor;
class CEventLoop;
class CEventLoopThreadPool;

class CTcpServer: muduo::noncopyable
{
public:
    typedef std::function<void (CEventLoop*)> ThreadInitCallBack;
    enum Option
    {
        eNoReusePort,
        eReusePort,
    };

    CTcpServer(CEventLoop *loop, const CInetAddress &listenAddr, const std::string nameArg, Option portReuseOrNot = eNoReusePort);
    ~CTcpServer();

    const std::string& ipPort() const { return mIpPort; }
    const std::string& name() const { return mName; }
    CEventLoop* getLoop() const { return mLoop; }
    
    void setThreadNum(int numThreads);
    void setThreadInitCallback(const ThreadInitCallBack& cb);
    std::shared_ptr<CEventLoopThreadPool> threadPool();
    void start();
    void setConnectionCallback(ConnectionCallback cb);
    void setMessageCallback(MessageCallback cb);
    void setWriteCompleteCallback(WriteCompleteCallback cb);
private:
    void newConnection(int sockfd, const CInetAddress &peerAddr);
    void removeConnection(const CTcpConnectionPtr &conn);
    void removeConnectionInLoop(const CTcpConnectionPtr &conn);

    typedef std::map<std::string, CTcpConnectionPtr> ConnectionMap;

    CEventLoop *mLoop;
    const std::string mIpPort;
    const std::string mName;
    std::unique_ptr<CAcceptor> mAccpetor;
    std::shared_ptr<CEventLoopThreadPool> mThreadPool;
    ConnectionCallback mConnectionCallback;
    MessageCallback mMessageCallback;
    WriteCompleteCallback mWriteCompleteCallback;
    ThreadInitCallBack mThreadInitCallback;
    muduo::AtomicInt32 mStarted;
    int mNextConnId;
    ConnectionMap mConnections;
};


}
#endif
