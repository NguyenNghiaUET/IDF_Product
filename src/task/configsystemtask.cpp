#include "task/configsystemtask.h"
#include "loggermanager.h"
#include "common/common.h"
#include "common/storeflashmanager.h"

ConfigSystemTask::ConfigSystemTask(std::string nameTask, int numElementQueueSet, IOCodec *codec, UdpAbstract *udp)
    : TaskAbstract(nameTask, numElementQueueSet)
{
    mCodec = codec;
    mUdp = udp;
}

ConfigSystemTask::~ConfigSystemTask()
{
}

void ConfigSystemTask::onTimer100HzProcess()
{
    // read data
    processReadDataFromUDP();
    if (mCounter100Hz % DIV_COUNTER_10HZ == 0)
    {
    }
}

void ConfigSystemTask::onQueueSetMessageProcess(OSBase::QueueHandle queue_sem)
{
}

void ConfigSystemTask::onInitProcess()
{
    if (mUdp == nullptr || mCodec == nullptr)
    {
        LOG_ERROR("ConfigSystemTask", "Why udp or codec null?");
        return;
    }
    mUdp->init();
}

void ConfigSystemTask::sendDataToAppCenter(CodecMessage *msg)
{
    if (mUdp == nullptr)
    {
        return;
    }
    uint8_t bufferSend[CodecMessage::MAX_DATA_LENGTH];
    uint16_t msgLength = mCodec->encode(msg, bufferSend);
    if (msgLength > 0)
    {
        mUdp->send(bufferSend, msgLength);
    }
}

void ConfigSystemTask::processReadDataFromUDP()
{
    if (mUdp == nullptr || mCodec == nullptr)
    {
        return;
    }
    if (mUdp->checkAvailable() == false)
    {
        // no data udp
        return;
    }
    // has data udp
    // read data udp
    int lengthReceived = mUdp->read(mBufferUDP, MAX_LENGTH_BUFFFER_UDP);
    // check length data
    if (lengthReceived <= 0)
    {
        return;
    }
    for (uint16_t index = 0; index < lengthReceived; index++)
    {
        if (mCodec->decode(mBufferUDP[index], &mCodecMsgReceived))
        {
            AbstractMessage::MessageId msgID = static_cast<AbstractMessage::MessageId>(mCodecMsgReceived.mMsgID);
            switch (msgID)
            {
            case AbstractMessage::MessageId::PING_MESSAGE:
            {
                PingMessage msgPing;
                auto ret = msgPing.unpackData(&mCodecMsgReceived);
                if (ret)
                {
                    processorPingMessage(msgPing);
                }
                break;
            }
            case AbstractMessage::MessageId::COMMAND_SYSTEM_MESSAGE:
            {
                CommandSystemMessage msgCmd;
                auto ret = msgCmd.unpackData(&mCodecMsgReceived);
                if (ret)
                {
                    processorCommandSystemMessage(msgCmd);
                }
                break;
            }
            case AbstractMessage::MessageId::CONFIG_SYSTEM_MESSAGE:
            {
                ConfigSystemMessage msgConfig;
                auto ret = msgConfig.unpackData(&mCodecMsgReceived);
                if (ret)
                {
                    processorConfigSystemMessage(msgConfig);
                }
                break;
            }
            default:
            {
                break;
            }
            }
        }
    }
}

void ConfigSystemTask::processorPingMessage(const PingMessage &ping)
{
    PingMessage::TypePingMessage typePing = ping.getPingMessage().typePing;
    switch (typePing)
    {
    case PingMessage::TypePingMessage::BroadCast:
    {
        // Update remote to source of this ping
        mUdp->updateRemoteFromRemoteOfDataReceive();
        // create response mesage
        PingMessage::ping_message_t ping;
        ping.typePing = PingMessage::TypePingMessage::Response;
        ping.deviceType = MessageCommon::DEVICE_TYPE::AquaControlDevice;
        ping.deviceID = gDeviceID;
        PingMessage pingMsg = PingMessage(ping);
        // pack and send
        CodecMessage codecMsg;
        auto ret = pingMsg.packData(&codecMsg);
        if (ret)
        {
            sendDataToAppCenter(&codecMsg);
        }
        break;
    }
    case PingMessage::TypePingMessage::Request:
    {
        // Update remote to source of this ping
        // create response mesage
        PingMessage::ping_message_t ping;
        ping.typePing = PingMessage::TypePingMessage::Response;
        ping.deviceType = MessageCommon::DEVICE_TYPE::AquaControlDevice;
        ping.deviceID = gDeviceID;
        PingMessage pingMsg = PingMessage(ping);
        // pack and send
        CodecMessage codecMsg;
        auto ret = pingMsg.packData(&codecMsg);
        if (ret)
        {
            sendDataToAppCenter(&codecMsg);
        }
        break;
    }
    default:
    {
        LOG_ERROR("ConfigSystemTask","Received ping type: %d not support", (int)typePing);
        break;
    }
    }
}

void ConfigSystemTask::processorCommandSystemMessage(const CommandSystemMessage &cmd)
{
    CommandSystemMessage::CommandSystemType cmdType = cmd.getCommandSystemMessage().cmdType;
    switch (cmdType)
    {
    case CommandSystemMessage::CommandSystemType::READ_CONFIG_SYSTEM:
    {
        // send config system response
        CodecMessage codecMsg;
        auto ret = gConfigSystem.packData(&codecMsg);
        if (ret)
        {
            sendDataToAppCenter(&codecMsg);
        }
        break;
    }
    default:
    {
        LOG_ERROR("ConfigSystemTask","Received cmd type: %d not support", (int)cmdType);
        break;
    }
    }
}

void ConfigSystemTask::processorConfigSystemMessage(const ConfigSystemMessage &config)
{
    gConfigSystem = config;
    // write to flash
    auto ret = StoreFlashManager::getInstance()->saveConfigToFlash(gConfigSystem);
    if (ret)
    {
        LOG_INFO("ConfigSystemTask", "Write config from nvs success");
        // read flash
        auto retRead = StoreFlashManager::getInstance()->readConfigFromFlash();
        if (retRead)
        {
            gConfigSystem = StoreFlashManager::getInstance()->configSystem();
            LOG_INFO("ConfigSystemTask", "Read config from nvs success");
        }
        else
        {
            LOG_ERROR("ConfigSystemTask", "Read config from nvs error");
        }
    }
    else
    {
        LOG_ERROR("ConfigSystemTask", "Write config from nvs error");
    }
    // response
    CodecMessage codecMsg;
    auto retPack = gConfigSystem.packData(&codecMsg);
    if (retPack)
    {
        sendDataToAppCenter(&codecMsg);
    }
}
