#include <assert.h>
#include <algorithm>
#include "LogStream.h"

using namespace detail;

const char digits[] = "9876543210123456789";
const char digitsHex[] = "0123456789ABCDEF";
const char* zero = digits + 9;
// Efficient Integer to String Conversions, by Matthew Wilson.
template<typename T>
size_t convert(char buf[], T value)
{
    T i = value;
    char* p = buf;

    do
    {
        int lsd = static_cast<int>(i % 10);
        i /= 10;
        *p++ = zero[lsd];
    } while (i != 0);

    if (value < 0)
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
    char* p = buf;

    do
    {
        int lsd = i % 16;
        i /= 16;
        *p++ = digitsHex[lsd];
    } while (i != 0);

    *p = '\0';
    std::reverse(buf, p);

    return p - buf;
}

namespace detail
{

template<int SIZE>
const char* CFixedBuffer<SIZE>::debugString()
{
    *mCur = '\0';
    return mData;
}

template<int SIZE>
void CFixedBuffer<SIZE>::cookieStart()
{

}

template<int SIZE>
void CFixedBuffer<SIZE>::cookieEnd()
{

}

}

template class CFixedBuffer<detail::kSmallBuffer>;
template class CFixedBuffer<detail::kLargeBuffer>;

template<typename T>
void CLogStream::formatInteger(T v)
{
    if (mBuffer.avail() >= kMaxNumericSize)
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
    if (mBuffer.avail() >= kMaxNumericSize)
    {
        char* buf = mBuffer.current();
        buf[0] = '0';
        buf[1] = 'x';
        size_t len = convertHex(buf + 2, v);
        mBuffer.add(len + 2);
    }
    return *this;
}

// FIXME: replace this with Grisu3 by Florian Loitsch.
CLogStream& CLogStream::operator<<(double v)
{
    if (mBuffer.avail() >= kMaxNumericSize)
    {
        int len = snprintf(mBuffer.current(), kMaxNumericSize, "%.12g", v);
        mBuffer.add(len);
    }
    return *this;
}

template<typename T>
Fmt::Fmt(const char* fmt, T val)
{
//   BOOST_STATIC_ASSERT(boost::is_arithmetic<T>::value == true);

  mLength = snprintf(mBuf, sizeof(mBuf), fmt, val);
  assert(static_cast<size_t>(mLength) < sizeof(mBuf));
}

// Explicit instantiations
template Fmt::Fmt(const char* fmt, char);
template Fmt::Fmt(const char* fmt, short);
template Fmt::Fmt(const char* fmt, unsigned short);
template Fmt::Fmt(const char* fmt, int);
template Fmt::Fmt(const char* fmt, unsigned int);
template Fmt::Fmt(const char* fmt, long);
template Fmt::Fmt(const char* fmt, unsigned long);
template Fmt::Fmt(const char* fmt, long long);
template Fmt::Fmt(const char* fmt, unsigned long long);

template Fmt::Fmt(const char* fmt, float);
template Fmt::Fmt(const char* fmt, double);
