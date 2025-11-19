#ifndef __CUSTOMER__H__
#define __CUSTOMER__H__
#include "guard.h"
#include "noncopyable.h"
#include <memory>
#include <vector>
#include <string>
#include <map>

class CustomerData: public muduo::noncopyable
{
public:
    CustomerData();
    ~CustomerData(){};

    typedef std::pair<std::string, int> Entry;
    typedef std::vector<Entry> EntryList;
    typedef std::map<std::string, EntryList> Map;
    typedef std::shared_ptr<Map> MapPtr;
public:
    int query(const std::string &customer, const std::string &stock)const;
    void update(const std::string &customer, const EntryList &entries);
    MapPtr getData()const;

    static int findEntry(const EntryList &entries, const std::string &stock);
private:
    mutable CMutex mutex;
    MapPtr data_;
};

#endif