#include "tcpServer.h"
#include "timestamp.h"

class CEchoServer
{
public:
    CEchoServer(net::CEventLoop *loop, const net::CInetAddress &listenAddr);
    void start();

private:
    void onConnection(const net::CTcpConnectionPtr &conn);
    void onMessage(const net::CTcpConnectionPtr &conn, net::CBuffer *buf, CTimestamp time);

    net::CTcpServer mServer;
};