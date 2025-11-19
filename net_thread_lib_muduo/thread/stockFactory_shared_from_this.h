#include <memory>
#include <string>
#include <map>
#include "guard.h"

class Stock
{
public:
    Stock(const std::string &key): mKey(key)
    {

    }

    std::string key()
    {
        return mKey;
    }

private:
    std::string mKey;
};

class StockFactory: public std::enable_shared_from_this<StockFactory>
{
public:
    StockFactory()
    {

    }

    ~StockFactory()
    {
        printf("StockFactory destruct\n");
    }

    std::shared_ptr<Stock>  get(const std::string &key)
    {
        std::shared_ptr<Stock> pStock;
        CGuard guard(mMtx);

        std::weak_ptr<Stock> &wStock = mVector[key];
        pStock = wStock.lock();
        if(!pStock)
        {
            // 这里用 shared_from_this() 传给 bind 绑定后返回的函数里， 将延长对象的生命期
            pStock.reset(new Stock(key), std::bind(&StockFactory::weakDeleteCallback, shared_from_this(), std::placeholders::_1));
            wStock = pStock; // wStock 是引用，这里是将新创建的 shared_ptr 插入 map 中
        }

        return pStock;
    }
private:
    static void weakDeleteCallback(std::shared_ptr<StockFactory> wkFactory, Stock *stock)
    {
        printf("Stock object destruct\n");
        if(wkFactory) 
        {
            wkFactory->removeStock(stock);
        }
        else
        {
            printf("StockFactory is not exist\n");
        }

        delete stock;
    }

    void removeStock(Stock *stock)
    {
        if(stock)
        {
            CGuard guard(mMtx);
            mVector.erase(stock->key());
        }
    }
private:
    CMutex  mMtx;
    std::map<std::string, std::weak_ptr<Stock>> mVector;
};