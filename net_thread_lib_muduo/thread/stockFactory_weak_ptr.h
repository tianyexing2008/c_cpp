#include <memory>
#include <string>
#include <map>
#include "guard.h"

class Stock
{
public:
    Stock(const std::string &key);

    std::string key()const;
    bool operator<(const Stock& rhs)
    {
        return mKey < rhs.mKey;
    }
private:
    std::string mKey;
};

class StockFactory: public std::enable_shared_from_this<StockFactory>
{
public:
    StockFactory();
    ~StockFactory();

    std::shared_ptr<Stock>  get(const std::string &key);
    
    void removeStock(Stock *stock);
private:
    static void weakDeleteCallback(const std::weak_ptr<StockFactory> &wkFactory, Stock *stock);
private:
    CMutex  mMtx;
    std::map<std::string, std::weak_ptr<Stock>> mMap;
};