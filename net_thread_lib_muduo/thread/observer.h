#include <vector>

class Observer
{
public:
	virtual ~Observer();
	virtual void update() = 0;
};

class Observable
{
public:
	void register_(Observer *x);
	void unregister(Observer *x);

	void notifyObservers() {
		for(Observer *x:mObserverVect) {
			x->update();
		}		
	}
private:
	std::vector<Observer*> mObserverVect;
};
