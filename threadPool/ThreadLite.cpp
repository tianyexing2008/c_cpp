#include "Thread.h"

struct ThreadLiteInternal
{
	CThreadLite::ThreadProc mProc;
};

CThreadLite::CThreadLite(ThreadProc proc, const char * name, int priority, int policy, int stackSize)
	: CThread(name, priority, policy, stackSize)
{
	mInternal = new ThreadLiteInternal;
	mInternal->mProc = proc;
}

CThreadLite::~CThreadLite()
{
	// 先停止线程体
	if (!isThreadOver())
	{
		destroyThread();
	}

	delete mInternal;
}

void CThreadLite::threadProc()
{
	mInternal->mProc(*this);
}
