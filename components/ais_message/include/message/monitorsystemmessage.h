#ifndef SUBMODULE_VTJC2_MESSAGE_MESSAGE_INC_ECUSENSORMONITORSYSTEMMESSAGE_H_
#define SUBMODULE_VTJC2_MESSAGE_MESSAGE_INC_ECUSENSORMONITORSYSTEMMESSAGE_H_
#include "abstractmessage.h"
#include "stdint.h"
#include "monitorcodec.h"

class MonitorSystemMessage: public AbstractMessage
{
public:
	static constexpr uint8_t MAX_TASK_MONITOR_TO_MAIN_ECU = 10;
	

#pragma pack(push, 1)
	typedef struct monitor_system_ecu_sensor
	{
		uint8_t taskID[MAX_TASK_MONITOR_TO_MAIN_ECU];
		uint8_t cpuUsage[MAX_TASK_MONITOR_TO_MAIN_ECU];
		uint8_t stackUsage[MAX_TASK_MONITOR_TO_MAIN_ECU];
		uint8_t numTaskCurrent;
		uint8_t heapUsage;
		monitor_codec_t monitorCodecPdu;
		uint64_t errorCode {0};
	} monitor_system_t;
#pragma pack(pop)
    MonitorSystemMessage();
	MonitorSystemMessage(monitor_system_t monitor);
	virtual ~MonitorSystemMessage();

	const monitor_system_t& getMonitor() const;
	void setMonitor(const monitor_system_t &mMonitor);

	virtual bool packData(CodecMessage *msg) override;
	virtual bool unpackData(CodecMessage *msg) override;

private:
    monitor_system_t mMonitor {};
};

#endif /* SUBMODULE_VTJC2_MESSAGE_MESSAGE_INC_ECUSENSORMONITORSYSTEMMESSAGE_H_ */
