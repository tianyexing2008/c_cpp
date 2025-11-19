#ifndef __NET_INETADDRESS_H__
#define __NET_INETADDRESS_H__

#include "Types.h"
#include "stringPiece.h"
#include <netinet/in.h>

namespace net
{

namespace sockets
{
const struct sockaddr *sockaddr_cast(const struct sockaddr_in6 *addr);
}


class CInetAddress
{
public:
    explicit CInetAddress(uint16_t port = 0, bool loopbackonly = false, bool ipv6 = false);
    CInetAddress(CStringArg ip, uint16_t port, bool ipv6 = false);
    explicit CInetAddress(const struct sockaddr_in &addr): mAddr(addr)
    {

    }
    explicit CInetAddress(const struct sockaddr_in6 &addr): mAddr6(addr)
    {

    }

    sa_family_t family()const
    {
        return mAddr.sin_family;
    }

    std::string toIP()const;
    std::string toIpPort()const;
    uint16_t port()const;

    const struct sockaddr *getSockAddr()const
    {
        return sockets::sockaddr_cast(&mAddr6);
    }

    void setSockAddrInet6(const struct sockaddr_in6 &addr6)
    {
        mAddr6 = addr6;
    }

    uint32_t ipv4NetEndian()const;
    uint16_t portNetEndian()const
    {
        return mAddr.sin_port;
    }

    void setScopeID(uint32_t scope_id);

    //resolve hostname to ip address
    static bool resolve(CStringArg hostname, CInetAddress *result);

private:
    union 
    {
        struct sockaddr_in mAddr;
        struct sockaddr_in6 mAddr6;
    };
};

}

#endif