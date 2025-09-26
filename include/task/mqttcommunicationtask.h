#include "mqttclientabstract.h"
#include "taskabstract.h"

class MqttCommunicationTask : public TaskAbstract
{
public:
    static constexpr uint16_t DIV_COUNTER_10_S = 1000; // 10s
    static constexpr uint16_t DIV_COUNTER_1_S = 100; // 10s
    const std::string TOPIC_SUBSCRIBE_WEB_MESSAGE = "TopicSubscribeFromWebMessage";
    const std::string TOPIC_PUBLISH_TO_WEB_MESSAGE = "TopicPublishToqWebMessage";

    MqttCommunicationTask(MqttClientAbstract *mqttClient, std::string nameTask, int numElementQueueSet);
    ~MqttCommunicationTask();

    virtual void onTimer100HzProcess() override;
    virtual void onQueueSetMessageProcess(OSBase::QueueHandle queue_sem) override;
    virtual void onInitProcess() override;

private:
    void onMqttMessageCallback(const std::string &topic, const std::string &message);
    void onSetupSubscribeTopicCallback();

private:
    MqttClientAbstract *mMqttClient{nullptr};
    uint32_t mCounter100Hz{0};
    bool mIsStartedMqtt{false};
    bool mHasWifiConenction {false};
};