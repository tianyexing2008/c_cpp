#ifndef __LOGSTREAM_H__
#define __LOGSTREAM_H__

#include <string>
#include <string.h> // for memcpy()
#include <boost/noncopyable.hpp>

namespace detail
{

const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000*1000;

///固定缓存区类
template<int SIZE>
class CFixedBuffer: boost::noncopyable
{
public:
    CFixedBuffer(): mCur(mData)
    {
        setCookie(cookieStart);
    }

    ~CFixedBuffer()
    {
        setCookie(cookieEnd);
    }

    void append(const char* /*restrict*/ buf, size_t len)
    {
        // FIXME: append partially
        if (static_cast<size_t>(avail()) > len)
        {
            memcpy(mCur, buf, len);
            mCur += len;
        }
    }

    const char* data() const { return mData; }
    int length() const { return static_cast<int>(mCur - mData); }

    // write to mData directly
    char* current() { return mCur; }

    int avail() const { return static_cast<int>(end() - mCur); }

    void add(size_t len) { mCur += len; }

    void reset() { mCur = mData; }

    void bzero() { ::bzero(mData, sizeof mData); }

    // for used by GDB
    const char* debugString();

    void setCookie(void (*cookie)()) { mCookie = cookie; }

    // for used by unit test
    std::string asString() const { return std::string(mData, length()); }
private:
    const char* end() const { return mData + sizeof(mData); }

    // Must be outline function for cookies.
    static void cookieStart();
    static void cookieEnd();

    void (*mCookie)();

    char mData[SIZE];

    char* mCur;
};

};

class CLogStream: public boost::noncopyable
{
    typedef CLogStream self;
public:
    typedef detail::CFixedBuffer<detail::kSmallBuffer> Buffer;

    self& operator<<(bool v)
    {
        mBuffer.append(v?"1":"0", 1);
        return *this;
    }

    self& operator<<(short v);
    self& operator<<(unsigned short v);
    self& operator<<(int v);
    self& operator<<(unsigned int v);
    self& operator<<(long v);
    self& operator<<(unsigned long v);
    self& operator<<(long long v);
    self& operator<<(unsigned long long v);

    self& operator<<(const void*);

    self& operator<<(float v)
    {
        *this << static_cast<double>(v);
        return *this;
    }

    self& operator<<(double);

    self& operator<<(char v)
    {
        mBuffer.append(&v, 1);
        return *this;
    }

    self& operator<<(const char* v)
    {
        mBuffer.append(v, strlen(v));
        return *this;
    }

    self& operator<<(const std::string& v)
    { 
        mBuffer.append(v.c_str(), v.size());
        return *this;
    }

    void append(const char *data, int len)
    {
        mBuffer.append(data, len);
    }

    const Buffer& buffer() const { return mBuffer; }
    void resetBuffer() { mBuffer.reset(); }
private:
    void staticCheck();

    template<typename T>
    void formatInteger(T);

    Buffer mBuffer;

    static const int kMaxNumericSize = 32;
};

class Fmt // : boost::noncopyable
{
public:
    template<typename T>
    Fmt(const char* fmt, T val);

    const char* data() const { return mBuf; }
    int length() const { return mLength; }

private:
    char mBuf[32];
    int mLength;
};

inline CLogStream& operator<<(CLogStream& s, const Fmt& fmt)
{
    s.append(fmt.data(), fmt.length());
    return s;
}

#endif
