#include <memory>
#include <string>
#include <map>
#include "guard.h"
#include "stockFactory_shared_ptr.h"


Stock::Stock(const std::string &key): mKey(key)
{

}

std::string Stock::key()const
{
    return mKey;
}


StockFactory::StockFactory()
{

}

StockFactory::~StockFactory()
{
    printf("StockFactory destruct\n");
}

std::shared_ptr<Stock>  StockFactory::get(const std::string &key)
{
    CGuard guard(mMtx);
    auto ite = mMap.find(key);
    if(ite != mMap.end())
    {
        return ite->second;
    }

    auto pStock = std::make_shared<Stock>(key);
    //mMap[key] = pStock; 这个[]操作符会先查找，没找到然后插入一个空值，然后再被覆盖为pStock
    //auto pair = mMap.insert(std::pair<std::string, std::shared_ptr<Stock>>(key, pStock));
    auto pair = mMap.insert({key, pStock}); //c++11支持的花括号初始化
    return pair.first->second;
}

int main()
{
    return 0;
}