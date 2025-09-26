#ifndef COMMAND_SYSTEM_MESSAGE_H
#define COMMAND_SYSTEM_MESSAGE_H
#include "abstractmessage.h"

class CommandSystemMessage : public AbstractMessage
{
public:
    enum CommandSystemType : uint8_t
    {
        NONE = 0,
        READ_CONFIG_SYSTEM,
    };

#pragma pack(push, 1)
    typedef struct command_system
    {
        CommandSystemType cmdType{CommandSystemType::NONE};
    } command_system_t;
#pragma pack(pop)

    CommandSystemMessage();
    CommandSystemMessage(command_system_t cmd);
    CommandSystemMessage(CommandSystemType cmdType);
    ~CommandSystemMessage();

    const command_system_t &getCommandSystemMessage() const;
    void setCommandSystemMessage(const command_system_t &cmd);

    virtual bool packData(CodecMessage *msg) override;
    virtual bool unpackData(CodecMessage *msg) override;

private:
    command_system_t mCommandSystem{};
};

#endif