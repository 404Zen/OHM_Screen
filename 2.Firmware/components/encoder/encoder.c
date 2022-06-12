/********************************************************************************************************
* @file     encoder.c
* 
* @brief    encoder c file
* 
* @author   404Zen
* 
* @date     2022-05-07 
* 
* @version  Ver: 0.1 
* 
* @attention 
* 
*  检测思路来自于 https://github.com/peng-zhihui/Peak/blob/main/2.Firmware/PlatformIO/Peak-ESP32-fw/src/HAL/HAL_Encoder.cpp
*   管脚电平变化
*   A ___|```|___|```|___|```|___|```|_
*   B _|```|___|```|___|```|___|```|___
*   CW -->                       <-- CCW
*   PS: CW方向时，引脚A的边缘跳变后, AB电平相同，
*       CCW方向时，引脚A的边缘跳变后，AB电平相反。
*   PSS: 按键检测使用 esp-iot-solution
* 
*******************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"
#include "esp_log.h"
#include "esp_err.h"
#include "sdkconfig.h"

#include "encoder.h"

/* External variables --------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define TAG         "ENCODER"

#define ESP_INTR_FLAG_DEFAULT           0
/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static volatile int16_t encoder_diff = 0;
/* Private function prototypes -----------------------------------------------*/
static void IRAM_ATTR encoder_gpio_isr_handler(void *args);
/* User code -----------------------------------------------------------------*/
 /**
  * @brief  encoder_init
  * @note   None.
  * @param  None.
  * @retval None.
  */
void encoder_init(void)
{
    gpio_config_t encoder_io_cfg;

    encoder_io_cfg.mode = GPIO_MODE_INPUT;
    encoder_io_cfg.pull_up_en = 1;
    encoder_io_cfg.pull_down_en = 0;
    encoder_io_cfg.intr_type = GPIO_INTR_ANYEDGE;
    encoder_io_cfg.pin_bit_mask = 1ULL << ENCODER_A_IO;
    gpio_config(&encoder_io_cfg);

    encoder_io_cfg.mode = GPIO_MODE_INPUT;
    encoder_io_cfg.pull_up_en = 1;
    encoder_io_cfg.pull_down_en = 0;
    encoder_io_cfg.intr_type = GPIO_INTR_DISABLE;
    encoder_io_cfg.pin_bit_mask = 1ULL << ENCODER_B_IO;
    gpio_config(&encoder_io_cfg);

    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(ENCODER_A_IO, encoder_gpio_isr_handler, (void *)ENCODER_A_IO);

}

int16_t get_encoder_diff(void)
{
    int16_t diff = -encoder_diff/2;
    if(diff != 0)
    {
        encoder_diff = 0;
    }

    return diff;
}
void encoder_task(void *args)
{   
    encoder_init();

    while (1)
    {
        int16_t diff = get_encoder_diff();
        if(diff != 0)
        {
            ESP_LOGI(TAG, "diff is %d.", diff);
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
    
}

 /**
  * @brief  encoder_gpio_isr_handler
  * @note   None.
  * @param  None.
  * @retval None.
  */
static void IRAM_ATTR encoder_gpio_isr_handler(void *args)
{

    static volatile int count, countLast;
    static volatile uint8_t a0, b0;
    static volatile uint8_t ab0;

    uint8_t a = gpio_get_level(ENCODER_A_IO);
    uint8_t b = gpio_get_level(ENCODER_B_IO);

    if(a != a0)
    {
        a0 = a;
        if(b != b0)
        {
            b0 = b;

            count += ((a == b)? 1:-1);
            if((a==b) != ab0)
            {
                count += ((a == b)? 1:-1);
            }
            ab0 = (a == b);
        }
    }
    if(count != countLast)
    {
        encoder_diff += (count - countLast) > 0 ? 1:-1;
        countLast = count; 
    }
}




/*********************************END OF FILE**********************************/
