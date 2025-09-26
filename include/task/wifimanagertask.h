#pragma once
#include "taskabstract.h"
#include <string>
#include "wifimanagerabstract.h"
#include "webservermanagerabstract.h"
class WifiManagerTask : public TaskAbstract
{
public:
    static constexpr uint16_t DIV_COUNTER_10_S = 1000; // 10s
    static constexpr uint16_t DIV_COUNTER_10Hz = 10; 
    // wifi
    const std::string SSID_WIFI_STA = "trangtraithongminh";
    const std::string PASS_WIFI_STA = "trangtraithongminh@";
    const std::string SSID_WIFI_AP = "SmartFarm";
    const std::string PASS_WIFI_AP = "123456a@";
    WifiManagerTask(WiFiManagerAbstract *wifimanager, WebServerAbstract* webserver, std::string nameTask, int numElementQueueSet);
    ~WifiManagerTask();

    virtual void onTimer100HzProcess() override;
    virtual void onQueueSetMessageProcess(OSBase::QueueHandle queue_sem) override;
    virtual void onInitProcess() override;

private:
    uint32_t mCounter100Hz{0};
    WiFiManagerAbstract *mWiFiManager{nullptr};
    WebServerAbstract* mWebServer;

    bool mIsWifiStaConnected {false};
};