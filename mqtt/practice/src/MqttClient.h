#include <stdint.h>
#include <string>

namespace MQTT {

struct MqttBrokerInfo
{
    int16_t port;
    std::string host;
    std::string userName;
    std::string password;
    std::string clientId;
};

// mqtt client 抽象类
class IMqttClient {
public:
    virtual ~IMqttClient() {

    }

    virtual bool initialize(const Json::Value &config) = 0;

    virtual bool publishMessage(const std::string &topic, const std::string &message) = 0;

    virtual void handleMessage(const std::string &topic, const std::string &playload) = 0;
};

}