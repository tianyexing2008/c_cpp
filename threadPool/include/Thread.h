
#ifndef _THREAD_H_
#define _THREAD_H_

#include <string>
#include "Defs.h"
#include "Function.h"

struct ThreadInternal;

class CThread
{
	friend class CThreadManager;
public:
	enum Priority
	{
		priorTop = 2,
		priorBottom = 127,
		priorDefault = 64,
	};

	enum Policy
	{
		policyNormal = 0,		///< 普通线程
		policyRealtime = 1		///< 实时线程
	};

public:

	/// 构造函数，并没有创建系统线程
	/// \param name 线程名称字符串，名称不一定会传给操作系统，但CThreadManager
	///		   管理线程时会用到。
	/// \param priority 线程优先级，值越小表示优先级越高，会被转化成对应操作系统
	///        平台的优先级。取值priorTop到priorBottom，默认值priorDefault。
	/// \param policy 线程调度策略
	/// \param stackSize 为线程指定的堆栈大小，如果等于0或者小于平台要求必须的值，
	///        则使用平台缺省值。
	CThread(const char * name, int priority = priorDefault, int policy = policyNormal, int stackSize = 0);

	/// 析构函数，如果线程还在执行，会销毁线程
	virtual ~CThread();

	/// 创建线程
	bool createThread();

	/// 销毁线程，设置退出标志，线程的执行体需要在各退出点判断这个标志。
	/// 阻塞方式，等待线程体执行结束，才返回
	bool destroyThread();

	/// 取消线程，设置线程退出标志，非阻塞方式，不等待线程结束
	bool cancelThread();

	/// 终止线程，和销毁线程不同在于它是由操作系统强制销毁线程，不保证用户数据安全。
	bool terminateThread();

	/// 判断线程是否正在执行
	bool isThreadOver();

	/// 得到线程ID
	int	getThreadID();

	/// 线程执行体，是一个虚函数，派生的线程类中重载此函数，实现各自的行为。
	virtual void threadProc() = 0;

	/// 设置线程名称
	/// \param name 新的线程名称
	void setThreadName(const char * name);

	/// 套接字共享接口，对pSOS操作系统调用系统接口返回共享得到的一个引用套接字
	/// 对于其他操作系统，直接返回传入的套接字
	/// \param socket 需要共享的原始套接字
	/// \return 共享后的引用套接字
	int shareSocket(int socket);

	/// 设置超时时间
	/// \param milliSeconds 超时毫秒数，设置为0表示清空设置
	void setTimeout(ulong milliSeconds);

	/// 设置超时时间，主要用于优化继承类中实现
	/// \param [in] milliSeconds 超期时间(以毫秒记)
	/// \param [in] now 当前时间(以毫秒记)
	void setTimeout(int milliSeconds, uint64 now);

	/// 判断是否超时，也就是判断从最后一次SetTimeout到这次调用IsTimeout的时间间隔
	/// 是否已经在超时时间之外。这两个接口目的是提供给用户监视和调度线程的方法，
	/// CThread类并不会根据是否超时对线程执行做任何干预。CThreadManager打印所有
	/// 线程状态时会调用这个接口。
	bool isTimeout();

	/// 得到调用线程的ID，而不是某个线程对象的ID，是一个静态函数。
	static int getCurrentThreadID();

	/// 设置当前线程超时时间
	/// \param [in] milliSeconds 超时毫秒数，设置为0表示清空设置
	/// \return true:设置成功，false:设置失败
	static bool setCurrentTimeout(ulong milliSeconds);

	bool looping() const;
protected:
	ThreadInternal	*mInternal; ///内部数据，不对外暴露
};


struct ThreadLiteInternal;

/// \class CThreadLite
/// \brief 多平台线程类; 提供组合方式创建线程
/// \see CThreadManager
class CThreadLite : public CThread
{
	CThreadLite(CThreadLite const&);
	CThreadLite& operator=(CThreadLite const&);

public:
	/// 线程执行体, 参数为线程对象的引用
	typedef TFunction1<void, CThreadLite&> ThreadProc;

	/// 构造函数，并没有创建系统线程
	/// \param proc 线程体函数
	/// \param name 线程名称字符串，名称不一定会传给操作系统，但CThreadManager
	///		   管理线程时会用到。
	/// \param priority 线程优先级，值越小表示优先级越高，会被转化成对应操作系统
	///        平台的优先级。取值priorTop到priorBottom，默认值priorDefault。
	/// \param policy 线程调度策略
	/// \param stackSize 为线程指定的堆栈大小，如果等于0或者小于平台要求必须的值，
	///        则使用平台缺省值。
	CThreadLite(ThreadProc proc, const char* name, int priority = priorDefault, int policy = policyNormal, int stackSize = 0);

	/// 析构函数，如果线程还在执行，会销毁线程
	virtual ~CThreadLite();

private:
	/// 线程执行体
	virtual void threadProc();

	ThreadLiteInternal	*mInternal;
};

/// \class IThreadManager
/// \brief 线程管理类
/// \see CThread
class IThreadManager
{
public:

	static IThreadManager* instance();

	/// 虚析构函数
	virtual ~IThreadManager();

	/// 注册主线程，只能在主线程中调用，这样在显示所有线程的信息是也可以看到主线程信息
	virtual void RegisterMainThread() = 0;

	/// 打印所有线程信息
	virtual void DumpThreads() = 0;

	/// 得到超时的线程名称
	virtual bool GetTimeOutThreads(std::string& names) = 0;
};


#endif //_THREAD_H_

