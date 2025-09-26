#include "monitorsystemmessage.h"

MonitorSystemMessage::MonitorSystemMessage(monitor_system_t monitor)
	: AbstractMessage(MessageId::MONITOR_SYSTEM_ECU_MESSAGE)
{
	mMonitor = monitor;
}

MonitorSystemMessage::~MonitorSystemMessage()
{
}

const MonitorSystemMessage::monitor_system_t &MonitorSystemMessage::getMonitor() const
{
	return mMonitor;
}

void MonitorSystemMessage::setMonitor(const monitor_system_t &mMonitor)
{
	this->mMonitor = mMonitor;
}
bool MonitorSystemMessage::packData(CodecMessage *msg)
{
	if (msg)
	{
		memcpy(msg->mDataRaw, &mMonitor, sizeof(monitor_system_t));
		msg->mMsgDataLength = sizeof(monitor_system_t);
		msg->mMsgID = (uint8_t)this->mMessageId;
		return true;
	}
	else
	{
		return false;
	}
}

MonitorSystemMessage::MonitorSystemMessage()
	: AbstractMessage(MessageId::MONITOR_SYSTEM_ECU_MESSAGE)
{
}

bool MonitorSystemMessage::unpackData(CodecMessage *msg)
{
	if (msg)
	{
		memcpy(&mMonitor, msg->mDataRaw, sizeof(monitor_system_t));
		return true;
	}
	else
	{
		return false;
	}
}
