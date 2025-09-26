#include "MyMain.h"
#include "common/common.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void timer_callback(void *arg)
{
    processTimer100Hz();
}

void app_main()
{ 
    // esp_log_level_set("*", ESP_LOG_DEBUG);  // Hiện toàn bộ log DEBUG trở xuống
    //freertos
    startAllTask();
    vTaskDelay(pdMS_TO_TICKS(1000));
    //timer 100hz
    const esp_timer_create_args_t timer_args = {
        .callback = &timer_callback,
        .name = "100HzTimer"};
    esp_timer_handle_t timer_handle;
    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &timer_handle));
    // 100Hz = 10ms = 10,000 microseconds
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handle, 10000)); // 10,000 µs
    vTaskDelay(pdMS_TO_TICKS(100));
}