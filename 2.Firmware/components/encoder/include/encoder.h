/********************************************************************************************************
* @file     encoder.h
* 
* @brief    encoder header file
* 
* @author   404Zen
* 
* @date     2022-05-07  
* 
* @version  Ver: 0.1
* 
* @attention 
* 
* None.
* 
*******************************************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ENCODER_H__
#define __ENCODER_H__

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
#include "driver/gpio.h"

/* Private includes ----------------------------------------------------------*/

/* Exported defines ----------------------------------------------------------*/
#define ENCODER_A_IO                    35
#define ENCODER_B_IO                    34


/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void encoder_init(void);
int16_t get_encoder_diff(void);
void encoder_task(void *args);          /* A test example */




#endif /* __ENCODER_H__ */
/*********************************END OF FILE**********************************/
