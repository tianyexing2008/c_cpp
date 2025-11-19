
#include "callback.h"
#include "eventLoop.h"
#include "tcpServer.h"
#include "logging.h"
#include "tcpConnection.h"

using namespace net;

void onMessage(const CTcpConnectionPtr& conn,
               CBuffer* buf,
               CTimestamp receiveTime)
{
    if(buf->findCRLF())
    {
        conn->send("No such user\n");
        conn->shutdown();
    }
}

int main()
{
    CEventLoop loop;
    CTcpServer tcpServer(&loop, CInetAddress(1079), "Finger");
    tcpServer.setMessageCallback(onMessage);
    tcpServer.start();
    loop.loop();

    return 0;
}