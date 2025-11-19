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
            // 这里直接将 this 指针绑定到的 bind 返回的函数里对象里，存在危险，因为可能 StockFactory 析构了，在函数对象里还在用 StockFactory 对象
            pStock.reset(new Stock(key), std::bind(&StockFactory::weakDeleteCallback, this, std::placeholders::_1));
            wStock = pStock; // wStock 是引用，这里是将新创建的 shared_ptr 插入 map 中
        }

        return pStock;
    }
private:
    static void weakDeleteCallback(StockFactory *wkFactory, Stock *stock)
    {
        printf("Stock object destruct\n");
        if(wkFactory) //这里无法判断对象是还在存在，因为它只是一个地址(只要不为0就能使用，但不一定是 StockFactory 对象)
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