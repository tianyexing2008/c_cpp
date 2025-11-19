#ifndef __FIXED_BUFFER_H__
#define __FIXED_BUFFER_H__

#include "Types.h"
#include "noncopyable.h"
#include "stringPiece.h"
const int gSmallBuffer = 4000;
const int gLargeBuffer = 4000*1000;

template <int SIZE>
class CFixedBuffer: public muduo::noncopyable
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

    void append(const char *buf, size_t len)
    {
        if(static_cast<size_t>(avail()) > len)
        {
            memcpy(mCur, buf, len);
            mCur += len;
        }
    }

    const char *data()const {return mData;}
    int lenght()const
    {
        return static_cast<int>(mCur - mData);
    }
    char *current(){return mCur;};
    int avail()const {return static_cast<int>(end() - mCur);}
    void add(size_t len){mCur += len;}
    void reset(){mCur = mData;}
    void bzero(){memZero(mData, sizeof(mData));}
    const char *debugString();
    void setCookie(void (*cookie)()){cookie_ = cookie;}
    std::string toString(){return std::string(mData, lenght());}
    CStringPiece toStringPiece()const{return CStringPiece(mData, lenght());}
private:
    const char *end()const 
    {
        return mData + sizeof(mData);
    }   
static void cookieStart();
static void cookieEnd();
void (*cookie_)();

char mData[SIZE];
char *mCur;
};
#endif