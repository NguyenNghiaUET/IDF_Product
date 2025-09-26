#include "commandsystemmessage.h"

CommandSystemMessage::CommandSystemMessage()
    : AbstractMessage(MessageId::COMMAND_SYSTEM_MESSAGE)
{
}

CommandSystemMessage::CommandSystemMessage(command_system_t cmd)
    : AbstractMessage(MessageId::COMMAND_SYSTEM_MESSAGE), mCommandSystem(cmd)
{
}

CommandSystemMessage::CommandSystemMessage(CommandSystemType cmdType)
    : AbstractMessage(MessageId::COMMAND_SYSTEM_MESSAGE)
{
    mCommandSystem.cmdType = cmdType;
}

CommandSystemMessage::~CommandSystemMessage()
{
}

const CommandSystemMessage::command_system_t &CommandSystemMessage::getCommandSystemMessage() const
{
    return mCommandSystem;
}

void CommandSystemMessage::setCommandSystemMessage(const command_system_t &cmd)
{
    mCommandSystem = cmd;
}

bool CommandSystemMessage::packData(CodecMessage *msg)
{
    if (msg)
    {
        memcpy(msg->mDataRaw, &mCommandSystem, sizeof(command_system_t));
        msg->mMsgDataLength = sizeof(command_system_t);
        msg->mMsgID = (uint8_t)this->mMessageId;
        return true;
    }
    else
    {
        return false;
    }
}

bool CommandSystemMessage::unpackData(CodecMessage *msg)
{
    if (msg)
    {
        memcpy(&mCommandSystem, msg->mDataRaw, sizeof(command_system_t));
        return true;
    }
    else
    {
        return false;
    }
}
