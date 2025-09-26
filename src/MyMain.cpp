/*
 * myMain.cpp
 *
 *  Created on: May 13, 2024
 *      Author: tungtd10
 */

#include "myMain.h"
#include "task/monitorsystemtask.h"
#include "task/wifimanagertask.h"
#include "taskmanager.h"
#include "task/mqttcommunicationtask.h"
#include "task/configsystemtask.h"
#include "task/relaymanagertask.h"
 

#include "common/common.h"
#include "HAL/HAL_ESP32/wifimanager.h"
#include "HAL/HAL_ESP32/mqttclientesp32.h"
#include "common/webservermanager.h"
#include "common/storeflashmanager.h"
#include "codec/codecesp32comunication.h"
#include "udpesp32.h"

// id
#define ID_MONITOR_SYS_TASK 0
const std::string MONITOR_SYSTEM_TASKNAME = "MonitorSysTask";
const uint8_t MaxElementQueueSetTaskMonitorSystem = 5;
MonitorSystemTask *mMonitorTask{nullptr};

// wifi
#define ID_WIFI_MANAGER_TASK 1
const std::string WIFI_MANAGER_TASKNAME = "WifiManagerTask";
const uint8_t MaxElementQueueSetTaskWifiManager = 5;
WifiManagerTask *mWifiManagerTask{nullptr};
WiFiManagerAbstract *mWifiManagerAbs{nullptr};
WebServerAbstract *WebServerAbs{nullptr};

// mqtt
#define ID_MQTT_TASK 2
const std::string MQTT_TASKNAME = "MqttTask";
const uint8_t MaxElementQueueSetTaskMqtt = 20;
MqttClientAbstract *mMqttClientAbstract{nullptr};
MqttCommunicationTask *mMqttCommunicationTask{nullptr};

// config system
#define ID_CONFIG_SYSTEM_TASK 2
const std::string CONFIG_SYSTEM_TASKNAME = "ConfigSystemTask";
const uint8_t MaxElementQueueSetTaskConfigSystem = 10;
ConfigSystemTask *mConfigSystemTask{nullptr};

// relay manager
#define ID_RELAY_MANAGER_TASK 3
const std::string RELAY_MANAGER_TASKNAME = "RelayManagerTask";
const uint8_t MaxElementQueueSetTaskRelayManager = 5;
RelayManagerTask *mRelayManagerTask{nullptr};

// relay manager removed (merge into MonitorSystemTask)

void StartMonitorSystemTask(void const *argument)
{
    if (mMonitorTask)
    {
        mMonitorTask->onStartProcess();
    }
}

void StartWifiMamnagerTask(void const *argument)
{
    if (mWifiManagerTask)
    {
        mWifiManagerTask->onStartProcess();
    }
}

void StartMqttTask(void const *argument)
{
    if (mMqttCommunicationTask)
    {
        mMqttCommunicationTask->onStartProcess();
    }
}

void StartConfigSytstemTask(void const *argument)
{
    if (mConfigSystemTask)
    {
        mConfigSystemTask->onStartProcess();
    }
}

void StartRelayManagerTask(void const *argument)
{
    if (mRelayManagerTask)
    {
        mRelayManagerTask->onStartProcess();
    }
}

 
/*


OSBase = cục gạch thật sự (object).

getOSBase() = đưa cho bạn địa chỉ của cục gạch.

mOSBase = cái tay bạn giữ địa chỉ đó.

Khi cần, bạn dùng cái tay (mOSBase) để gọi chức năng của cục gạch (OSBase).

*/
void startAllTask()
{
    LOG_INFO("MyMain", "************Start init system and run task****************");
    // get OSBase
    mOSBase = TaskManager::getInstance()->getOSBase();
    // create inter-task queues/semaphores
    gQueueRelayStatusToWifi = mOSBase->queueCreate(5, sizeof(RelayStatusMsg));
    // read config
    auto retSysConfig = StoreFlashManager::getInstance()->readConfigFromFlash();
    if (!retSysConfig)
    {
        LOG_ERROR("MyMain", "Read Config from flash error");
    }
    else
    {
        // update config from flash
        gConfigSystem = StoreFlashManager::getInstance()->configSystem();
    }
    // todo: read status relay, timer

    // init task object
    mMonitorTask = new MonitorSystemTask(MONITOR_SYSTEM_TASKNAME, MaxElementQueueSetTaskMonitorSystem);
    // wifi + web server
    mWifiManagerAbs = new WiFiManagerESP32();
    WebServerAbs = new WebServerEsp32();
    mWifiManagerTask = new WifiManagerTask(mWifiManagerAbs, WebServerAbs, WIFI_MANAGER_TASKNAME, MaxElementQueueSetTaskWifiManager);

    // mqtt task
    mMqttClientAbstract = new MqttClientEsp32("mqtts://f49a54591886417d8ed6a9ea64d0041f.s1.eu.hivemq.cloud", 8883, "phamngoccanh", "123456aA@");
    mMqttCommunicationTask = new MqttCommunicationTask(mMqttClientAbstract, MQTT_TASKNAME, MaxElementQueueSetTaskMqtt);
    mMqttCommunicationTask->initRegisterSemaphoreToQueueset(&gSemWifiStaConnectedFromWifiTaskToMqttTask);
    mMqttCommunicationTask->initRegisterSemaphoreToQueueset(&gSemWifiStaDisConnectedFromWifiTaskToMqttTask);

    // config system task
    auto codecConfigSystem = new CodecEsp32Comunication();
    auto udpConnection = new UdpEsp32(UDP_LOCAL_PORT_SYSTEM_CONFIG);
    mConfigSystemTask = new ConfigSystemTask(CONFIG_SYSTEM_TASKNAME, MaxElementQueueSetTaskConfigSystem,
                                             codecConfigSystem, udpConnection);

    // relay manager task
    mRelayManagerTask = new RelayManagerTask(RELAY_MANAGER_TASKNAME, MaxElementQueueSetTaskRelayManager);

    // relay manager logic handled in MonitorSystemTask

    // init task
    if (mOSBase->taskCreate((char *)MONITOR_SYSTEM_TASKNAME.c_str(), (TaskProc)StartMonitorSystemTask, OSBase::PRIORITY_NORMAL, 4096, ID_MONITOR_SYS_TASK) &&
        mOSBase->taskCreate((char *)WIFI_MANAGER_TASKNAME.c_str(), (TaskProc)StartWifiMamnagerTask, OSBase::PRIORITY_ABOVENORMAL, 4096, ID_WIFI_MANAGER_TASK) &&
        mOSBase->taskCreate((char *)MQTT_TASKNAME.c_str(), (TaskProc)StartMqttTask, OSBase::PRIORITY_HIGH, 4096, ID_MQTT_TASK) &&
        mOSBase->taskCreate((char *)CONFIG_SYSTEM_TASKNAME.c_str(), (TaskProc)StartConfigSytstemTask, OSBase::PRIORITY_HIGH, 4096, ID_CONFIG_SYSTEM_TASK) &&
        mOSBase->taskCreate((char *)RELAY_MANAGER_TASKNAME.c_str(), (TaskProc)StartRelayManagerTask, OSBase::PRIORITY_NORMAL, 4096, ID_RELAY_MANAGER_TASK))
    {
        LOG_INFO("MyMain", "Start All Task Success");
    }
    else
    {
        LOG_ERROR("MyMain", "Start All Task Error");
    }
}
