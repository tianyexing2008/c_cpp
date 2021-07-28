
#include "mqtt/MQTTAsync.h"
#include "MqttClientImpl.h"

namespace MQTT 
{

/*
* @brief 消息接收回调
*/
int32_t messageArrived(void* context, char* topicName, int topicLen, MQTTAsync_message* message) 
{
    static int count = 0;
    std::string subMessage((char *)message->payload, message->payloadlen);
    std::string topic(topicName, topicLen);
    
    CMqttClientImpl::instance()->handleMessage(topicName, subMessage);


    //每次调用完需要释放，否则接收的消息一直是第一次的
    MQTTAsync_freeMessage(&message);
    MQTTAsync_free(topicName);

    char buf[64] = {0};
    snprintf(buf, sizeof(buf), "topic response --- %d", count);
    CMqttClientImpl::instance()->publishMessage("mqtt_example1", std::string(buf));
    count += 1;
    //看 MQTTAsync_messageArrived 的注释，这里返回1，否则再接收消息时会有问题
    return 1;
}

void onSubscribe(void* context, MQTTAsync_successData* message)
{

}

void onSubscribeFailure(void* context, MQTTAsync_failureData* message)
{

}


/*
* @brief 
*/
void onConnect(void* context, MQTTAsync_successData* message)
{
    printf("connect to server success\n");
    MQTTAsync client = (MQTTAsync)context;
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;

    //进行订阅
    opts.onSuccess = onSubscribe;
    opts.onFailure = onSubscribeFailure;
    opts.context = client;
    
    std::string topic = "MQTT_example";
    int retCode = MQTTAsync_subscribe(client, topic.c_str(), 1, &opts);
    if (retCode != MQTTASYNC_SUCCESS) 
    {
        printf("Failed to start subscribe, return code %d\n", retCode);
    }
    printf("subscribe topic[%s] success!\n", topic.c_str());
}

/*
* @brief 
*/
void onConnectFailure(void* context, MQTTAsync_failureData* message)
{
    printf("connect to server fail\n");
}

void onPublishFailure(void *context, MQTTAsync_failureData *response) 
{
    printf("Publish failed, returnCode %d\n", response ? -1 : response->code);
}

void onPublish(void *context, MQTTAsync_successData *response) 
{
    printf("send success\n");
}

//内部数据类的定义
class CMqttClientImpl::CMqttClientImplInternal 
{
public:
    CMqttClientImplInternal(): mTopic("MQTT_example"), mMqttConnected(false)
    {

    }

    ~CMqttClientImplInternal()
    {
        MQTTAsync_destroy(&mMqttClientHandle);
    }

    int32_t mqttClientCreate(MqttBrokerInfo *broker);

    bool mqttSendMessage(const std::string &topic, const std::string &message);

    bool mqttConnectState()
    {
        return mMqttConnected;
    }
public:
    std::string     mTopic;

private:
    bool            mMqttConnected;
    MQTTAsync       mMqttClientHandle;
    MQTTAsync_responseOptions mMqttResOpt;
};

IMqttClient* CMqttClientImpl::instance()
{
    static CMqttClientImpl mqttclientimpl;
    return &mqttclientimpl;
}

bool CMqttClientImpl::initialize(const Json::Value &config)
{
    MqttBrokerInfo brokerInfo;
    if(config.isMember("host") && config.isMember("port")
        && config.isMember("userName") && config.isMember("password")
        && config.isMember("clientId"))
    {
        brokerInfo.port = config["port"].asInt();
        brokerInfo.host = config["host"].asString();
        brokerInfo.userName = config["userName"].asString();
        brokerInfo.password = config["password"].asString();
        brokerInfo.clientId = config["clientId"].asString();
    }
    else
    {
        printf("mqtt broker info invalid!\n");
        return false;
    }

    int32_t iRet = mInteranl->mqttClientCreate(&brokerInfo);

    return iRet == MQTTASYNC_SUCCESS;
}

CMqttClientImpl::CMqttClientImpl()
{
    mInteranl = new CMqttClientImplInternal();
}

CMqttClientImpl::~CMqttClientImpl()
{
    if(mInteranl) 
    {
        delete mInteranl;
        mInteranl = nullptr;
    }
}

bool CMqttClientImpl::publishMessage(const std::string &topic, const std::string &message) 
{   
    if(mInteranl->mqttConnectState())
    {
        return mInteranl->mqttSendMessage(topic, message);
    }
    
    return false;
}

void CMqttClientImpl::handleMessage(const std::string &topic, const std::string &playload)
{
    if(mInteranl->mTopic.compare(topic) == 0)
    {
        printf("mqtt mesage arrived, topic[%s]:\nlen = %d, playload = %s\n", topic.c_str(), playload.length(), playload.c_str());
    }
}

/********************
* CMqttClientImplInternal
********************/
int32_t CMqttClientImpl::CMqttClientImplInternal::mqttClientCreate(MqttBrokerInfo *broker)
{
    char url[128] = {0};
    snprintf(url, sizeof(url), "%s:%d", broker->host.c_str(), broker->port);

    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
    MQTTAsync_createOptions create_opts = MQTTAsync_createOptions_initializer;
    create_opts.sendWhileDisconnected = 0;
    create_opts.maxBufferedMessages = 1000;

    printf("connect mqtt server: %s\n", url);
    int32_t ret = MQTTAsync_createWithOptions(&mMqttClientHandle, url, broker->clientId.c_str(), MQTTCLIENT_PERSISTENCE_NONE, NULL, &create_opts);
    if (MQTTASYNC_SUCCESS == ret)
    {
        ret = MQTTAsync_setCallbacks(mMqttClientHandle, mMqttClientHandle, /*connectionLost*/NULL, messageArrived, NULL); 
        printf("create mqtt client success, setCallback ret = %d\n", ret);
    }
    else
    {
        printf("create mqtt client fail, ret = %d\n", ret);
        return MQTTASYNC_FAILURE;
    }

    conn_opts.keepAliveInterval = 20;
    conn_opts.MQTTVersion = MQTTVERSION_3_1_1;
    conn_opts.cleansession = 1;
    conn_opts.onSuccess = onConnect;
    conn_opts.onFailure = onConnectFailure;
    conn_opts.context = mMqttClientHandle;
    conn_opts.username = broker->userName.c_str();
    conn_opts.password = broker->password.c_str();
    conn_opts.automaticReconnect = 1;
    conn_opts.connectTimeout = 60;

    int32_t retCode = MQTTAsync_connect(mMqttClientHandle, &conn_opts);
    if (MQTTASYNC_SUCCESS != retCode)
    {
        printf("Failed to connect, return code %d\n", retCode);
        return MQTTASYNC_FAILURE;
    }
    mMqttConnected = true;
    mMqttResOpt = MQTTAsync_responseOptions_initializer;
    mMqttResOpt.onSuccess = onPublish;
    mMqttResOpt.onFailure = onPublishFailure;
    return retCode;
}

bool CMqttClientImpl::CMqttClientImplInternal::mqttSendMessage(const std::string &topic, const std::string &message)
{
    int32_t iRet = MQTTAsync_send(mMqttClientHandle, topic.c_str(), message.length(), message.c_str(), 1, 0, &(mMqttResOpt));
    printf("mqtt send iRet = %d\n", iRet);
    return iRet == MQTTASYNC_SUCCESS;
}

}