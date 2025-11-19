#ifndef __NET_CHANNEL_H__
#define __NET_CHANNEL_H__

#include "noncopyable.h"
#include "timestamp.h"

#include <functional>
#include <memory>

namespace net
{

class CEventLoop;

class CChannel: muduo::noncopyable
{
public:
    typedef std::function<void()> EventCallback;
    typedef std::function<void(CTimestamp)> ReadEventCallback;

    CChannel(CEventLoop *loop, int fd);
    ~CChannel();

    void handleEvent(CTimestamp receiveTime);
    void setReadCallback(ReadEventCallback cb)
    {
        mReadCallback = cb;
    }

    void setWriteCallback(EventCallback cb)
    {
        mWriteCallback = cb;
    }

    void setCloseCallback(EventCallback cb)
    {
        mCloseCallback = cb;
    }

    void setErrorCallback(EventCallback cb)
    {
        mErrorCallback = cb;
    }

    //Tie this channel to the owner objct managed by shared_ptr
    //prevent  the owner object being destroyed in handleEvent
    void tie(const std::shared_ptr<void>&);

    int fd()const
    {
        return mFd;
    }

    int events()const
    {
        return mEvents;
    }
    void setRevents(int revt)
    {
        mRevents = revt;
    }

    bool isNoneEvent()const
    {
        return mEvents == mNoneEvent;
    }
    
    void enableReading()
    {
        mEvents |= mReadEvent;
        update();
    }
    
    void disableReading()
    {
        mEvents &= ~mReadEvent;
        update();
    }

    void enableWriting()
    {
        mEvents |= mWriteEvent;
        update();
    }

    void disableWriting()
    {
        mEvents &= ~mWriteEvent;
        update();
    }

    void disableAll()
    {
        mEvents = mNoneEvent;
        update();
    }

    bool isWriting()const
    {
        return mEvents & mWriteEvent;
    }

    bool isReading()const
    {
        return mEvents & mReadEvent;
    }

    int index()
    {
        return mIndex;
    }

    void setIndex(int idx)
    {
        mIndex = idx;
    }

    std::string reventsToString()const;
    std::string eventsToString()const;

    void doNotLogHup(){mLogHup = false;}
    CEventLoop *ownerLoop(){return mLoop;}
    void remove();
    
private:
    static std::string eventsToString(int fd, int ev);
    void update();
    void handleEventWithGuard(CTimestamp receiveTime);
    static const int mNoneEvent;
    static const int mReadEvent;
    static const int mWriteEvent;
    CEventLoop *mLoop;
    const int   mFd;
    int mEvents;
    int mRevents;//it's the received event
    int mIndex; //use by poller
    bool mLogHup;
    std::weak_ptr<void> mTie;
    bool mTied;
    bool mEventHandling;
    bool mAddToLoop;

    ReadEventCallback mReadCallback;
    EventCallback mWriteCallback;
    EventCallback mCloseCallback;
    EventCallback mErrorCallback;
};

}

#endif