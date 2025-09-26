#include "task/relaymanagertask.h"
#include "common/common.h"
#include "loggermanager.h"

RelayManagerTask::RelayManagerTask(std::string nameTask, int numElementQueueSet)
    : TaskAbstract(nameTask, numElementQueueSet)
{
}

RelayManagerTask::~RelayManagerTask()
{
}

void RelayManagerTask::onTimer100HzProcess()
{
    // 1 Hz: toggle relay state
    if (mCounter100Hz % DIV_COUNTER_1Hz == 0)
    {
        mRelayOn = !mRelayOn;
        LOG_INFO("RelayManagerTask", "Toggle relay: %s", mRelayOn ? "ON" : "OFF");
        // TODO: actual relay GPIO control via HAL
    }

    // 2 Hz: send status to WifiManagerTask via queue
    if (mCounter100Hz % DIV_COUNTER_2Hz == 0)
    {
        RelayStatusMsg msg{mRelayOn};
        if (gQueueRelayStatusToWifi)
        {
            auto ret = mOSBase->queueSend(gQueueRelayStatusToWifi, &msg);
            if (ret != OSBase::QUEUE_OK)
            {
                LOG_ERROR("RelayManagerTask", "queueSend relay->wifi failed");
            }
        }
    }

    mCounter100Hz++;
}

void RelayManagerTask::onQueueSetMessageProcess(OSBase::QueueHandle queue_sem)
{
    // No queue members for now
}

void RelayManagerTask::onInitProcess()
{
    LOG_INFO("RelayManagerTask", "Init task");
}


