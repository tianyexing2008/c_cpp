#include <memory>
#include <string>
#include <map>
#include "guard.h"
#include "noncopyable.h"

class Stock
{
public:
    Stock(const std::string &key);
    std::string key()const;
    bool operator<(const Stock &rhs)
    {
        return mKey < rhs.mKey;
    }
private:
    std::string mKey;
};

class StockFactory: public muduo::noncopyable
{
public:
    StockFactory();
    ~StockFactory();
    
    std::shared_ptr<Stock>  get(const std::string &key);
private:
    CMutex  mMtx;
    std::map<std::string, std::shared_ptr<Stock>> mMap;
};