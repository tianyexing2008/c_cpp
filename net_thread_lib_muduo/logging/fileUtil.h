#ifndef __FILEUTIL_H__
#define __FILEUTIL_H__

#include "noncopyable.h"
#include "stringPiece.h"

namespace FileUtil
{

class CReadSmallFile: muduo::noncopyable
{
public:
    CReadSmallFile(CStringArg fileName);
    ~CReadSmallFile();

    template<typename String>
    int readToString(int maxSize, String *content, int64_t *fileSize, int64_t *modifyTime, int64_t *createTime);
    int readToBuffer(int *size);
    const char *buffer()const{return mBuf;}
    static const int kBufferSize = 64 * 1024;
private:
    int mFd;
    int mErr;
    char mBuf[kBufferSize];
};

template<typename String>
int readFile(CStringArg fileName, int maxSize, String *content, int64_t *fileSize = nullptr, int64_t *modifyTime = nullptr, int64_t *createTime = nullptr)
{
    CReadSmallFile file(fileName);
    return file.readToString(maxSize, content, fileSize, modifyTime, createTime);
}

class CAppendFile: muduo::noncopyable
{
public:
    explicit CAppendFile(CStringArg fileName);
    ~CAppendFile();

    void append(const char *msg, size_t len);
    void flush();
    off_t writtenBytes()const {return mWrittenBytes;}
private:
    size_t write(const char *msg, size_t len);
    FILE *mFp;
    char mBuffer[CReadSmallFile::kBufferSize];
    off_t mWrittenBytes;
};
}
#endif