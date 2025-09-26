#pragma once
#include "taskabstract.h"
#include <string>
#include "monitorsystemmessage.h"
#include "udpabstract.h"
#include "iocodec.h"
#include "message/pingmessage.h"
#include "message/commandsystemmessage.h"
#include "message/configsystemmessage.h"

class ConfigSystemTask : public TaskAbstract
{
public:
    static constexpr uint8_t DIV_COUNTER_10HZ = 10;
    static constexpr uint16_t MAX_LENGTH_BUFFFER_UDP = 512;
    ConfigSystemTask(std::string nameTask, int numElementQueueSet,
                     IOCodec* codec, UdpAbstract* udp);
    ~ConfigSystemTask();

    virtual void onTimer100HzProcess() override;
    virtual void onQueueSetMessageProcess(OSBase::QueueHandle queue_sem) override;
    virtual void onInitProcess() override;
private:
    void sendDataToAppCenter(CodecMessage *msg);
    void processReadDataFromUDP();
    //processor mesage
    void processorPingMessage(const PingMessage& ping);
    void processorCommandSystemMessage(const CommandSystemMessage& cmd);
    void processorConfigSystemMessage(const ConfigSystemMessage& config);
private:
    uint32_t mCounter100Hz{0};
    IOCodec* mCodec;
    UdpAbstract* mUdp;
    uint8_t mBufferUDP[MAX_LENGTH_BUFFFER_UDP] {0};
    CodecMessage mCodecMsgReceived;
};