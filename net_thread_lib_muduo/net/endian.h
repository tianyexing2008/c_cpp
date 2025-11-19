
#ifndef __NET_ENDIAN_H__
#define __NET_ENDIAN_H__

#include <stdint.h>
#include <cstdint>
//#include <algorithm>

// 利用编译器内置宏判断字节序
#if defined(__LITTLE_ENDIAN__) || defined(_LITTLE_ENDIAN) || \
    defined(__LITTLE_ENDIAN) || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
constexpr bool is_little_endian = true;
constexpr bool is_big_endian = false;
#elif defined(__BIG_ENDIAN__) || defined(_BIG_ENDIAN) || \
      defined(__BIG_ENDIAN) || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
constexpr bool is_little_endian = false;
constexpr bool is_big_endian = true;
#else
#error "无法检测字节序，请手动定义 is_little_endian"
#endif

namespace net
{

// 16位：主机序 -> 网络序（大端）
inline uint16_t htobe16_r(uint16_t host16) 
{
    if (is_little_endian) 
    {
        // 小端系统：交换两个字节
        return (host16 << 8) | (host16 >> 8);
    }
    return host16; // 大端系统直接返回
}

// 16位：网络序（大端）-> 主机序
inline uint16_t be16toh_r(uint16_t be16) 
{
    return htobe16_r(be16); // 转换是双向的
}

// 32位：主机序 -> 网络序（大端）
inline uint32_t htobe32_r(uint32_t host32) 
{
    if (is_little_endian) 
    {
        // 小端系统：反转4字节顺序
        return ((host32 & 0x000000FF) << 24) |
               ((host32 & 0x0000FF00) << 8)  |
               ((host32 & 0x00FF0000) >> 8)  |
               ((host32 & 0xFF000000) >> 24);
    }
    return host32; // 大端系统直接返回
}

// 32位：网络序（大端）-> 主机序
inline uint32_t be32toh_r(uint32_t be32) 
{
    return htobe32_r(be32); // 转换是双向的
}

// 64位：主机序 -> 网络序（大端）
inline uint64_t htobe64_r(uint64_t host64) 
{
    if (is_little_endian) {
        // 小端系统：反转8字节顺序
        uint8_t* bytes = reinterpret_cast<uint8_t*>(&host64);
        //std::reverse(bytes, bytes + 8);
        // 手动交换字节（首尾对称交换）
        for (int i = 0; i < 4; ++i) {  // 8字节只需交换4次（0<->7, 1<->6, 2<->5, 3<->4）
            uint8_t temp = bytes[i];
            bytes[i] = bytes[7 - i];   // 第i个字节与第7-i个字节交换
            bytes[7 - i] = temp;
        }
    }
    return host64; // 大端系统直接返回
}

// 64位：网络序（大端）-> 主机序
inline uint64_t be64toh_r(uint64_t be64) 
{
    return htobe64_r(be64); // 转换是双向的
}

namespace sockets
{

inline uint64_t hostToNetwork64(uint64_t host64)
{
    return htobe64_r(host64);
}

inline uint32_t hostToNetwork32(uint32_t host32)
{
    return htobe32_r(host32);
}

inline uint16_t hostToNetwork16(uint16_t host16)
{
    return htobe16_r(host16);
}

inline uint64_t networkToHost64(uint64_t net64)
{
    return be64toh_r(net64);
}

inline uint32_t networkToHost32(uint32_t net32)
{
    return be32toh_r(net32);
}

inline uint16_t networkToHost16(uint16_t net16)
{
    return be16toh_r(net16);
}

}
}
#endif