#include "pingmessage.h"

PingMessage::PingMessage()
    : AbstractMessage(MessageId::PING_MESSAGE)
{
}

PingMessage::PingMessage(ping_message_t ping)
    : AbstractMessage(MessageId::PING_MESSAGE)
{
    mPingMessage = ping;
}

PingMessage::PingMessage(TypePingMessage typePing, MessageCommon::DEVICE_TYPE type, uint64_t id)
    : AbstractMessage(MessageId::PING_MESSAGE)
{
    mPingMessage.deviceID = id;
    mPingMessage.deviceType = type;
    mPingMessage.typePing = typePing;
}

PingMessage::~PingMessage()
{
}

const PingMessage::ping_message_t &PingMessage::getPingMessage() const
{
    return mPingMessage;
}

void PingMessage::setPingMessage(const ping_message_t &ping)
{
    mPingMessage = ping;
}

bool PingMessage::packData(CodecMessage *msg)
{
    if (msg)
    {
        memcpy(msg->mDataRaw, &mPingMessage, sizeof(ping_message_t));
        msg->mMsgDataLength = sizeof(ping_message_t);
        msg->mMsgID = (uint8_t)this->mMessageId;
        return true;
    }
    else
    {
        return false;
    }
}

bool PingMessage::unpackData(CodecMessage *msg)
{
    if (msg)
    {
        memcpy(&mPingMessage, msg->mDataRaw, sizeof(ping_message_t));
        return true;
    }
    else
    {
        return false;
    }
}
