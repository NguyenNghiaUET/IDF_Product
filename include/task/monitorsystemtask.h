#pragma once
#include "taskabstract.h"
#include <string>
#include "monitorsystemmessage.h"

extern "C" void* __dso_handle = nullptr;

class MonitorSystemTask : public TaskAbstract
{
public:
    static constexpr uint8_t DIV_COUNTER_10HZ = 10;
    static constexpr uint16_t DIV_COUNTER_0_2_HZ = 500; //5s
    MonitorSystemTask(std::string nameTask, int numElementQueueSet);
    ~MonitorSystemTask();

    virtual void onTimer100HzProcess() override;
    virtual void onQueueSetMessageProcess(OSBase::QueueHandle queue_sem) override;
    virtual void onInitProcess() override;

private:
    uint32_t mCounter100Hz{0};
    MonitorSystemMessage::monitor_system_t mMonitorSystem;
};