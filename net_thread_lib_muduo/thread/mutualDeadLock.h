#include <set>
#include <string>
#include "guard.h"

class Request
{
public:
    Request(const char *str);
    ~Request();
    void process();
    void print()const;
private:
    std::string mName;
    mutable CMutex mMutex;
};
class Inventory
{
public:
    void add(Request *req);
    void remove(Request *req);
    void printAll() const;

private:
    mutable CMutex mMutex;
    std::set<Request*> mRequestsSet;
};