#include "task/mqttcommunicationtask.h"
#include "loggermanager.h"
#include "common/common.h"

MqttCommunicationTask::MqttCommunicationTask(MqttClientAbstract *mqttClient, std::string nameTask, int numElementQueueSet)
    : TaskAbstract(nameTask, numElementQueueSet)
{
    mMqttClient = mqttClient;
}

MqttCommunicationTask::~MqttCommunicationTask()
{
    mMqttClient->stop();
}

void MqttCommunicationTask::onTimer100HzProcess()
{
    if (mCounter100Hz % DIV_COUNTER_10_S == 0)
    {
        // reconenct to mqtt if started mqtt + has wifi
        if (mIsStartedMqtt && mMqttClient->isMqttConnected() == false && mHasWifiConenction)
        {
            mMqttClient->reconnect();
            LOG_INFO("MqttCommunicationTask", "Reconnect to mqtt");
        }
    }

    if (mCounter100Hz % DIV_COUNTER_1_S == 0)
    {
        if (mMqttClient->isMqttConnected() && mHasWifiConenction)
        {
            LOG_INFO("MqttCommunicationTask", "send to mqtt");
            mMqttClient->publish(TOPIC_PUBLISH_TO_WEB_MESSAGE, "data: " + std::to_string(mCounter100Hz));
        }
    }
    mCounter100Hz++;
}

void MqttCommunicationTask::onQueueSetMessageProcess(OSBase::QueueHandle queue_sem)
{
    if (queue_sem == gSemWifiStaConnectedFromWifiTaskToMqttTask)
    {
        //sem take
        mOSBase->semTake(gSemWifiStaConnectedFromWifiTaskToMqttTask);
        LOG_INFO("MqttCommunicationTask", "received event wifi sta connected");
        if (mIsStartedMqtt == false)
        {
            LOG_INFO("MqttCommunicationTask", "Has wifi connection -> start mqtt client");
            mMqttClient->start();
            mIsStartedMqtt = true;
        }
        mHasWifiConenction = true;
    }
    else if (queue_sem == gSemWifiStaDisConnectedFromWifiTaskToMqttTask)
    {
        //sem take
        mOSBase->semTake(gSemWifiStaDisConnectedFromWifiTaskToMqttTask);
        LOG_INFO("MqttCommunicationTask", "Wifi disconnection");
        mHasWifiConenction = false;
    }
}

void MqttCommunicationTask::onInitProcess()
{
    LOG_INFO("MqttCommunicationTask", "Init task");
    if (mMqttClient == nullptr)
    {
        LOG_ERROR("MqttCommunicationTask", "Why mqtt client null?");
        return;
    }
    mMqttClient->setReceivedMessageCallback(std::bind(&MqttCommunicationTask::onMqttMessageCallback, this,
                                                      std::placeholders::_1, std::placeholders::_2));
    mMqttClient->setSubcribeToppicCallback(std::bind(&MqttCommunicationTask::onSetupSubscribeTopicCallback, this));
}

void MqttCommunicationTask::onMqttMessageCallback(const std::string &topic, const std::string &message)
{
    LOG_DEBUG("[MqttCommunicationTask]", "[onMqttMessageCallback]%s: %s", topic.c_str(), message.c_str());
}

void MqttCommunicationTask::onSetupSubscribeTopicCallback()
{
    mMqttClient->subscribe(TOPIC_SUBSCRIBE_WEB_MESSAGE);
}
