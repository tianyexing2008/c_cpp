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
	void obRegister(std::shared_ptr<Observer> x);

	void notifyObservers();
private:
	mutable CMutex	mMtx;

	std::vector<std::shared_ptr<Observer> > mObserverVect;

	typedef std::vector<std::shared_ptr<Observer> >::iterator Iterator;
};
