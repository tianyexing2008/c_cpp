#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "MqttClientImpl.h"

int main()
{
    Json::Value brokerInfo;
    brokerInfo["host"] = "tcp://broker-cn.emqx.io"; //免费的mqtt broker
    brokerInfo["userName"] = "test";
    brokerInfo["password"] = "111";
    brokerInfo["clientId"] = "111111";
    brokerInfo["port"] = 1883;

    bool iRet = MQTT::CMqttClientImpl::instance()->initialize(brokerInfo);
    printf("initialize mqtt iRet = %d\n", iRet);
    if(iRet)
    {
        iRet = MQTT::CMqttClientImpl::instance()->publishMessage("liangshen_mqtt", "liangshen mqtt test!");
        printf("mqtt publish message iRet = %d\n", iRet);
    }

    while (1)
    {
        /* code */
    }
    
    return 0;
}
