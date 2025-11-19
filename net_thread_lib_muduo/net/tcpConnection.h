#ifndef __NET_TCP_CONNECTION_H__
#define __NET_TCP_CONNECTION_H__

#include "timestamp.h"
#include "stringPiece.h"
#include "noncopyable.h"
#include "inetAddress.h"
#include "callback.h"
#include "buffer.h"
#include "socket.h"
#include <memory>

namespace net
{

class CChannel;
class CSocket;
class CEventLoop;

class CTcpConnection: muduo::noncopyable, public std::enable_shared_from_this<CTcpConnection>
{
public:
    CTcpConnection(CEventLoop *loop, std::string &name, int sockfd, const CInetAddress &localAddr, const CInetAddress &peerAddr);
    ~CTcpConnection();

    CEventLoop *getLoop()const {return mLoop;}
    const std::string name()const {return mName;}
    const CInetAddress& localAddress()const {return mLocalAddr;}
    const CInetAddress& peerAddress()const {return mPeerAddr;}
    bool connected()const {return mState == kConnected;}
    bool disconnected()const{return mState == kDisconnected;}
    bool getTcpInfo(::tcp_info*)const;
    std::string getTcpInfoString()const;
    void send(const void *msg, size_t len);
    void send(const CStringPiece &message);
    void send(CBuffer *message);
    void shutdown();
    void forceClose();
    void forCloseWithDelay(double seconds);
    void setTcpNoDelay(bool on);
    void startRead();
    void stopRead();
    bool isReading()const {return mReading;}
    //void setContext(boost:any &context) {mContext = contex;}
    //const boost::any getContext()const {return mContext;}
    //boost::any *getMutableContext() {return mContext;}
    void setConnectionCallback(const ConnectionCallback& cb)
    {
        mConnectionCallback = cb;
    }
    void setMessageCallback(const MessageCallback cb)
    {
        mMessageCallback = cb;
    }
    void setWriteCompleteCallback(const WriteCompleteCallback cb)
    {
        mWriteCompleteCallback = cb;
    }
    void setHighWaterMarkCallback(const HighWaterMarkCallback cb)
    {
        mHighWaterMarkCallback = cb;
    }

    void setCloseCallback(const CloseCallback& cb)
    {
        mCloseCallback = cb;
    }

    CBuffer* inputBuffer()
    {
        return &mInputBuffer;
    }

    CBuffer* outputBuffer()
    {
        return &mOutputBuffer;
    }

    void connectEstablished();
    void connectDestroyed();

private:
    enum stateE
    {
        kDisconnected,
        kConnecting,
        kConnected,
        kDisconnecting
    };

    void handleRead(CTimestamp receiveTime);
    void handleWrite();
    void handleClose();
    void handleError();
    void sendInLoop(const CStringPiece &message);
    void sendInLoop(const void *message, size_t len);
    void shutdownInLoop();
    void forceCloseInLoop();
    void setState(stateE s) {mState = s;}
    const char *stateToString()const;
    void startReadInLoop();
    void stopReadInLoop();

    CEventLoop *mLoop;
    const std::string mName;
    stateE mState;
    bool mReading;
    std::unique_ptr<CSocket> mSocket;
    std::unique_ptr<CChannel> mChannel;
    const CInetAddress mLocalAddr;
    const CInetAddress mPeerAddr;
    ConnectionCallback mConnectionCallback;
    MessageCallback mMessageCallback;
    WriteCompleteCallback mWriteCompleteCallback;
    HighWaterMarkCallback mHighWaterMarkCallback;
    CloseCallback mCloseCallback;
    size_t mHighWaterMark;
    CBuffer mInputBuffer;
    CBuffer mOutputBuffer;
    //boost::any mContext;
};















}
#endif