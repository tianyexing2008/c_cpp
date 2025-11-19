#include <memory>
#include <string>
#include <map>
#include <functional>
#include "guard.h"
#include "stockFactory_weak_ptr.h"

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
    std::shared_ptr<Stock> pStock;
    CGuard guard(mMtx);

    std::weak_ptr<Stock> &wStock = mMap[key];
    pStock = wStock.lock();
    if(!pStock)
    {
        // 将 shared_from_this() 转换为 weak_ptr， 这样就不会延长对象的生命期
        pStock.reset(new Stock(key), std::bind(&StockFactory::weakDeleteCallback, std::weak_ptr<StockFactory>(shared_from_this()), std::placeholders::_1));

        wStock = pStock; // wStock 是引用，这里是将新创建的 shared_ptr 插入 map 中
    }

    return pStock;
}

void StockFactory::weakDeleteCallback(const std::weak_ptr<StockFactory> &wkFactory, Stock *stock)
{
    printf("Stock object destruct\n");
    //尝试提升为 shared_ptr
    std::shared_ptr<StockFactory> factory(wkFactory.lock());
    if(factory) //如果 factory 还在，则删除对应 stock
    {
        factory->removeStock(stock);
    }
    else
    {
        printf("StockFactory is not exist\n");
    }

    delete stock;
}

void StockFactory::removeStock(Stock *stock)
{
    if(stock)
    {
        CGuard guard(mMtx);
        mMap.erase(stock->key());
    }
}


void testLongLifeFactory()
{
    printf("test long life factory\n");
    std::shared_ptr<StockFactory> factory = std::make_shared<StockFactory>();
    {
        std::shared_ptr<Stock> stock1 = factory->get("GOOGLE");
        std::shared_ptr<Stock> stock2 = factory->get("IBM");
    }
}

void shortLifeFactory()
{
    printf("test short life factory\n");
    std::shared_ptr<Stock> stock1;
    std::shared_ptr<Stock> stock2;
    {
        auto factory = std::make_shared<StockFactory>();
        stock1 = factory->get("google");
        stock2 = factory->get("ibm");
    }
}

int main()
{
    testLongLifeFactory();
    shortLifeFactory();
    return 0;
}