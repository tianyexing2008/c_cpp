#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include <string>
#include "guard.h"

class Fool
{
public:
    Fool(const char *str): mName(str)
    {}
    ~Fool()
    {
        printf("Fool_%s destructor\n", mName.c_str());
    }
private:
    std::string mName;
};

std::shared_ptr<Fool> globalFoo = std::make_shared<Fool>("global");
CMutex globalMutex;

int main()
{
    std::shared_ptr<Fool> localFool(new Fool("local"));
    {
        CGuard locl(globalMutex);
        globalFoo = localFool; //这里导致global管理的原对象被释放，然后管理新的对象
    }

    return 0;
}