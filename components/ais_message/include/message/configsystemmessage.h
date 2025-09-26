#ifndef CONFIG_SYSTEM_MESSAGE_H
#define CONFIG_SYSTEM_MESSAGE_H

#include "abstractmessage.h"
#include "stdint.h"
#include "monitorcodec.h"
#include <string>

class ConfigSystemMessage: public AbstractMessage
{
public:
#pragma pack(push, 1)
    struct config_system_data_t
    {
        char ssidWifiSta[32]{0};
        char passWifiSta[32]{0};
        char uriMqtt[96] {0};
        uint16_t portMqtt{0};
        char mqttUser[32] {0};
        char mqttPass[32] {0};
    };
#pragma pack(pop)
    ConfigSystemMessage();
    ConfigSystemMessage(config_system_data_t config);
    ~ConfigSystemMessage();

    const config_system_data_t& getConfigSystem() const;
	void setConfigSyste(const config_system_data_t &config);
 
    virtual bool packData(CodecMessage *msg) override;
	virtual bool unpackData(CodecMessage *msg) override;

    //getter setter
     void setSsid(const char* value);
     void setPass(const char* value);
     void setUriMqtt(const char* value);
     void setPortMqtt(int value);
     void setMqttUser(const char* value);
     void setMqttPass(const char* value);

private:
    config_system_data_t mConfig;
};

#endif