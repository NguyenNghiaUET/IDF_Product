#include "task/monitorsystemtask.h"
#include "loggermanager.h"
#include "common/common.h"

MonitorSystemTask::MonitorSystemTask(std::string nameTask, int numElementQueueSet)
    : TaskAbstract(nameTask, numElementQueueSet)
{
}

MonitorSystemTask::~MonitorSystemTask()
{
}

void MonitorSystemTask::onTimer100HzProcess()
{
    // 10 Hz debug (tắt)
    // if (mCounter100Hz % DIV_COUNTER_10HZ == 0) {}

    // (đã di chuyển logic relay sang RelayManagerTask)
    if (mCounter100Hz % DIV_COUNTER_0_2_HZ == 0)
    {
        OSBase::task_monitor_infor monitorInfor[OSBase::MAX_TASK_MONITOR];
        uint8_t heapUsage = 0;
        uint8_t cpuUsageIdle = 0;
        uint8_t numTaskMonitor = mOSBase->getAllTaskInfor(monitorInfor, OSBase::MAX_TASK_MONITOR, &heapUsage, &cpuUsageIdle);
        mMonitorSystem.heapUsage = heapUsage;
        LOG_DEBUG("MonitorSystemTask", "HeapUsage: %d, TotalCpu: %d", heapUsage, 100 - cpuUsageIdle);
        if (numTaskMonitor > 0 && numTaskMonitor < MonitorSystemMessage::MAX_TASK_MONITOR_TO_MAIN_ECU)
        {
            mMonitorSystem.numTaskCurrent = numTaskMonitor;
            for (int i = 0; i < numTaskMonitor; i++)
            {
                mMonitorSystem.cpuUsage[i] = monitorInfor[i].cpuUsage;
                mMonitorSystem.stackUsage[i] = monitorInfor[i].stackUsage;
                mMonitorSystem.taskID[i] = monitorInfor[i].idTask;
                LOG_DEBUG("MonitorSystemTask", "[Task %d]Cpu: %d, stack: %d", monitorInfor[i].idTask, monitorInfor[i].cpuUsage, monitorInfor[i].stackUsage);
            }
            // update total cpu
            mMonitorSystem.cpuUsage[numTaskMonitor] = cpuUsageIdle;
        }
    }
    mCounter100Hz++;
}

void MonitorSystemTask::onQueueSetMessageProcess(OSBase::QueueHandle queue_sem)
{
    LOG_INFO("MonitorSystemTask", "has data");
}

void MonitorSystemTask::onInitProcess()
{
    LOG_INFO("MonitorSystemTask", "Init task");
}
