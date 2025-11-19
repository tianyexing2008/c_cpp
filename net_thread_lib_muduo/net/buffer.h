#ifndef __NET_BUFFER_H__
#define __NET_BUFFER_H__

#include "stringPiece.h"
#include "endian.h"
#include "Types.h"

#include <algorithm>
#include <vector>

#include <assert.h>
#include <string.h>

namespace net
{

class CBuffer
{
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    explicit CBuffer(size_t initialSize = kInitialSize)
    :mBuffer(kCheapPrepend + kInitialSize)
    ,mReaderIndex(kCheapPrepend)
    ,mWriterIndex(kCheapPrepend)
    {
    }

    void swap(CBuffer &rhs)
    {
        mBuffer.swap(rhs.mBuffer);
        std::swap(mReaderIndex, rhs.mReaderIndex);
        std::swap(mWriterIndex, rhs.mWriterIndex);
    }

    //返回可读数据的长度
    size_t readableBytes()const
    {
        return mWriterIndex - mReaderIndex;
    }

    //返回可写区域的长度
    size_t writableBytes()const
    {
        return mBuffer.size() - mWriterIndex;
    }

    //返回预留区域的长度
    size_t prependableBytes()const
    {
        return mReaderIndex;
    }
    
    //返回可读区域的起始位置，供读取数据
    const char *peek()const
    {
        return begin() + mReaderIndex;
    }

    //
    const char *findCRLF()const
    {
        const char *crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF + 2);
        return crlf == beginWrite() ? NULL : crlf;
    }

    const char *findCRLF(const char *start)const
    {
        const char *crlf = std::search(start, beginWrite(), kCRLF, kCRLF + 2);
        return crlf == beginWrite() ? NULL : crlf;       
    }

    const char *findEOL()const
    {
        const void *eol = memchr(peek(), '\n', readableBytes());
        return static_cast<const char *>(eol);
    }

    //从可读区别提取 len 字节
    void retrieve(size_t len)
    {
        if(len < readableBytes())
        {
            mReaderIndex += len;
        }
        else 
        {
            retrieveAll();
        }
    }

    //提取数据直到 end 指针位置
    void retrieveUntil(const char *end)
    {
        retrieve(end - peek());
    }

    void retrieveInt64()
    {
        retrieve(sizeof(int64_t));
    }

    void retrieveInt32()
    {
        retrieve(sizeof(int32_t));
    }

    void retrieveInt16()
    {
        retrieve(sizeof(int16_t));
    }

    void retrieveInt8()
    {
        retrieve(sizeof(int8_t));
    } 

    //清空可读区域
    void retrieveAll()
    {
        mReaderIndex = kCheapPrepend;
        mWriterIndex = kCheapPrepend;
    }

    //
    std::string retrieveAllAsString()
    {
        return retrieveAsString(readableBytes());
    }

    std::string retrieveAsString(size_t len)
    {
        std::string result(peek(), len);
        retrieve(len);
        return result;
    }

    CStringPiece toStringPiece()const
    {
        return CStringPiece(peek(), static_cast<int>(readableBytes()));
    }

    void append(const CStringPiece& str)
    {
        append(str.data(), str.size());
    }

    void append(const char *data, size_t len)
    {
        ensureWritableBytes(len);
        std::copy(data, data + len, beginWrite());
        hasWritten(len);
    }

    void append(const void *data, size_t len)
    {
        append(static_cast<const char*>(data), len);
    }

    void ensureWritableBytes(size_t len)
    {
        if(writableBytes() < len)
        {
            makeSpace(len);
        }
    }

    char *beginWrite()
    {
        return begin() + mWriterIndex;
    }

    const char* beginWrite()const
    {
        return begin() + mWriterIndex;
    }

    void hasWritten(size_t len)
    {
        mWriterIndex += len;
    }

    void unwrite(size_t len)
    {
        mWriterIndex -= len;
    }

    void appendInt64(int64_t x)
    {
        int64_t be64 = sockets::hostToNetwork64(x);
        append(&be64, sizeof be64);
    }

    void appendInt32(int32_t x)
    {
        int32_t be32 = sockets::hostToNetwork32(x);
        append(&be32, sizeof be32);
    }

    void appendInt16(int16_t x)
    {
        int16_t be16 = sockets::hostToNetwork16(x);
        append(&be16, sizeof be16);
    }

    void appendInt8(int8_t x)
    {
        append(&x, sizeof x);
    }

    int64_t readInt64()
    {
        int64_t result = peekInt64();
        retrieveInt64();
        return result;
    }

    int32_t readInt32()
    {
        int32_t result = peekInt32();
        retrieveInt32();
        return result;
    }

    int16_t readInt16()
    {
        int16_t result = peekInt16();
        retrieveInt16();
        return result;
    }

    int8_t readInt8()
    {
        int8_t result = peekInt8();
        retrieveInt8();
        return result;
    }

    int64_t peekInt64() const
    {
        int64_t be64 = 0;
        ::memcpy(&be64, peek(), sizeof be64);
        return sockets::networkToHost64(be64);
    }
    int32_t peekInt32() const
    {
        int32_t be32 = 0;
        ::memcpy(&be32, peek(), sizeof be32);
        return sockets::networkToHost32(be32);
    }

    int16_t peekInt16() const
    {
        int16_t be16 = 0;
        ::memcpy(&be16, peek(), sizeof be16);
        return sockets::networkToHost16(be16);
    }

    int8_t peekInt8() const
    {
        int8_t x = *peek();
        return x;
    }

  void prependInt64(int64_t x)
  {
    int64_t be64 = sockets::hostToNetwork64(x);
    prepend(&be64, sizeof be64);
  }

    ///
    /// Prepend int32_t using network endian
    ///
    void prependInt32(int32_t x)
    {
        int32_t be32 = sockets::hostToNetwork32(x);
        prepend(&be32, sizeof be32);
    }

    void prependInt16(int16_t x)
    {
        int16_t be16 = sockets::hostToNetwork16(x);
        prepend(&be16, sizeof be16);
    }

    void prependInt8(int8_t x)
    {
        prepend(&x, sizeof x);
    }

    void prepend(const void* /*restrict*/ data, size_t len)
    {
        mReaderIndex -= len;
        const char* d = static_cast<const char*>(data);
        std::copy(d, d+len, begin() + mReaderIndex);
    }

    void shrink(size_t reserve)
    {
        // FIXME: use vector::shrink_to_fit() in C++ 11 if possible.
        CBuffer other;
        other.ensureWritableBytes(readableBytes() + reserve);
        other.append(toStringPiece());
        swap(other);
    }

    size_t internalCapacity() const
    {
        return mBuffer.capacity();
    }

    /// Read data directly into buffer.
    ///
    /// It may implement with readv(2)
    /// @return result of read(2), @c errno is saved
    ssize_t readFd(int fd, int* savedErrno);
private:
    char* begin()
    { 
        return &(*(mBuffer.begin()));
    }

    const char* begin() const
    { 
        return &(*(mBuffer.begin()));
    }

    void makeSpace(size_t len)
    {
        if (writableBytes() + prependableBytes() < len + kCheapPrepend)
        {
            // FIXME: move readable data
            mBuffer.resize(mWriterIndex+len);
        }
        else
        {
            // move readable data to the front, make space inside buffer
            size_t readable = readableBytes();
            std::copy(begin() + mReaderIndex,
                    begin() + mWriterIndex,
                    begin() + kCheapPrepend);
            mReaderIndex = kCheapPrepend;
            mWriterIndex = mReaderIndex + readable;
        }
    }
private:
    std::vector<char> mBuffer;
    size_t mReaderIndex;//可读数据的起始位置
    size_t mWriterIndex;//可写区域的起始位置

    static const char kCRLF[]; //只是声明
};
}
#endif