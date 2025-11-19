#include <vector>
#include <memory>
#include "guard.h"

class Observer
{
public:
	virtual ~Observer() {
		
	}
	virtual void update() = 0;
};

class configObserver: public Observer
{
public:
	configObserver();
	~configObserver();
	virtual void update();
};

class Observable
{
public:
	void obRegister(std::weak_ptr<Observer> x);
	// void unregister(Observer *x); //不需要了

	void notifyObservers();
private:
	CMutex	mMtx;

	//容器中保存的是weak_ptr，能探测对象是否还存在
	std::vector<std::weak_ptr<Observer> > mObserverVect;

	typedef std::vector<std::weak_ptr<Observer> >::iterator Iterator;
};
