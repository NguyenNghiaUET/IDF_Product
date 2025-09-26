#include "common/common.h"
#include "taskmanager.h"
#include "flashmanager.h"
#include "common.h"
#include "common/storeflashmanager.h"
#include <inttypes.h>
#include "esp_mac.h"

// Init param
OSBase *mOSBase;
// TaskMonitorSys
OSBase::SemHandle gMonitorSystemSemaphore;
OSBase::SemHandle gQueuePowerMonitorToMain;
OSBase::QueueSetHandle gPowerMonitorQueueSet;
// Mqtt task
OSBase::SemHandle gSemWifiStaConnectedFromWifiTaskToMqttTask;
OSBase::SemHandle gSemWifiStaDisConnectedFromWifiTaskToMqttTask;
// Relay -> Wifi queue
OSBase::QueueHandle gQueueRelayStatusToWifi;

uint64_t gDeviceID;
// object
ConfigSystemMessage gConfigSystem;
FlashManagerAbstract *gFlashManager = new FlashManager();
// Init function

void processTimer100Hz()
{
    TaskManager::getInstance()->onTimer100Hz();
}

// get ID
uint64_t device_id48_u64(void)
{
    uint8_t mac[6];
    esp_efuse_mac_get_default(mac); // Factory MAC duy nhất
    uint64_t id = 0;
    for (int i = 0; i < 6; ++i)
    {
        id = (id << 8) | mac[i];
    }
    return id; // 48-bit nằm ở phần thấp của u64
}

void initSystem()
{
    // get OSBase
    mOSBase = TaskManager::getInstance()->getOSBase();
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

    // init device id
    gDeviceID = device_id48_u64();
}