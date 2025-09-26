#pragma once
#include "taskabstract.h"
#include <string>

class RelayManagerTask : public TaskAbstract
{
public:
    static constexpr uint16_t DIV_COUNTER_1Hz = 100;   // 1 Hz at 100 Hz tick
    static constexpr uint16_t DIV_COUNTER_2Hz = 50;    // 2 Hz at 100 Hz tick

    RelayManagerTask(std::string nameTask, int numElementQueueSet);
    ~RelayManagerTask();

    virtual void onTimer100HzProcess() override;
    virtual void onQueueSetMessageProcess(OSBase::QueueHandle queue_sem) override;
    virtual void onInitProcess() override;

private:
    uint32_t mCounter100Hz{0};
    bool mRelayOn{false};
};


