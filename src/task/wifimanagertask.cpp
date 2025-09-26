#include "task/wifimanagertask.h"
#include "loggermanager.h"
#include "common/common.h"

WifiManagerTask::WifiManagerTask(WiFiManagerAbstract *wifimanager,  WebServerAbstract* webserver,std::string nameTask, int numElementQueueSet)
    : TaskAbstract(nameTask, numElementQueueSet)
{
    mWiFiManager = wifimanager;
    mWebServer = webserver;
}

WifiManagerTask::~WifiManagerTask()
{
}

void WifiManagerTask::onTimer100HzProcess()
{
    mCounter100Hz++;
    if (mCounter100Hz % DIV_COUNTER_10_S == 0)
    {
        if (mWiFiManager->isWifiConnected() == false)
        {
            LOG_DEBUG("MonitorSystemTask", "Reconnect to wifi: ssid: %s, pass: %s",
                      mWiFiManager->getSsidWifiSTA().c_str(), mWiFiManager->getPassWifiSTA().c_str());
            mWiFiManager->reconnectWifi();
        }
    }
    if (mCounter100Hz % DIV_COUNTER_10Hz == 0)
    {
        if (mIsWifiStaConnected != mWiFiManager->isWifiConnected())
        {
            mIsWifiStaConnected = mWiFiManager->isWifiConnected();
            if (mIsWifiStaConnected == true)
            {
                LOG_INFO("WifiManagerTask", "Wifi sta connected -> send event to mqtt task");
                mOSBase->semGive(gSemWifiStaConnectedFromWifiTaskToMqttTask);
                mWebServer->start();
            }
            else
            {
                LOG_INFO("WifiManagerTask", "Wifi sta disconnected -> send event to mqtt task");
                mOSBase->semGive(gSemWifiStaDisConnectedFromWifiTaskToMqttTask);
                mWebServer->stop();
            }
        }
    }
}

void WifiManagerTask::onQueueSetMessageProcess(OSBase::QueueHandle queue_sem)
{
    if (queue_sem == gQueueRelayStatusToWifi)
    {
        RelayStatusMsg msg{};
        if (mOSBase->queueReceive(gQueueRelayStatusToWifi, &msg) == OSBase::QUEUE_OK)
        {
            LOG_INFO("WifiManagerTask", "Relay status: %s", msg.isOn ? "ON" : "OFF");
            // TODO: publish via webserver or update internal state if needed
        }
        return;
    }
}

void WifiManagerTask::onInitProcess()
{
    LOG_INFO("WifiManagerTask", "Init task");
    // register queue to queueset to receive relay status (queue created in startAllTask)
    initregisterQueueToQueueset(&gQueueRelayStatusToWifi, sizeof(RelayStatusMsg), 5);
    WiFiManagerAbstract::STAConfig staConfig;
    staConfig.ssid = SSID_WIFI_STA;
    staConfig.password = PASS_WIFI_STA;

    WiFiManagerAbstract::APConfig apConfig;
    apConfig.ssid = SSID_WIFI_AP;
    apConfig.password = PASS_WIFI_AP;

    mWiFiManager->initWifi(WiFiManagerAbstract::ModeWifi::MODE_AP_STA, staConfig, apConfig);
}
