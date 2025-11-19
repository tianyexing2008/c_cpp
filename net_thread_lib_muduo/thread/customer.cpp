#include "customer.h"
#include "thread.h"
#include <assert.h>
#include <unistd.h>

CustomerData::CustomerData(): data_(new Map)
{

}

CustomerData::MapPtr CustomerData::getData() const
{
    CGuard lock(mutex);
    return data_;
}

int CustomerData::query(const std::string &customer, const std::string &stock)const
{
    MapPtr data = getData(); //getData已经加锁，data是局部数据操作不需要加锁
    Map::const_iterator entries = data->find(customer);
    if(entries != data->end())
    {
        return findEntry(entries->second, stock);
    }
    else
    {
        return -1;
    }
}

void CustomerData::update(const std::string &customer, const EntryList &entries)
{
    CGuard lock(mutex);
    if(!data_.unique()) //如果资源多人在使用则创建一个副本
    {
        MapPtr newData(new Map(*data_));//创建一个副本，更新副本
        data_.swap(newData); //如果使用 data_= newData;则副本的引用计数变成了2，即data和newData都指向了副本资源。
    }
    assert(data_.unique());
    (*data_)[customer] = entries;
}

int CustomerData::findEntry(const EntryList &entries, const std::string &stock)
{
    for(const auto &entry:entries)
    {
        if(entry.first == stock)
        {
            return entry.second;
        }
    }
    return -1;
}

CustomerData gData;

void threadFunc(std::string &name)
{
    CustomerData::EntryList vect;
    //初始化列表方式 or vect.push_back(std::make_pair(name, 100)) or vect.push_back(std::pair<std::string, int>(name, 100))
    for(int i = 100; i < 110; i++)
    {
        vect.push_back({name, i});
    }
    
    gData.update(name, vect);
}

int main()
{
    muduo::CThread thread(threadFunc, "google");
    thread.start();

    sleep(2);
    int stockNo = gData.query("google", "google");
    printf("stockNo = %d\n", stockNo);
    thread.join();

    return 0;
}