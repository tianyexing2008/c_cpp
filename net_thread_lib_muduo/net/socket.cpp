#include "socket.h"
#include "logging.h"
#include "inetAddress.h"
#include "socketsOps.h"
#include <netinet/in.h>
#include <stdio.h>  // snprintf
#include <netinet/tcp.h>

namespace net
{

CSocket::~CSocket()
{
    sockets::close(mSockFd);
}

bool CSocket::getTcpInfo(struct tcp_info* info)const
{
    socklen_t len = sizeof(*info);
    memZero(info, len);
    return ::getsockopt(mSockFd, SOL_TCP, TCP_INFO, info, &len) == 0;
}

bool CSocket::getTcpInfoString(char* buf, int len) const
{
    struct tcp_info tcpi;
    bool ok = getTcpInfo(&tcpi);
    if (ok)
    {
        snprintf(buf, len, "unrecovered=%u "
                    "rto=%u ato=%u snd_mss=%u rcv_mss=%u "
                    "lost=%u retrans=%u rtt=%u rttvar=%u "
                    "sshthresh=%u cwnd=%u total_retrans=%u",
                    tcpi.tcpi_retransmits,  // Number of unrecovered [RTO] timeouts
                    tcpi.tcpi_rto,          // Retransmit timeout in usec
                    tcpi.tcpi_ato,          // Predicted tick of soft clock in usec
                    tcpi.tcpi_snd_mss,
                    tcpi.tcpi_rcv_mss,
                    tcpi.tcpi_lost,         // Lost packets
                    tcpi.tcpi_retrans,      // Retransmitted packets out
                    tcpi.tcpi_rtt,          // Smoothed round trip time in usec
                    tcpi.tcpi_rttvar,       // Medium deviation
                    tcpi.tcpi_snd_ssthresh,
                    tcpi.tcpi_snd_cwnd,
                    tcpi.tcpi_total_retrans);  // Total retransmits for entire connection
    }
    return ok;
}

void CSocket::bindAddress(const CInetAddress& addr)
{
    sockets::bindOrDie(mSockFd, addr.getSockAddr());
}

void CSocket::listen()
{
    sockets::listenOrDie(mSockFd);
}

int CSocket::accept(CInetAddress* peeraddr)
{
    struct sockaddr_in6 addr;
    memZero(&addr, sizeof addr);
    int connfd = sockets::accept(mSockFd, &addr);
    if (connfd >= 0)
    {
        peeraddr->setSockAddrInet6(addr);
    }
    return connfd;
}

void CSocket::shutdownWrite()
{
    sockets::shutdownWrite(mSockFd);
}

void CSocket::setTcpNoDelay(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(mSockFd, IPPROTO_TCP, TCP_NODELAY, &optval, static_cast<socklen_t>(sizeof optval));
    // FIXME CHECK
}

void CSocket::setReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    int ret = ::setsockopt(mSockFd, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t>(sizeof optval));
    if(ret != 0)
    {
        LOG_WARN << "SO_REUSEADDR failed\n";
    }
    // FIXME CHECK
}

void CSocket::setReusePort(bool on)
{
#ifdef SO_REUSEPORT
    int optval = on ? 1 : 0;
    int ret = ::setsockopt(mSockFd, SOL_SOCKET, SO_REUSEPORT, &optval, static_cast<socklen_t>(sizeof optval));
    if (ret < 0 && on)
    {
        LOG_SYSERR << "SO_REUSEPORT failed.";
    }
#else
    if (on)
    {
        LOG_ERROR << "SO_REUSEPORT is not supported.";
    }
#endif
}

void CSocket::setKeepAlive(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(mSockFd, SOL_SOCKET, SO_KEEPALIVE, &optval, static_cast<socklen_t>(sizeof optval));
    // FIXME CHECK
}

}