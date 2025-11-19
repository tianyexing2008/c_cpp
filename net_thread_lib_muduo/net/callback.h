#ifndef __NET_CALLBACKS_H__
#define __NET_CALLBACKS_H__

#include "Types.h"
#include "timestamp.h"

#include <functional>
#include <memory>

namespace net
{

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

template<typename T>
inline T* getPointer(const std::shared_ptr<T> &ptr)
{
    return ptr.get();
}

template<typename T>
inline T* getPointer(const std::unique_ptr<T> &ptr)
{
    return ptr.get();
}
template<typename To, typename From>
inline ::std::shared_ptr<To> down_pointer_cast(const ::std::shared_ptr<From>& f) 
{
    if (false)
    {
        implicit_cast<From*, To*>(0);
    }

#ifndef NDEBUG
#endif
    return ::std::static_pointer_cast<To>(f);
}

class CBuffer;
class CTcpConnection;
typedef std::shared_ptr<CTcpConnection> CTcpConnectionPtr;
typedef std::function<void()> TimerCallback;
typedef std::function<void (const CTcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void (const CTcpConnectionPtr&)> CloseCallback;
typedef std::function<void (const CTcpConnectionPtr&)> WriteCompleteCallback;
typedef std::function<void (const CTcpConnectionPtr&, size_t)> HighWaterMarkCallback;

// the data has been read to (buf, len)
typedef std::function<void (const CTcpConnectionPtr&, CBuffer*, CTimestamp)> MessageCallback;

void defaultConnectionCallback(const CTcpConnectionPtr& conn);
void defaultMessageCallback(const CTcpConnectionPtr& conn, CBuffer* buffer, CTimestamp receiveTime);

}
#endif