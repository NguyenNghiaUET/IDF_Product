#include "configsystemmessage.h"

ConfigSystemMessage::ConfigSystemMessage()
    : AbstractMessage(MessageId::MONITOR_SYSTEM_ECU_MESSAGE)
{
}
ConfigSystemMessage::ConfigSystemMessage(config_system_data_t config)
    : AbstractMessage(MessageId::MONITOR_SYSTEM_ECU_MESSAGE), mConfig(config)
{
}

ConfigSystemMessage::~ConfigSystemMessage()
{
}

const ConfigSystemMessage::config_system_data_t &ConfigSystemMessage::getConfigSystem() const
{
    return mConfig;
}

void ConfigSystemMessage::setConfigSyste(const config_system_data_t &config)
{
    mConfig = config;
}

bool ConfigSystemMessage::packData(CodecMessage *msg)
{
    if (msg)
    {
        memcpy(msg->mDataRaw, &mConfig, sizeof(config_system_data_t));
        msg->mMsgDataLength = sizeof(config_system_data_t);
        msg->mMsgID = (uint8_t)this->mMessageId;
        return true;
    }
    else
    {
        return false;
    }
}

bool ConfigSystemMessage::unpackData(CodecMessage *msg)
{
    if (msg)
    {
        memcpy(&mConfig, msg->mDataRaw, sizeof(config_system_data_t));
        return true;
    }
    else
    {
        return false;
    }
}

void ConfigSystemMessage::setSsid(const char *value)
{
    strncpy(mConfig.ssidWifiSta, value, sizeof(mConfig.ssidWifiSta) - 1);
    mConfig.ssidWifiSta[sizeof(mConfig.ssidWifiSta) - 1] = '\0'; // Đảm bảo null-terminated
}

void ConfigSystemMessage::setPass(const char *value)
{
    strncpy(mConfig.passWifiSta, value, sizeof(mConfig.passWifiSta) - 1);
    mConfig.passWifiSta[sizeof(mConfig.passWifiSta) - 1] = '\0'; // Đảm bảo null-terminated
}

void ConfigSystemMessage::setUriMqtt(const char *value)
{
    strncpy(mConfig.uriMqtt, value, sizeof(mConfig.uriMqtt) - 1);
    mConfig.uriMqtt[sizeof(mConfig.uriMqtt) - 1] = '\0'; // Đảm bảo null-terminated
}

void ConfigSystemMessage::setPortMqtt(int value)
{
    mConfig.portMqtt = value; // Chuyển chuỗi sang int
}

void ConfigSystemMessage::setMqttUser(const char *value)
{
    strncpy(mConfig.mqttUser, value, sizeof(mConfig.mqttUser) - 1);
    mConfig.mqttUser[sizeof(mConfig.mqttUser) - 1] = '\0'; // Đảm bảo null-terminated
}

void ConfigSystemMessage::setMqttPass(const char *value)
{
    strncpy(mConfig.mqttPass, value, sizeof(mConfig.mqttPass) - 1);
    mConfig.mqttPass[sizeof(mConfig.mqttPass) - 1] = '\0'; // Đảm bảo null-terminated
}