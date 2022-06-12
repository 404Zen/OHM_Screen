/***********************************************************************
 * @file       user_smartconfig.c
 * @brief      user_smartconfig c file
 * @version    v0.0.1
 * @date       2022-05-28
 *
 * Copyright(C) 2022 .404Zen. all right reserved
***********************************************************************/

/*Include-------------------------------------------------------------*/
#include "user_smartconfig.h" 

#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_wifi.h"
// #include "esp_wpa2.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_netif.h"
#include "esp_smartconfig.h"

#include "nvs_flash.h"


/*Defines-------------------------------------------------------------*/
#define TAG                             "USER_SC"

/* 定义wifi更新标识符，WiFi SSID 与 PWD */
#define DEFAULT_WIFI_UPDATE             4096
#define DEFAULT_WIFI_SSID               "ssid"
#define DEFAULT_WIFI_PASSWD             "password"

/* 联网事件标志位 */
#define WIFI_CONNECTED_BIT              BIT0
#define WIFI_FAIL_BIT                   BIT1
#define SMART_CONFIG_BIT                BIT2

/* WIFI连接事件标志组 */
static EventGroupHandle_t s_wifi_event_group;

/*Function prototypes-------------------------------------------------*/
static void new_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
static void smartconfig_init_start(void);
void got_ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

/*User code-----------------------------------------------------------*/

 /**
  * @brief  got_ip_event_handler
  * @note   None.
  * @param  None.
  * @retval None.
  */
void got_ip_event_handler(void *arg, esp_event_base_t event_base,
                                 int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
    const esp_netif_ip_info_t *ip_info = &event->ip_info;

    #if 1
    ESP_LOGI(TAG, "Got IP Address");
    ESP_LOGI(TAG, "~~~~~~~~~~~~~~~~");
    ESP_LOGI(TAG, "IP:" IPSTR, IP2STR(&ip_info->ip));
    ESP_LOGI(TAG, "MASK:" IPSTR, IP2STR(&ip_info->netmask));
    ESP_LOGI(TAG, "GW:" IPSTR, IP2STR(&ip_info->gw));
    ESP_LOGI(TAG, "~~~~~~~~~~~~~~~~");
    #endif
}

 /**
* @brief  wifi_init
* @note   None.
* @param  None.
* @retval None.
*/
void wifi_init(void)
{
    esp_err_t err = 0;

    /* nvs操作句柄 */
    nvs_handle wificfg_nvs_handler;
    /* 打开一个NVS命名空间 */
    ESP_ERROR_CHECK(nvs_open("wifi_cfg", NVS_READWRITE, &wificfg_nvs_handler));

    /* 获取wifi更新标志 */
    uint32_t wifi_update = 0;
    err = nvs_get_u32(wificfg_nvs_handler, "wifi_update", &wifi_update);

    if(wifi_update == DEFAULT_WIFI_UPDATE)
    {
        ESP_LOGI(TAG, "WiFi config data needn't update\r\n");
    }
    else
    {
        ESP_LOGI(TAG, "WiFi config data updating...\r\n");
        ESP_ERROR_CHECK( nvs_set_str(wificfg_nvs_handler, "wifi_ssid", DEFAULT_WIFI_SSID) );
        ESP_ERROR_CHECK( nvs_set_str(wificfg_nvs_handler, "wifi_pwd", DEFAULT_WIFI_PASSWD) );
        ESP_ERROR_CHECK( nvs_set_u32(wificfg_nvs_handler, "wifi_update", DEFAULT_WIFI_UPDATE) );
        ESP_LOGI(TAG, "WiFi config data update ok...\r\n");
    }

    ESP_ERROR_CHECK( nvs_commit(wificfg_nvs_handler) );     /* 提交NVS数据,保存到flash中 */
    nvs_close(wificfg_nvs_handler);                         /* 关闭NVS操作空间 */


    /* wifi station mode */
    ESP_LOGI(TAG, "WiFi Init Station Mode! \r\n");

    // Initialize TCP/IP network interface (should be called only once in application)
    ESP_ERROR_CHECK(esp_netif_init());
    s_wifi_event_group = xEventGroupCreate();               //创建事件组

    ESP_ERROR_CHECK(esp_event_loop_create_default());       //esp loop
    
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );

    //事件注册
    // ESP_ERROR_CHECK( esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL) );
    // ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL) );
    // ESP_ERROR_CHECK( esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL) );

    ESP_ERROR_CHECK( esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &new_event_handler, NULL) );
    ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &new_event_handler, NULL) );
    ESP_ERROR_CHECK( esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &new_event_handler, NULL) );

    /* 从NVS中读取WiFi SSID PASSWD等 */
    char wifi_ssid[32] = {0};
    char wifi_pwd[64] = {0};
    size_t len;
    ESP_ERROR_CHECK(nvs_open("wifi_cfg", NVS_READWRITE, &wificfg_nvs_handler));
    len = sizeof(wifi_ssid);
    ESP_ERROR_CHECK( nvs_get_str(wificfg_nvs_handler, "wifi_ssid", wifi_ssid, &len) );
    len = sizeof(wifi_pwd);
    ESP_ERROR_CHECK( nvs_get_str(wificfg_nvs_handler, "wifi_pwd", wifi_pwd, &len) );
    ESP_ERROR_CHECK( nvs_commit(wificfg_nvs_handler) );     /* 提交NVS数据 */
    nvs_close(wificfg_nvs_handler);                         /* 关闭NVS操作空间 */

    /* 复制wifi连接参数 */
    wifi_config_t wifi_config;
    bzero(&wifi_config, sizeof(wifi_config_t));
    memcpy(wifi_config.sta.ssid, wifi_ssid, sizeof(wifi_config.sta.ssid));
    memcpy(wifi_config.sta.password, wifi_pwd, sizeof(wifi_config.sta.password));


    ESP_LOGI(TAG, "NVS SSID:%s", wifi_ssid);
    ESP_LOGI(TAG, "NVS PWD:%s", wifi_pwd);

    /* 设置为STA模式 */
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    /* 设置wifi连接参数 */
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    /* 启动wifi连接 */
    ESP_ERROR_CHECK( esp_wifi_start() );

    ESP_LOGI(TAG, "WiFi config done! connecting... \r\n");
    
    
    /* 等待连接建立事件 */  
    EventBits_t bits;

    bits = xEventGroupWaitBits( s_wifi_event_group, 
                                WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                pdFALSE,
                                pdFALSE,
                                portMAX_DELAY   );

    if(bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI(TAG, "WiFi connected! \r\n");
        // vEventGroupDelete(s_wifi_event_group);
    }
    else if(bits & WIFI_FAIL_BIT)
    {
        ESP_LOGI(TAG, "WiFi connecte fail... Start smart config! \r\n");
        smartconfig_init_start();
    }
    else
    {
        ESP_LOGI(TAG, "WiFi connecte fail... Start smart config! \r\n");
        smartconfig_init_start();
    }

    /* Set current WiFi power save type */
    // ESP_ERROR_CHECK( esp_wifi_set_ps(WIFI_PS_MIN_MODEM) ); //must call this
    // ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    // ESP_ERROR_CHECK( esp_wifi_start() );


    (void)err;
}


bool is_wifi_connected(void)
{
    EventBits_t bits;
    bits = xEventGroupGetBits(s_wifi_event_group);

    if(bits & WIFI_CONNECTED_BIT)
    {
        return true;
    }
    return false;
}


 /**
  * @brief  new_event_handler
  * @note   wifi 事件处理函数.
  * @param  None.
  * @retval None.
  */
static void new_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    static uint32_t retry = 0;
    
    if(event_base == WIFI_EVENT)
    {
        if(event_id == WIFI_EVENT_STA_START)
        {
            esp_wifi_connect();
        }
        else if(event_id == WIFI_EVENT_STA_DISCONNECTED)
        {
            esp_wifi_connect();
            retry++;

            if(retry >= 10)
            {
                ESP_LOGI(TAG, "WiFi Connect fail...\r\n");
                xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
            }

            xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        }
    }
    else if(event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        retry = 0;
        
        ip_event_got_ip_t   *event = (ip_event_got_ip_t *)event_data;
        printf("Got IP: %d.%d.%d.%d\r\n", IP2STR(&event->ip_info.ip) );
        ESP_LOGI(TAG, "WiFi Connected to ap");
        
        got_ip_event_handler(NULL, event_base, event_id, event);
        
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
    else if(event_base == SC_EVENT)
    {
        if(event_id == SC_EVENT_SCAN_DONE)
        {
            ESP_LOGI(TAG, "SmartConfig Scan done.\r\n");
        }
        else if(event_id == SC_EVENT_FOUND_CHANNEL)
        {
            ESP_LOGI(TAG, "SmartConfig Found Channel.\r\n");
        }
        else if(event_id == SC_EVENT_GOT_SSID_PSWD)
        {
            ESP_LOGI(TAG, "SmartConfig Got SSID & PWD.\r\n");

            smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t*)event_data;
            wifi_config_t wifi_config;
            char ssid[32] = { 0 };
            char password[64] = { 0 };
            uint8_t rvd_data[33] = { 0 };

            bzero(&wifi_config, sizeof(wifi_config_t));
            memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
            memcpy(wifi_config.sta.password, evt->password, sizeof(wifi_config.sta.password));
            wifi_config.sta.bssid_set = evt->bssid_set;
            if (wifi_config.sta.bssid_set == true) 
            {
                memcpy(wifi_config.sta.bssid, evt->bssid, sizeof(wifi_config.sta.bssid));
            }

            memcpy(ssid, evt->ssid, sizeof(evt->ssid));
            memcpy(password, evt->password, sizeof(evt->password));
            ESP_LOGI(TAG, "SSID:%s", ssid);
            ESP_LOGI(TAG, "PASSWORD:%s", password);

            if (evt->type == SC_TYPE_ESPTOUCH_V2) 
            {
                ESP_ERROR_CHECK( esp_smartconfig_get_rvd_data(rvd_data, sizeof(rvd_data)) );
                ESP_LOGI(TAG, "RVD_DATA:");
                for (int i=0; i<33; i++) {
                    printf("%02x ", rvd_data[i]);
                }
                printf("\n");
            }

            /* 保存SSID和PWD */
            nvs_handle wificfg_nvs_handler;
            ESP_ERROR_CHECK( nvs_open("wifi_cfg", NVS_READWRITE, &wificfg_nvs_handler) );
            ESP_ERROR_CHECK( nvs_set_str(wificfg_nvs_handler, "wifi_ssid", ssid) );
            ESP_ERROR_CHECK( nvs_set_str(wificfg_nvs_handler, "wifi_pwd", password) );
            ESP_ERROR_CHECK( nvs_commit(wificfg_nvs_handler) );
            nvs_close(wificfg_nvs_handler);
            ESP_LOGI(TAG, "SmartConfig Data save to NVS.\r\n");


            /* 连接wifi */
            ESP_ERROR_CHECK( esp_wifi_disconnect() );
            ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
            esp_wifi_connect();

        }
        else if(event_id == SC_EVENT_SEND_ACK_DONE) 
        {
            xEventGroupSetBits(s_wifi_event_group, SMART_CONFIG_BIT);
        }
    }
}



static void smartconfig_init_start(void)
{
    EventBits_t uxBits;

    ESP_ERROR_CHECK( esp_smartconfig_set_type(SC_TYPE_ESPTOUCH_AIRKISS) );
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();

    ESP_ERROR_CHECK( esp_smartconfig_start(&cfg) );

    ESP_LOGI(TAG, "Smartconfig start...\r\n");

    uxBits = xEventGroupWaitBits(   s_wifi_event_group, 
                                    WIFI_CONNECTED_BIT | SMART_CONFIG_BIT,
                                    true,
                                    false,
                                    portMAX_DELAY
                                    );

    if(uxBits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI(TAG, "WiFi Connected to ap");
        esp_smartconfig_stop();
    }
    
    if(uxBits & SMART_CONFIG_BIT)
    {
        ESP_LOGI(TAG, "smartconfig over");
        esp_smartconfig_stop();
    }
}


/***** END OF FILE *****/
