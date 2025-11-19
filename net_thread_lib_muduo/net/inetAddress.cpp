#include "endian.h"
#include "inetAddress.h"
#include "socketsOps.h"
#include "logging.h"
#include <assert.h>
#include <netdb.h>
#include <netinet/in.h>

namespace net
{

CInetAddress::CInetAddress(uint16_t port, bool loopbackonly, bool ipv6)
{
    if(ipv6)
    {
        memZero(&mAddr6, sizeof(mAddr6));
        mAddr6.sin6_family = AF_INET6;
        in6_addr ip = loopbackonly ? in6addr_loopback : in6addr_any;
        mAddr6.sin6_addr = ip;
        mAddr6.sin6_port = sockets::hostToNetwork16(port);
    }
    else
    {
        memZero(&mAddr, sizeof(mAddr));
        mAddr.sin_family = AF_INET;
        in_addr_t ip = loopbackonly ? INADDR_LOOPBACK : INADDR_ANY;
        mAddr.sin_addr.s_addr = sockets::hostToNetwork32(ip);
        mAddr.sin_port = sockets::hostToNetwork16(port);
    }


}

CInetAddress::CInetAddress(CStringArg ip, uint16_t port, bool ipv6)
{
    if(ipv6 || strchr(ip.c_str(), ':'))
    {
        memZero(&mAddr6, sizeof(mAddr6));
        sockets::fromIpPort(ip.c_str(), port, &mAddr6);
    }
    else
    {
        memZero(&mAddr, sizeof(mAddr));
        sockets::fromIpPort(ip.c_str(), port, &mAddr);
    }
}

std::string CInetAddress::toIP()const
{
    char buf[64] = {0};
    sockets::toIp(buf, sizeof(buf), getSockAddr());
    return buf;
}

std::string CInetAddress::toIpPort()const
{
    char buf[64] = {0};
    sockets::toIpPort(buf, sizeof(buf), getSockAddr());
    return buf;    
}

uint32_t CInetAddress::ipv4NetEndian()const
{
    return mAddr.sin_addr.s_addr;
}

uint16_t CInetAddress::port()const
{
    return sockets::networkToHost16(portNetEndian());
}

static __thread char t_resolveBuffer[64 * 1024];

bool CInetAddress::resolve(CStringArg hostname, CInetAddress *result)
{
    struct hostent hent;
    struct hostent* he = NULL;
    int herrno = 0;
    memZero(&hent, sizeof(hent));

    int ret = gethostbyname_r(hostname.c_str(), &hent, t_resolveBuffer, sizeof t_resolveBuffer, &he, &herrno);
    if (ret == 0 && he != NULL)
    {
        result->mAddr.sin_addr = *reinterpret_cast<struct in_addr*>(he->h_addr);
        return true;
    }
    else
    {
        if (ret)
        {
            LOG_SYSERR << "InetAddress::resolve" << "\n";
        }
        return false;
    }
}

void CInetAddress::setScopeID(uint32_t scope_id)
{
    if (family() == AF_INET6)
    {
        mAddr6.sin6_scope_id = scope_id;
    }
}

}