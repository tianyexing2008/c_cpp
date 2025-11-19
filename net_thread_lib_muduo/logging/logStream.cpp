#include "logStream.h"
#include <algorithm>
#include <inttypes.h>
#include <assert.h>

const char digits[] = "9876543210123456789";
const char* zero = digits + 9;
const char digitsHex[] = "0123456789ABCDEF";

//efficient integer to string conversions
template<typename T>
size_t convert(char buf[], T value)
{
    T i = value;
    char *p = buf;
    
    do
    {
        int lsd = static_cast<int>(i % 10);
        i /= 10;
        *p++ = zero[lsd];
    }while(i != 0);

    if(value < 0)
    {
        *p++ = '-';
    }
    *p = '\0';

    std::reverse(buf, p);
    return p - buf;
}

size_t convertHex(char buf[], uintptr_t value)
{
    uintptr_t i = value;
    char *p = buf;
    
    do
    {
        int lsd = static_cast<int>(i % 16);
        i /= 16;
        *p++ = digitsHex[lsd];
    } while (i != 0);
    *p = '\0';

    std::reverse(buf, p);
    return p - buf;
}

//显式实例化，会在当前文件里生成具体参数的代码
template class CFixedBuffer<gSmallBuffer>;
template class CFixedBuffer<gLargeBuffer>;


std::string formatSI(int64_t s)
{
    double n = static_cast<double>(s);
    char buf[64] = {0};
    if (s < 1000)
    {   
        snprintf(buf, sizeof(buf), "%" PRId64, s);
    }
    else if (s < 9995)
    {
        snprintf(buf, sizeof(buf), "%.2fk", n/1e3);
    }
    else if (s < 99950)
    {
        snprintf(buf, sizeof(buf), "%.1fk", n/1e3);
    }
    else if (s < 999500)
    {
        snprintf(buf, sizeof(buf), "%.0fk", n/1e3);
    }
    else if (s < 9995000)
    {
        snprintf(buf, sizeof(buf), "%.2fM", n/1e6);
    }
    else if (s < 99950000)
    {
        snprintf(buf, sizeof(buf), "%.1fM", n/1e6);
    }
    else if (s < 999500000)
    {
        snprintf(buf, sizeof(buf), "%.0fM", n/1e6);
    }
    else if (s < 9995000000)
    {
        snprintf(buf, sizeof(buf), "%.2fG", n/1e9);
    }
    else if (s < 99950000000)
    {
        snprintf(buf, sizeof(buf), "%.1fG", n/1e9);
    }
    else if (s < 999500000000)
    {
        snprintf(buf, sizeof(buf), "%.0fG", n/1e9);
    }
    else if (s < 9995000000000)
    {
        snprintf(buf, sizeof(buf), "%.2fT", n/1e12);
    }
    else if (s < 99950000000000)
    {
        snprintf(buf, sizeof(buf), "%.1fT", n/1e12);
    }
    else if (s < 999500000000000)
    {
        snprintf(buf, sizeof(buf), "%.0fT", n/1e12);
    }
    else if (s < 9995000000000000)
    {
        snprintf(buf, sizeof(buf), "%.2fP", n/1e15);
    }
    else if (s < 99950000000000000)
    {
        snprintf(buf, sizeof(buf), "%.1fP", n/1e15);
    }
    else if (s < 999500000000000000)
    {
        snprintf(buf, sizeof(buf), "%.0fP", n/1e15);
    }
    else
    {
        snprintf(buf, sizeof(buf), "%.2fE", n/1e18);
    }
    return buf;
}

template<int SIZE>
void CFixedBuffer<SIZE>::cookieStart()
{

}

template<int SIZE>
void CFixedBuffer<SIZE>::cookieEnd()
{

}

void CLogStream::staticCheck()
{
    static_assert(mMaxNumericSize - 10 > std::numeric_limits<double>::digits10,
                "kMaxNumericSize is large enough");
    static_assert(mMaxNumericSize - 10 > std::numeric_limits<long double>::digits10,
                "kMaxNumericSize is large enough");
    static_assert(mMaxNumericSize - 10 > std::numeric_limits<long>::digits10,
                "kMaxNumericSize is large enough");
    static_assert(mMaxNumericSize - 10 > std::numeric_limits<long long>::digits10,
                "kMaxNumericSize is large enough");
}

//数值转换成字符串
template<typename T>
void CLogStream::formatInteger(T v)
{
    if(mBuffer.avail() >= mMaxNumericSize)
    {
        size_t len = convert(mBuffer.current(), v);
        mBuffer.add(len);
    }
}

CLogStream& CLogStream::operator<<(short v)
{
    *this << static_cast<int>(v);
    return *this;
}

CLogStream& CLogStream::operator<<(unsigned short v)
{
    *this << static_cast<unsigned int>(v);
    return *this;
}

CLogStream& CLogStream::operator<<(int v)
{
    formatInteger(v);
    return *this;
}

CLogStream& CLogStream::operator<<(unsigned int v)
{
    formatInteger(v);
    return *this;
}

CLogStream& CLogStream::operator<<(long v)
{
    formatInteger(v);
    return *this;
}

CLogStream& CLogStream::operator<<(unsigned long v)
{
    formatInteger(v);
    return *this;
}

CLogStream& CLogStream::operator<<(long long v)
{
    formatInteger(v);
    return *this;
}

CLogStream& CLogStream::operator<<(unsigned long long v)
{
    formatInteger(v);
    return *this;
}

CLogStream& CLogStream::operator<<(const void* p)
{
    uintptr_t v = reinterpret_cast<uintptr_t>(p);
    if(mBuffer.avail() >= mMaxNumericSize)
    {
        char *buf = mBuffer.current();
        buf[0] = '0';
        buf[1] = 'x';
        size_t len = convertHex(buf + 2, v);
        mBuffer.add(len);
    }
    return *this;
}

CLogStream& CLogStream::operator<<(double v)
{
    if(mBuffer.avail() >= mMaxNumericSize)
    {
        int len = snprintf(mBuffer.current(), mMaxNumericSize, "%.12g", v);
        mBuffer.add(len);
    }
    return *this;
}

template<typename T>
CFmt::CFmt(const char *fmt, T val)
{
    static_assert(std::is_arithmetic<T>::value == true, "Must be arithmetic type"); //必须是算术类型
    mLength = snprintf(mBuf, sizeof(mBuf), fmt, val);
    assert(static_cast<size_t>(mLength) < sizeof(mBuf));
}

//explicit instantiation
template CFmt::CFmt(const char* fmt, char);
template CFmt::CFmt(const char* fmt, short);
template CFmt::CFmt(const char* fmt, unsigned short);
template CFmt::CFmt(const char* fmt, int);
template CFmt::CFmt(const char* fmt, unsigned int);
template CFmt::CFmt(const char* fmt, long);
template CFmt::CFmt(const char* fmt, unsigned long);
template CFmt::CFmt(const char* fmt, long long);
template CFmt::CFmt(const char* fmt, unsigned long long);
template CFmt::CFmt(const char* fmt, float);
template CFmt::CFmt(const char* fmt, double);
