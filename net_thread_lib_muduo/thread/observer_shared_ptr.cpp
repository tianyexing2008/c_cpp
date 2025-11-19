#include <stdio.h>
#include <stdlib.h>
#include "observer_shared_ptr.h"

configObserver::configObserver()
{

}

configObserver::~configObserver()
{
	printf("configObserver destructor\n");
}

void configObserver::update()
{
	printf("derived configObserver\n");
}

void Observable::obRegister(std::shared_ptr<Observer> x)
{
	CGuard guard(mMtx);
	mObserverVect.push_back(x);
}

void Observable::notifyObservers()
{
	CGuard guard(mMtx);
	Iterator ite = mObserverVect.begin();
	while (ite != mObserverVect.end())
	{
		printf("2, reference count: %ld\n", (*ite).use_count()); //这是在容器里，拷贝了所有权，所以这里输入为2
		(*ite)->update();
		++ite;
	}
}

int main()
{
	std::shared_ptr<configObserver> cfgOb(new configObserver);  //这里引用计数为1
	{
		Observable obMgr; 
		obMgr.obRegister(cfgOb); //添加到容器中后，即拷贝了所有权，引用计数为2
		printf("1, reference count: %ld\n", cfgOb.use_count()); //所以这里输出引用计数为2
		obMgr.notifyObservers();
		printf("3, reference count: %ld\n", cfgOb.use_count()); //这里引用计数仍为2，因为容器里还有拷贝
	} //这里加了一个作用域，让obMgr释放后容器里没有拷贝了，引用计数减1
	printf("3, reference count: %ld\n", cfgOb.use_count()); //这里只剩下cfgOb拥有所有权，所以这里输出为1
	return 0;
} //离开main函数后，cfgOb 释放，引用计数为0，正确释放资源
