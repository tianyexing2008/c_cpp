#ifndef __LOG_STREAM_H__
#define __LOG_STREAM_H__

#include "fixedBuffer.h"

class CLogStream: public muduo::noncopyable
{
public:
    typedef CLogStream self;
    typedef CFixedBuffer<gSmallBuffer> Buffer;

    self& operator<<(bool v)
    {
        mBuffer.append(v ? "1" : "0", 1);
        return *this;
    }

    self& operator<<(short);
    self& operator<<(unsigned short);
    self& operator<<(int);
    self& operator<<(unsigned int);
    self& operator<<(long);
    self& operator<<(unsigned long);
    self& operator<<(long long);
    self& operator<<(unsigned long long);
    self& operator<<(const void*);
    self& operator<<(float v)
    {
        *this << static_cast<double>(v);
        return *this;
    }
    self& operator<<(double);

    self& operator<<(char ch)
    {
        mBuffer.append(&ch, 1);
        return *this;
    }

    self& operator<<(const char *str)
    {
        if(str)
        {
            mBuffer.append(str, strlen(str));
        }
        else
        {
            mBuffer.append("(null)", 6);
        }
        return *this;
    }
    self& operator<<(const unsigned char *str)
    {
        return operator<<(reinterpret_cast<const char*>(str));
    }

    self& operator<<(const std::string &str)
    {
        mBuffer.append(str.c_str(), str.size());
        return *this;
    }

    self& operator<<(const CStringPiece &str)
    {
        mBuffer.append(str.data(), str.size());
        return *this;
    }

    self& operator<<(const Buffer &str)
    {
        *this << str.toStringPiece();
    }

    void append(const char *data, int len)
    {
        mBuffer.append(data, len);
    }

    const Buffer& buffer()const {return mBuffer;}
    void resetBuffer(){mBuffer.reset();}
private:
    void staticCheck();
    template<typename T>
    void formatInteger(T);
private:
    Buffer mBuffer;
    static const int mMaxNumericSize = 48;
};

class CFmt
{
public:
    template<typename T>
    CFmt(const char *fmt, T val);

    const char *data()const{return mBuf;};
    int length()const{return mLength;}
private:
    char mBuf[32];
    int mLength;
};

inline CLogStream& operator<<(CLogStream& s, const CFmt &fmt)
{
    s.append(fmt.data(), fmt.length());
}

std::string formatSI(int64_t n);
std::string formatIEC(int64_t n);
#endif