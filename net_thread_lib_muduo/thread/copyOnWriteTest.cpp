#include <vector>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "guard.h"

class Foo
{
public:
    void doit()const;
};

typedef std::vector<Foo> FooList;
typedef std::shared_ptr<FooList> FooListPtr;
FooListPtr g_foos;
CMutex mutex;

void post(Foo &f)
{
    printf("post\n");
    CGuard lock(mutex);
    if(!g_foos.unique())
    {
        g_foos.reset(new FooList(*g_foos)); //以当前列表创建一个副本，g_foos.reset后 g_foos指向副本
        printf("copy the whole list\n");
    }
    assert(g_foos.unique());
    g_foos->push_back(f);
}

void post_v2(Foo &f)
{
    CGuard lock(mutex);
    g_foos->push_back(f);
}

void traverse()
{
    FooListPtr foos;
    {
        CGuard lock(mutex);
        foos = g_foos;
        assert(!g_foos.unique());
    }
    for(auto ite = foos->begin(); ite != foos->end(); ite++)
    {
        ite->doit();
    }
}

void Foo::doit() const
{
    Foo f;
    post(f);
}

int main()
{
    g_foos.reset(new FooList);
    Foo f;
    post(f);
    traverse();

    return 0;
}