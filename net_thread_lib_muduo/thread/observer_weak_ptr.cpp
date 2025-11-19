#include <stdio.h>
#include <stdlib.h>
#include "observer_weak_ptr.h"

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

void Observable::obRegister(std::weak_ptr<Observer> x)
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
		// weak_ptr 不能单独使用，需要提升为 shared_ptr 后才能使用
		std::shared_ptr<Observer> obj(ite->lock());
		if(obj)
		{	
			printf("2, reference count: %ld\n", obj.use_count());
			obj->update();
			++ite;
		}
		else 
		{
			//对象已经销毁，从容器中删除掉 weak_ptr
			ite = mObserverVect.erase(ite);
		}
	}
}

int main()
{
	std::shared_ptr<configObserver> cfgOb(new configObserver); 
	Observable obMgr; 
	obMgr.obRegister(cfgOb);
	printf("1, reference count: %ld\n", cfgOb.use_count());
	obMgr.notifyObservers();
	printf("3, reference count: %ld\n", cfgOb.use_count());
	return 0;
}
