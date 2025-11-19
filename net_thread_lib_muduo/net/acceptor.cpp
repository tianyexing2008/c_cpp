#include "acceptor.h"
#include "logging.h"
#include "eventLoop.h"
#include "inetAddress.h"
#include "socketsOps.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
//#include <sys/types.h>
//#include <sys/stat.h>
#include <unistd.h>


namespace net{

CAcceptor::CAcceptor(CEventLoop* loop, const CInetAddress& listenAddr, bool reuseport)
  : mLoop(loop),
    mAcceptSocket(sockets::createNonblockingOrDie(listenAddr.family())),
    mAcceptChannel(loop, mAcceptSocket.fd()),
    mListening(false),
    mIdleFd(::open("/dev/null", O_RDONLY | O_CLOEXEC))
{
    mAcceptSocket.setReuseAddr(true);
    mAcceptSocket.setReusePort(reuseport);
    mAcceptSocket.bindAddress(listenAddr);
    mAcceptChannel.setReadCallback(std::bind(&CAcceptor::handleRead, this));
}

CAcceptor::~CAcceptor()
{
    mAcceptChannel.disableAll();
    mAcceptChannel.remove();
    ::close(mIdleFd);
}

void CAcceptor::listen()
{
    mLoop->assertInLoopThread();
    mListening = true;
    mAcceptSocket.listen();
    mAcceptChannel.enableReading();
}

void CAcceptor::handleRead()
{
    mLoop->assertInLoopThread();
    CInetAddress peerAddr;
    //FIXME loop until no more
    int connfd = mAcceptSocket.accept(&peerAddr);
    if (connfd >= 0)
    {
        // string hostport = peerAddr.toIpPort();
        // LOG_TRACE << "Accepts of " << hostport;
        if (mNewConnectionCallback)
        {
            mNewConnectionCallback(connfd, peerAddr);
        }
        else
        {
            sockets::close(connfd);
        }
    }
    else
    {
        LOG_SYSERR << "in CAcceptor::handleRead";
        // Read the section named "The special problem of
        // accept()ing when you can't" in libev's doc.
        // By Marc Lehmann, author of libev.
        if (errno == EMFILE)
        {
            ::close(mIdleFd);
            mIdleFd = ::accept(mAcceptSocket.fd(), NULL, NULL);
            ::close(mIdleFd);
            mIdleFd = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
        }
    }
}

}
