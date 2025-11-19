
#include "tcpServer.h"
#include "logging.h"
#include "acceptor.h"
#include "eventLoop.h"
#include "eventLoopThreadPool.h"
#include "socketsOps.h"
#include "tcpConnection.h"

#include <stdio.h>  // snprintf

using namespace muduo;
using namespace net;

CTcpServer::CTcpServer(CEventLoop *loop, const CInetAddress &listenAddr, const std::string nameArg, Option portReuseOrNot)
:mLoop(CHECK_NOT_NULL(loop))
,mIpPort(listenAddr.toIpPort())
,mName(nameArg)
,mAccpetor(new CAcceptor(loop, listenAddr, portReuseOrNot))
,mThreadPool(new CEventLoopThreadPool(loop, nameArg))
,mConnectionCallback(defaultConnectionCallback)
,mMessageCallback(defaultMessageCallback)
,mNextConnId(1)
{
    mAccpetor->setNewConnectionCallback(std::bind(&CTcpServer::newConnection, this, _1, _2));
}

CTcpServer::~CTcpServer()
{
    mLoop->assertInLoopThread();
    LOG_TRACE << "TcpServer::~TcpServer [" << mName << "] destructing";
    for(auto &item : mConnections)
    {
        CTcpConnectionPtr conn(item.second);
        item.second.reset();
        conn->getLoop()->runInLoop(std::bind(&CTcpConnection::connectDestroyed, conn));
    }
}

void CTcpServer::setThreadInitCallback(const ThreadInitCallBack& cb)
{
    mThreadInitCallback = cb;
}

std::shared_ptr<CEventLoopThreadPool> CTcpServer::threadPool() 
{
    return mThreadPool;
}

void CTcpServer::setConnectionCallback(ConnectionCallback cb)
{
    mConnectionCallback = cb;
}

void CTcpServer::setMessageCallback(MessageCallback cb)
{
    mMessageCallback = cb;
}

void CTcpServer::setWriteCompleteCallback(WriteCompleteCallback cb)
{
    mWriteCompleteCallback = cb;
}

void CTcpServer::setThreadNum(int numThreads)
{
    mThreadPool->setThreadNum(numThreads);
}

void CTcpServer::start()
{
    if(mStarted.getAndSet(1) == 0)
    {
        mThreadPool->start(mThreadInitCallback);
        mLoop->runInLoop(std::bind(&CAcceptor::listen, getPointer(mAccpetor)));
    }
}

void CTcpServer::newConnection(int sockfd, const CInetAddress &peerAddr)
{
    mLoop->assertInLoopThread();
    CEventLoop* ioLoop = mThreadPool->getNextLoop();
    char buf[64];
    snprintf(buf, sizeof buf, "-%s#%d", mIpPort.c_str(), mNextConnId);
    ++mNextConnId;
    std::string connName = mName + buf;

    LOG_INFO << "TcpServer::newConnection [" << mName
            << "] - new connection [" << connName
            << "] from " << peerAddr.toIpPort();
    CInetAddress localAddr(sockets::getLocalAddr(sockfd));
    // FIXME poll with zero timeout to double confirm the new connection
    // FIXME use make_shared if necessary
    CTcpConnectionPtr conn(new CTcpConnection(ioLoop,
                                            connName,
                                            sockfd,
                                            localAddr,
                                            peerAddr));
    mConnections[connName] = conn;
    conn->setConnectionCallback(mConnectionCallback);
    conn->setMessageCallback(mMessageCallback);
    conn->setWriteCompleteCallback(mWriteCompleteCallback);
    conn->setCloseCallback(std::bind(&CTcpServer::removeConnection, this, _1)); // FIXME: unsafe
    ioLoop->runInLoop(std::bind(&CTcpConnection::connectEstablished, conn));    
}

void CTcpServer::removeConnection(const CTcpConnectionPtr& conn)
{
    // FIXME: unsafe
    mLoop->runInLoop(std::bind(&CTcpServer::removeConnectionInLoop, this, conn));
}

void CTcpServer::removeConnectionInLoop(const CTcpConnectionPtr& conn)
{
    mLoop->assertInLoopThread();
    LOG_INFO << "TcpServer::removeConnectionInLoop [" << mName
            << "] - connection " << conn->name();
    size_t n = mConnections.erase(conn->name());
    (void)n;
    CEventLoop* ioLoop = conn->getLoop();
    ioLoop->queueInLoop(std::bind(&CTcpConnection::connectDestroyed, conn));
}