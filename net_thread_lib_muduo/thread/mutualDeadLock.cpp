#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mutualDeadLock.h"
#include "thread.h"
#include "singleton.h"

Inventory g_Inventory;
Request::Request(const char *str): mName(str)
{

}

Request::~Request()
{
    CGuard lock(mMutex);
    g_Inventory.remove(this);
}

void Request::process()
{
    CGuard lock(mMutex);
    g_Inventory.add(this);
}

void Request::print()const
{
    CGuard lock(mMutex);
    printf("request name %s\n", mName.c_str());
}

void Inventory::add(Request *req)
{
    CGuard lock(mMutex);
    mRequestsSet.insert(req);
}

void Inventory::remove(Request *req)
{
    CGuard lock(mMutex);
    mRequestsSet.erase(req);
}

void Inventory::printAll()const
{
    std::set<Request *> tmpSet;
    {
        CGuard lock(mMutex);
        tmpSet = mRequestsSet;
    }
    
    for(auto &it: tmpSet)
    {
        
        it->print();
    }
    printf("Inventory::printAll unlocked\n");
}

using namespace muduo;

void threadFunc()
{
    Request *req = new Request("threadFunc");
    req->process();
    delete req;
}

int main()
{
    // g_Inventory.printAll();
    // CThread thread(threadFunc);
    // thread.start();
    // thread.join();
    
    return 0;
}