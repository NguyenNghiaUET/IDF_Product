#ifndef PING_MESSAGE_H
#define PING_MESSAGE_H
#include "abstractmessage.h"
#include "messagecommon.h"

class PingMessage : public AbstractMessage
{
public:
    enum TypePingMessage : uint8_t
    {
        BroadCast = 0,
        Request,
        Response
    };
#pragma pack(push, 1)
    typedef struct ping_message
    {
        TypePingMessage typePing{TypePingMessage::Request};
        MessageCommon::DEVICE_TYPE deviceType{MessageCommon::DEVICE_TYPE::AquaControlDevice};
        uint64_t deviceID{0};
    } ping_message_t;
#pragma pack(pop)

    PingMessage();
    PingMessage(ping_message_t ping);
    PingMessage(TypePingMessage typePing, MessageCommon::DEVICE_TYPE type, uint64_t id);
    ~PingMessage();

    const ping_message_t &getPingMessage() const;
    void setPingMessage(const ping_message_t &pingMessage);

    virtual bool packData(CodecMessage *msg) override;
    virtual bool unpackData(CodecMessage *msg) override;

private:
    ping_message_t mPingMessage{};
};

#endif