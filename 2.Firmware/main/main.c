/*
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-09-04 16:11:59
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2022-01-19 11:53:59
 * @FilePath: \3.lvgl_v8\main\main.c
 */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"


#include "esp_freertos_hooks.h"
#include "esp_system.h"
#include "nvs_flash.h"


#include "gui_task.h"
#include "encoder.h"
#include "user_smartconfig.h"
#include "user_test_case.h"
#include "tc_parse_ohm.h"



void app_main(void)
{
    /* Initialize NVS */
    esp_err_t ret = nvs_flash_init();
    if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    /* Initialize WiFi */
    wifi_init();

    /* LVGL */
    xTaskCreatePinnedToCore(gui_task, "gui task", 1024 * 4, NULL, 2, NULL, 0);
    /* Encoder */
    // xTaskCreate(encoder_task, "encoder_task", 2048, NULL, 3, NULL);
    /* OHM data parase */
    xTaskCreate(tc_ohm_data_refresh, "tc_ohm_data_refresh", 4096, NULL, 5, NULL);
}
