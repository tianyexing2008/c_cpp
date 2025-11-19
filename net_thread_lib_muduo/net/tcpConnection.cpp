#include "tcpConnection.h"
#include "logging.h"
#include "callback.h"
#include "socket.h"
#include "channel.h"
#include "eventLoop.h"
#include "socketsOps.h"
#include "weakCallback.h"

extern const char *strerror_tl(int saveError);

namespace net
{

void defaultConnectionCallback(const CTcpConnectionPtr& conn)
{
    LOG_TRACE << conn->localAddress().toIpPort() << " -> "
            << conn->peerAddress().toIpPort() << " is "
            << (conn->connected() ? "UP" : "DOWN");
    // do not call conn->forceClose(), because some users want to register message callback only.
}

void defaultMessageCallback(const CTcpConnectionPtr& conn, CBuffer* buffer, CTimestamp receiveTime)
{
    buffer->retrieveAll();
}

CTcpConnection::CTcpConnection(CEventLoop *loop, std::string &name, int sockfd, const CInetAddress &localAddr, const CInetAddress &peerAddr)
:mLoop(loop)
,mName(name)
,mState(kConnecting)
,mReading(true)
,mSocket(new CSocket(sockfd))
,mChannel(new CChannel(loop, sockfd))
,mLocalAddr(localAddr)
,mPeerAddr(peerAddr)
,mHighWaterMark(64*1024*1024)
{
    mChannel->setReadCallback(std::bind(&CTcpConnection::handleRead, this, _1));
    mChannel->setWriteCallback(std::bind(&CTcpConnection::handleWrite, this));
    mChannel->setCloseCallback(std::bind(&CTcpConnection::handleClose, this));
    mChannel->setErrorCallback(std::bind(&CTcpConnection::handleError, this));
    LOG_TRACE << "TcpConnection::ctor[" <<  mName << "] at " << this  << " fd=" << sockfd;
    mSocket->setKeepAlive(true);
}
CTcpConnection::~CTcpConnection()
{
    LOG_DEBUG << "TcpConnection::dtor[" <<  mName << "] at " << this
            << " fd=" << mChannel->fd()
            << " state=" << stateToString();
}

bool CTcpConnection::getTcpInfo(::tcp_info* tcpi)const
{
    return mSocket->getTcpInfo(tcpi);
}

std::string CTcpConnection::getTcpInfoString() const
{
    char buf[1024];
    buf[0] = '\0';
    mSocket->getTcpInfoString(buf, sizeof buf);
    return buf;
}

void CTcpConnection::send(const void *msg, size_t len)
{
    send(CStringPiece(static_cast<const char*>(msg), len));
}

void CTcpConnection::send(const CStringPiece &message)
{
    LOG_INFO << "will send msg: '" << message << "'";
    if(mState == kConnected)
    {
        if(mLoop->isInLoopThread())
        {
            sendInLoop(message);
        }
        else
        {
            void (CTcpConnection::*fp)(const CStringPiece &msg) = &CTcpConnection::sendInLoop;
            mLoop->runInLoop(std::bind(fp, this, message.asString()));
        }
    }
}

void CTcpConnection::send(CBuffer *buf)
{
    if(mState == kConnected)
    {
        if(mLoop->isInLoopThread())
        {
            sendInLoop(buf->peek(), buf->readableBytes());
            buf->retrieveAll();
        }
        else
        {
            void (CTcpConnection::*fp)(const CStringPiece &message) = &CTcpConnection::sendInLoop;
            mLoop->runInLoop(std::bind(fp, this, buf->retrieveAllAsString()));
        }
    }
}

void CTcpConnection::sendInLoop(const CStringPiece &message)
{
    sendInLoop(message.data(), message.size());
}

void CTcpConnection::sendInLoop(const void *message, size_t len)
{
    mLoop->assertInLoopThread();
    ssize_t nwrote = 0;
    size_t remaining = len;
    bool faultError = false;
    if(mState == kDisconnected)
    {
        LOG_WARN << "disconnected, give up writing\n";
        return;
    }

    //if no thing in output queue, try writing directly
    if(!mChannel->isWriting() && mOutputBuffer.readableBytes() == 0)
    {
        nwrote = sockets::write(mChannel->fd(), message, len);
        if(nwrote > 0)
        {
            remaining = len - nwrote;
            if(remaining == 0 && mWriteCompleteCallback)
            {
                mLoop->queueInLoop(std::bind(mWriteCompleteCallback, shared_from_this()));
            }
        }
        else //nwrote < 0
        {
            nwrote = 0;
            if(errno != EWOULDBLOCK)
            {
                LOG_SYSERR << "CTcpConnection::sendInLoop\n";
                if(errno == EPIPE || errno == ECONNRESET)
                {
                    faultError = true;
                }
            }
        }
    }

    if(!faultError && remaining > 0)
    {
        size_t oldLen = mOutputBuffer.readableBytes();
        if(oldLen + remaining >= mHighWaterMark && oldLen < mHighWaterMark && mHighWaterMarkCallback)
        {
            mLoop->queueInLoop(std::bind(mHighWaterMarkCallback, shared_from_this(), oldLen + remaining));
        }
        mOutputBuffer.append(static_cast<const char*>(message) + nwrote, remaining);
        if(!mChannel->isWriting())
        {
            mChannel->enableWriting();
        }
    }
}

void CTcpConnection::shutdown()
{
    if(mState == kConnected)
    {
        setState(kDisconnecting);
        mLoop->runInLoop(std::bind(&CTcpConnection::shutdownInLoop, this));
    }
}

void CTcpConnection::shutdownInLoop()
{
    mLoop->assertInLoopThread();
    if(!mChannel->isWriting())
    {
        mSocket->shutdownWrite();
    }
}

void CTcpConnection::forceClose()
{
    if(mState == kConnected || mState == kConnecting)
    {
        setState(kDisconnecting);
        mLoop->queueInLoop(std::bind(&CTcpConnection::forceCloseInLoop, shared_from_this()));
    }
}

void CTcpConnection::forCloseWithDelay(double seconds)
{
    if(mState == kConnected || mState == kConnecting)
    {
        setState(kDisconnecting);
        mLoop->runAfter(seconds, makeWeakCallback(shared_from_this(), &CTcpConnection::forceClose));
    }    
}

void CTcpConnection::forceCloseInLoop()
{
    mLoop->assertInLoopThread();
    if(mState == kConnected || mState == kDisconnecting)
    {
        handleClose();
    }
}

const char* CTcpConnection::stateToString() const
{
    switch (mState)
    {
    case kDisconnected:
        return "kDisconnected";
    case kConnecting:
        return "kConnecting";
    case kConnected:
        return "kConnected";
    case kDisconnecting:
        return "kDisconnecting";
    default:
        return "unknown state";
    }
}

void CTcpConnection::setTcpNoDelay(bool on)
{
    mSocket->setTcpNoDelay(on);
}

void CTcpConnection::startRead()
{
    mLoop->runInLoop(std::bind(&CTcpConnection::startReadInLoop, this));
}

void CTcpConnection::startReadInLoop()
{
    mLoop->assertInLoopThread();
    if(!mReading || !mChannel->isReading())
    {
        mChannel->enableReading();
        mReading = true;
    }
}

void CTcpConnection::stopRead()
{
    mLoop->runInLoop(std::bind(&CTcpConnection::stopReadInLoop, this));
}

void CTcpConnection::stopReadInLoop()
{
    mLoop->assertInLoopThread();
    if(mReading || mChannel->isReading())
    {
        mChannel->disableReading();
        mReading = false;
    }
}

void CTcpConnection::connectEstablished()
{
    mLoop->assertInLoopThread();
    setState(kConnected);
    mChannel->tie(shared_from_this());
    mChannel->enableReading();

    mConnectionCallback(shared_from_this());
}

void CTcpConnection::connectDestroyed()
{
    mLoop->assertInLoopThread();
    if (mState == kConnected)
    {
        setState(kDisconnected);
        mChannel->disableAll();

        mConnectionCallback(shared_from_this());
    }
    mChannel->remove();
}

void CTcpConnection::handleRead(CTimestamp receiveTime)
{
    mLoop->assertInLoopThread();
    int savedErrno = 0;
    ssize_t n = mInputBuffer.readFd(mChannel->fd(), &savedErrno);
    if (n > 0)
    {
        mMessageCallback(shared_from_this(), &mInputBuffer, receiveTime);
    }
    else if (n == 0)
    {
        handleClose();
    }
    else
    {
        errno = savedErrno;
        LOG_SYSERR << "TcpConnection::handleRead\n";
        handleError();
    }
}

void CTcpConnection::handleWrite()
{
    mLoop->assertInLoopThread();
    if (mChannel->isWriting())
    {
        ssize_t n = sockets::write(mChannel->fd(), mOutputBuffer.peek(), mOutputBuffer.readableBytes());
        if (n > 0)
        {
            mOutputBuffer.retrieve(n);
            if (mOutputBuffer.readableBytes() == 0)
            {
                mChannel->disableWriting();
                if (mWriteCompleteCallback)
                {
                    mLoop->queueInLoop(std::bind(mWriteCompleteCallback, shared_from_this()));
                }
                if (mState == kDisconnecting)
                {
                    shutdownInLoop();
                }
            }
        }
        else
        {
            LOG_SYSFATAL << "TcpConnection::handleWrite";
            // if (state_ == kDisconnecting)
            // {
            //   shutdownInLoop();
            // }
        }
    }
    else
    {
        LOG_TRACE << "Connection fd = " << mChannel->fd()
                    << " is down, no more writing";
    }
}

void CTcpConnection::handleClose()
{
    mLoop->assertInLoopThread();
    LOG_TRACE << "fd = " << mChannel->fd() << " state = " << stateToString();
    // we don't close fd, leave it to dtor, so we can find leaks easily.
    setState(kDisconnected);
    mChannel->disableAll();

    CTcpConnectionPtr guardThis(shared_from_this());
    mConnectionCallback(guardThis);
    // must be the last line
    mCloseCallback(guardThis);
}

void CTcpConnection::handleError()
{
    int err = sockets::getSocketError(mChannel->fd());
    LOG_ERROR << "TcpConnection::handleError [" << mName
            << "] - SO_ERROR = " << err << " " << strerror_tl(err);
}







}