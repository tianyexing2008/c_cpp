#include "json/json.h"
#include "MqttClient.h"

namespace MQTT {

class CMqttClientImpl: public IMqttClient 
{
public:
    static IMqttClient* instance();

    bool initialize(const Json::Value &config);
public:
    CMqttClientImpl();
    virtual ~CMqttClientImpl();

    virtual bool publishMessage(const std::string &topic, const std::string &message);

    virtual void handleMessage(const std::string &topic, const std::string &playload);

// 采用内部数据的方式封装
private:
    class CMqttClientImplInternal;      //前置声明就可以，因为只声明指针
    CMqttClientImplInternal *mInteranl; //所有操作都用mInternal进行操作
};

}