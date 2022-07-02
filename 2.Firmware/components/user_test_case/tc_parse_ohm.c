/***********************************************************************
 * @file       tc_parse_ohm.c
 * @brief      tc_parse_ohm c file
 * @version    v0.0.1
 * @date       2022-05-28
 *
 * Copyright(C) 2022 .404Zen. all right reserved
***********************************************************************/

/*Include-------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>

#include "tc_parse_ohm.h" 

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#include "esp_log.h"

#include "user_smartconfig.h"
#include "user_http_request.h"
#include "ohm_data_parse.h"
#include "gui_task.h"



/*Defines-------------------------------------------------------------*/
#define TAG         "TC_OHM"

/* Variables -------------------------------------------------------- */
TimerHandle_t xTimer_ohm_refresh;
char ohm_raw_data[2048];                /* according by your hardware, must be greater than the json data length */

extern info_label_t pc_name;
extern info_label_t cpu_name;
extern info_label_t cpu_clock;
extern info_label_t cpu_load;
extern info_label_t cpu_temp;
extern info_label_t gpu_name;
extern info_label_t gpu_clock;
extern info_label_t gpu_load;
extern info_label_t gpu_temp;
extern info_label_t memory;
extern info_label_t hdd0;
extern info_label_t hdd1;
/*Function prototypes-------------------------------------------------*/
static void vTimerCallback_ohm_refresh( TimerHandle_t xTimer );
/*User code-----------------------------------------------------------*/
 /**
* @brief  vTimerCallback_ohm_refresh
* @note   None.
* @param  None.
* @retval None.
*/
static void vTimerCallback_ohm_refresh( TimerHandle_t xTimer )
{
    char strbuf[256];
    /* Optionally do something if the pxTimer parameter is NULL. */
    configASSERT( xTimer );

    if(http_stream_reader(ohm_raw_data, sizeof(ohm_raw_data)) == ESP_OK)
    {
        refresh_ohm_rt_data(ohm_raw_data);

        #if 1
        ohm_data_struct_t *ohm_data;

        ohm_data = get_ohm_data();
        
        printf("PC : %s .\r\n", ohm_data->pc_name);
        refresh_label_text(&pc_name, &(ohm_data->pc_name));
        printf("Mainboard : %s .\r\n", ohm_data->main_board_name);
        for (size_t i = 0; i < CPU_NUMS; i++)
        {   
            memset(strbuf, 0, sizeof(strbuf));
            sprintf(strbuf, "CPU%d : %s",i, ohm_data->cpu[i].name);
            refresh_label_text(&cpu_name, &strbuf);
            printf("CPU%d : %s.\r\n", i, ohm_data->cpu[i].name);
            
            
            printf("|---Clock       : %s.\r\n", ohm_data->cpu[i].clock);
            memset(strbuf, 0, sizeof(strbuf));
            sprintf(strbuf, "Clock : %s", ohm_data->cpu[i].clock);
            refresh_label_text(&cpu_clock, &strbuf);


            printf("|---Load        : %s.\r\n", ohm_data->cpu[i].load);
            memset(strbuf, 0, sizeof(strbuf));
            sprintf(strbuf, "Load : %s", ohm_data->cpu[i].load);
            refresh_label_text(&cpu_load, &strbuf);


            printf("|---Temperture  : %s.\r\n", ohm_data->cpu[i].temp);
            printf("|---Power       : %s.\r\n", ohm_data->cpu[i].power);
            memset(strbuf, 0, sizeof(strbuf));
            sprintf(strbuf, "Temperture : %s", ohm_data->cpu[i].temp);
            // sprintf(strbuf, "Temperture : %s; Power : %s;", ohm_data->cpu[i].temp, ohm_data->cpu[i].power);
            refresh_label_text(&cpu_temp, &strbuf);

            

            int16_t cpu_load = 0;
            cpu_load = atoi(ohm_data->cpu[i].load);
            // set_cpu_load_arc(cpu_load);
        }

        {
            printf("RAM : %s.\r\n", ohm_data->ram.name);
            printf("|---Load        : %s.\r\n", ohm_data->ram.load);
            printf("|---Used        : %s.\r\n", ohm_data->ram.used);
            printf("|---Free        : %s.\r\n", ohm_data->ram.free);
            memset(strbuf, 0, sizeof(strbuf));
            // sprintf(strbuf, "%s : Load %s; Used : %s; Free : %s;", ohm_data->ram.name, ohm_data->ram.load, ohm_data->ram.used, ohm_data->ram.free);
            sprintf(strbuf, "%s : Load %s", ohm_data->ram.name, ohm_data->ram.load);
            refresh_label_text(&memory, &strbuf);
        }

        for (size_t i = 0; i < GPU_NUMS; i++)
        {
            printf("GPU%d : %s.\r\n", i, ohm_data->gpu[i].name);
            memset(strbuf, 0, sizeof(strbuf));
            sprintf(strbuf, "%s", ohm_data->gpu[i].name);
            refresh_label_text(&gpu_name, &strbuf);

            printf("|---Clock       : %s.\r\n", ohm_data->gpu[i].clock);
            printf("|---Load        : %s.\r\n", ohm_data->gpu[i].load);
            memset(strbuf, 0, sizeof(strbuf));
            // sprintf(strbuf, "Clock : %s; Load : %s", ohm_data->gpu[i].clock, ohm_data->gpu[i].load);
            sprintf(strbuf, "Clock : %s", ohm_data->gpu[i].clock);
            refresh_label_text(&gpu_clock, &strbuf);


            printf("|---Temperture  : %s.\r\n", ohm_data->gpu[i].temp);
            printf("|---Power       : %s.\r\n", ohm_data->gpu[i].power);
            memset(strbuf, 0, sizeof(strbuf));
            // sprintf(strbuf, "Temperture : %s; Power : %s", ohm_data->gpu[i].temp, ohm_data->gpu[i].power);
            sprintf(strbuf, "Temperture : %s", ohm_data->gpu[i].temp);
            refresh_label_text(&gpu_temp, &strbuf);

            printf("|---Mem_used    : %s.\r\n", ohm_data->gpu[i].mem_used);
            printf("|---Mem_free    : %s.\r\n", ohm_data->gpu[i].mem_free);
            printf("|---Mem_total   : %s.\r\n", ohm_data->gpu[i].mem_total);
            memset(strbuf, 0, sizeof(strbuf));
            // sprintf(strbuf, "Mem used : %s; Mem free : %s", ohm_data->gpu[i].mem_used, ohm_data->gpu[i].mem_free);
            sprintf(strbuf, "Load : %s", ohm_data->gpu[i].load);
            refresh_label_text(&gpu_load, &strbuf);
        }

        for (size_t i = 0; i < HDD_NUMS; i++)
        {
            printf("HDD%d : %s.(Include SSD Here. But can't get SSD temperture now)\r\n", i, ohm_data->hdd[i].name);
            printf("|---Temperture  : %s.\r\n", ohm_data->hdd[i].temp);
            printf("|---Usage       : %s.\r\n", ohm_data->hdd[i].usage);
            if(i == 0)
            {
                memset(strbuf, 0, sizeof(strbuf));
                sprintf(strbuf, "HDD%d;Temp:%s;Usage:%s", i, ohm_data->hdd[i].temp, ohm_data->hdd[i].usage);
                refresh_label_text(&hdd0, &strbuf);
            }
            else if(i == 1)
            {
                memset(strbuf, 0, sizeof(strbuf));
                sprintf(strbuf, "HDD%d;Temp:%s;Usage:%s", i, ohm_data->hdd[i].temp, ohm_data->hdd[i].usage);
                refresh_label_text(&hdd1, &strbuf);
            }
        }
        #endif
    }
    else
    {
        ESP_LOGW(TAG, "Ohm data read fail");
    }
}

 /**
* @brief  tc_ohm_data_refresh
* @note   None.
* @param  None.
* @retval None.
*/
void tc_ohm_data_refresh(void *args)
{
    /* 定时刷新 ohm 作为后台数据 */
    while (1)
    {
        if(is_wifi_connected())
        {
            ESP_LOGI(TAG, "WiFi Connected. Create http client");

            http_cilent_init();

            if(http_stream_reader(ohm_raw_data, sizeof(ohm_raw_data)) == ESP_OK)
            {
                /* parse static data and realtime data first time */
                parse_ohm_static_data(ohm_raw_data);
                refresh_ohm_rt_data(ohm_raw_data);
            }
            else
            {
                ESP_LOGW(TAG, "Ohm data read init fail. some data may lose");
            }

            /* Create a timer refresh ohm data in time */
            xTimer_ohm_refresh = xTimerCreate (
                "xTimer_ohm_refresh",                       /* Just a text name, not used by the RTOS kernel. */
                pdMS_TO_TICKS(OHM_DATA_REFRESH_TIME*1000),  /* The timer period in ticks, must be greater than 0. */
                pdTRUE,                                     /* The timers will auto-reload themselves when they expire. */
                ( void * ) 0,                               /* The ID is used to store a count of the number of times the timer has expired, which is initialised to 0. */
                vTimerCallback_ohm_refresh                  /* Timer calls the callback when it expires. */
            );

            if( xTimer_ohm_refresh == NULL )
            {
                ESP_LOGE(TAG, "xTimer_ohm_refresh create fail");
            }

            if( xTimerStart( xTimer_ohm_refresh, 0 ) != pdPASS )
            {
                ESP_LOGE(TAG, "xTimer_ohm_refresh start fail");
            }

            vTaskDelete(NULL);
        }
        else
        {
            ESP_LOGI(TAG, "Waiting connect to wifi");
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}



/***** END OF FILE *****/
