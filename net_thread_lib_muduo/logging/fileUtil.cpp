#include "fileUtil.h"
#include <assert.h>
#include <stdio.h>
#include <fcntl.h>
#include <thread.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

extern const char *strerror_tl(int saveError);

namespace FileUtil
{

CReadSmallFile::CReadSmallFile(CStringArg fileName): mFd(::open(fileName.c_str(), O_RDONLY | O_CLOEXEC)), mErr(0)
{
    mBuf[0] = '\0';
    if(mFd < 0)
    {
        mErr = errno;
    }
}

CReadSmallFile::~CReadSmallFile()
{
    if(mFd > 0)
    {
        ::close(mFd);
    }
}

template<typename String>
int CReadSmallFile::readToString(int maxSize, String *content, int64_t *fileSize, int64_t *modifyTime, int64_t *createTime)
{
    static_assert(sizeof(off_t) == 8, "_FILE_OFFSET_BITS = 8");
    assert(content != nullptr);
    int err = mErr;
    if(mFd > 0)
    {
        content->clear();
        if(fileSize)
        {
            struct stat statBuf;
            if(::fstat(mFd, &statBuf) == 0)
            {
                if(S_ISREG(statBuf.st_mode))
                {
                    *fileSize = statBuf.st_size;
                    content->reserve(static_cast<int>(std::min(static_cast<int64_t>(maxSize), *fileSize)));
                }
                else if(S_ISDIR(statBuf.st_mode))
                {
                    err = EISDIR;
                }

                if(modifyTime)
                {
                    *modifyTime = statBuf.st_mtime;
                }
                if(createTime)
                {
                    *createTime = statBuf.st_ctime;
                }
            }
            else 
            {
                err = errno;
            }
        }

        while(content->size() < maxSize)
        {
            size_t toRead = std::min(maxSize - content->size(), sizeof(mBuf));
            ssize_t n = ::read(mFd, mBuf, toRead);
            if(n > 0)
            {
                content->append(mBuf, n);
            }
            else 
            {
                if(n < 0)
                {
                    err = errno;
                }
                break;
            }
        }
    }
    return err;
}

int CReadSmallFile::readToBuffer(int *size)
{
    int err = mErr;
    if(mFd > 0)
    {
        ssize_t n = ::pread(mFd, mBuf, sizeof(mBuf) - 1, 0);
        if(n > 0)
        {
            if(size)
            {
                *size = static_cast<int>(n);
            }
            mBuf[n] = '\0';
        }
    }
    else
    {
        err = errno;
    }
    return err;
}

CAppendFile::CAppendFile(CStringArg fileName)
:mFp(::fopen(fileName.c_str(), "ae"))
,mWrittenBytes(0)
{
    assert(mFp);
    ::setbuffer(mFp, mBuffer, sizeof(mBuffer));
}

CAppendFile::~CAppendFile()
{
    ::fclose(mFp);
}

void CAppendFile::append(const char *msg, size_t len)
{
    size_t written = 0;

    while(written != len)
    {
        size_t remain = len - written;
        size_t n = write(msg + written, remain);
        if(n != remain)
        {
            int err = ferror(mFp);
            if(err)
            {
                fprintf(stderr, "CAppendFile::append() failed %s\n", strerror_tl(err));
                break;
            }
        }
        written += n;
    }

    mWrittenBytes += written;
}

size_t CAppendFile::write(const char *msg, size_t len)
{
    return ::fwrite_unlocked(msg, 1, len, mFp);
}

void CAppendFile::flush()
{
    ::fflush(mFp);
}

template int readFile(CStringArg fileName, int maxSize, std::string *content, int64_t *fileSize, int64_t *modifyTime, int64_t *createTime);
template int CReadSmallFile::readToString(int maxSize, std::string *content, int64_t *fileSize, int64_t *modifyTime, int64_t *createTime);

}