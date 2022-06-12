/***********************************************************************
 * @file       user_test_case.c
 * @brief      user_test_case c file
 * @version    v0.0.1
 * @date       2022-05-28
 *
 * Copyright(C) 2022 .404Zen. all right reserved
***********************************************************************/

/*Include-------------------------------------------------------------*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

#include "user_test_case.h" 

/* Test case */
#include "tc_parse_ohm.h"
#include "encoder.h"


/*Defines-------------------------------------------------------------*/

/*Function prototypes-------------------------------------------------*/


/*User code-----------------------------------------------------------*/
 /**
* @brief  vtask_test_case
* @note   None.
* @param  None.
* @retval None.
*/
void vtask_test_case(void *args)
{       
    // xTaskCreate(tc_ohm_data_refresh, "tc_ohm_data_refresh", 4096, NULL, 5, NULL);
    // xTaskCreate(encoder_task, "encoder_task", 2048, NULL, 3, NULL);
    // xTaskCreate(vtask_gui, "vtask_gui", 8192, NULL, 5, NULL);
    vTaskDelete(NULL);
}



/***** END OF FILE *****/
