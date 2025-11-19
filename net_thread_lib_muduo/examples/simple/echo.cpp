#include "echo.h"
#include "logging.h"
#include "eventLoop.h"
#include "inetAddress.h"
#include "tcpConnection.h"

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using namespace net;

CEchoServer::CEchoServer(net::CEventLoop *loop, const net::CInetAddress &listenAddr)
:mServer(loop, listenAddr, "EchoServer")
{
    mServer.setConnectionCallback(std::bind(&CEchoServer::onConnection, this, _1));
    mServer.setMessageCallback(std::bind(&CEchoServer::onMessage, this, _1, _2, _3));
}

void CEchoServer::start()
{
    mServer.start();
}

void CEchoServer::onConnection(const net::CTcpConnectionPtr &conn)
{
    LOG_INFO << "EchoServer - " << conn->peerAddress().toIpPort() << " -> "
            << conn->localAddress().toIpPort() << " is "
            << (conn->connected() ? "UP" : "DOWN");
}

void CEchoServer::onMessage(const net::CTcpConnectionPtr &conn, net::CBuffer *buf, CTimestamp time)
{
    std::string msg(buf->retrieveAllAsString());
    LOG_INFO << conn->name() << " echo " << msg.size() << " bytes, "
            << "data received at " << time.toString();
    conn->send(msg);
}

int main()
{
    CInetAddress local("10.91.90.176", 3999);
    CEventLoop loop;
    CEchoServer echoServer(&loop, local);
    echoServer.start();
    
    return 0;
}