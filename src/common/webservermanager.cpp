// WebServerEsp32.cpp
#include "common/webservermanager.h"
#include "esp_log.h"
#include <cstring>
#include <algorithm>
#include "common/common.h"
#include "common/webserversource.h"
#include "common/storeflashmanager.h"

static const char *TAG = "WebServerEsp32";

WebServerEsp32::WebServerEsp32()
{
}

WebServerEsp32::~WebServerEsp32()
{
    stop();
}

bool WebServerEsp32::start()
{
    if (mServer)
    {
        ESP_LOGW(TAG, "Server already started");
        return true;
    }

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.stack_size = 8192; // Tăng nếu cần nhiều xử lý
    esp_err_t ret = httpd_start(&mServer, &config);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to start server: %s", esp_err_to_name(ret));
        return false;
    }

    registerHandlers();
    ESP_LOGI(TAG, "Web server started");
    return true;
}

void WebServerEsp32::stop()
{
    if (mServer)
    {
        httpd_stop(mServer);
        mServer = nullptr;
        ESP_LOGI(TAG, "Web server stopped");
    }
}

void WebServerEsp32::registerHandlers()
{
    httpd_uri_t root_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = handleRoot,
        .user_ctx = nullptr};
    httpd_register_uri_handler(mServer, &root_uri);

    httpd_uri_t config_uri = {
        .uri = "/config",
        .method = HTTP_POST,
        .handler = handleConfigPost,
        .user_ctx = nullptr};
    httpd_register_uri_handler(mServer, &config_uri);

    httpd_uri_t get_config_uri = {
        .uri = "/get-config",
        .method = HTTP_POST, // ← dùng POST
        .handler = handle_get_config,
        .user_ctx = nullptr};

    httpd_register_uri_handler(mServer, &get_config_uri);
}

esp_err_t WebServerEsp32::handleRoot(httpd_req_t *req)
{
    httpd_resp_send(req, SourceWebserver::html_config, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t WebServerEsp32::handleConfigPost(httpd_req_t *req)
{
    // Đọc dữ liệu POST
    char buf[512];
    int ret = httpd_req_recv(req, buf, sizeof(buf));
    if (ret <= 0)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to receive POST data");
        return ESP_FAIL;
    }
    buf[ret] = '\0'; // Đảm bảo kết thúc chuỗi
    ESP_LOGI(TAG, "Received config: %s", buf);
    // Parse dữ liệu dạng: ssid=abc&pass=123456&uriMqtt=...&portM=8883&mqttUser=...&mqttPass=...
    // Todo: giới hạn độ dài trên giao diện nhập
    char ssid[32] = "";
    char pass[32] = "";
    char uriMqtt[96] = "";
    char portStr[6] = "";
    char mqttUser[32] = "";
    char mqttPass[32] = "";
    // Dùng sscanf hoặc tự parse
    httpd_query_key_value(buf, "ssid", ssid, sizeof(ssid));
    httpd_query_key_value(buf, "pass", pass, sizeof(pass));
    httpd_query_key_value(buf, "uriMqtt", uriMqtt, sizeof(uriMqtt));
    httpd_query_key_value(buf, "portMqtt", portStr, sizeof(portStr));
    httpd_query_key_value(buf, "mqttUser", mqttUser, sizeof(mqttUser));
    httpd_query_key_value(buf, "mqttPass", mqttPass, sizeof(mqttPass));

    // Gán vào struct
    gConfigSystem.setSsid(ssid);
    gConfigSystem.setPass(pass);
    gConfigSystem.setUriMqtt(uriMqtt);
    gConfigSystem.setMqttUser(mqttUser);
    gConfigSystem.setPass(pass);
    gConfigSystem.setPortMqtt(atoi(portStr));
    // store config
    auto status = StoreFlashManager::getInstance()->saveConfigToFlash(gConfigSystem);
    if (!status)
    {
        LOG_ERROR("WebServerEsp32", "Save config to flash error");
    }

    // Phản hồi
    httpd_resp_sendstr(req, "✅ Configuration saved successfully!");
    return ESP_OK;
}

esp_err_t WebServerEsp32::handle_get_config(httpd_req_t *req)
{
    // Tạo JSON thủ công
    char response[512];
    snprintf(response, sizeof(response),
             "{"
             "\"ssid\":\"%s\","
             "\"pass\":\"%s\","
             "\"uriMqtt\":\"%s\","
             "\"portMqtt\":\"%u\","
             "\"mqttUser\":\"%s\","
             "\"mqttPass\":\"%s\""
             "}",
             gConfigSystem.getConfigSystem().ssidWifiSta,
             gConfigSystem.getConfigSystem().passWifiSta,
             gConfigSystem.getConfigSystem().uriMqtt,
             gConfigSystem.getConfigSystem().portMqtt,
             gConfigSystem.getConfigSystem().mqttUser,
             gConfigSystem.getConfigSystem().mqttPass);

    // Gửi header JSON
    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, response, HTTPD_RESP_USE_STRLEN);
}