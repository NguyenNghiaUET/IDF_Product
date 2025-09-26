#ifndef COMMON_H_
#define COMMON_H_

#include "stdint.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C"
{
#endif

// === C-compatible section ===
#define MAX_LENGTH_TASK_NAME 16
#define MAX_TASK 10

    void processTimer100Hz();

#ifdef __cplusplus
}
#endif

// === C++-only section ===
#ifdef __cplusplus

#include "OSFreeRTOS.h"
#include "loggermanager.h"
#include <string>
#include "configsystemmessage.h"
#include "flashmanager.h"

#define UDP_LOCAL_PORT_SYSTEM_CONFIG 8080
// Osbase
extern OSBase *mOSBase;

// TaskMonitorSys
extern OSBase::QueueHandle gQueuePowerMonitorToMain;
// Mqtt task
extern OSBase::SemHandle gSemWifiStaConnectedFromWifiTaskToMqttTask;
extern OSBase::SemHandle gSemWifiStaDisConnectedFromWifiTaskToMqttTask;

extern uint64_t gDeviceID;
extern FlashManagerAbstract* gFlashManager;
extern ConfigSystemMessage gConfigSystem;

// Relay status message (send from RelayManagerTask -> WifiManagerTask)
struct RelayStatusMsg
{
    bool isOn;
};
// Queue handle for relay status to wifi task
extern OSBase::QueueHandle gQueueRelayStatusToWifi;
#endif // __cplusplus

#endif /* COMMON_H_ */
