/********************************************************************************************************
* @file     ohm_data_parse.h
* 
* @brief    ohm_data_parse header file
* 
* @author   404Zen
* 
* @date     2022-05-18  
* 
* @version  Ver: 0.1
* 
* @attention 
* 
* None.
* 
*******************************************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __OHM_DATA_PARSE_H__
#define __OHM_DATA_PARSE_H__

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>

/* Private includes ----------------------------------------------------------*/

/* Exported defines ----------------------------------------------------------*/
/* Hardware define */
#define CPU_NUMS                        1
#define GPU_NUMS                        1
#define HDD_NUMS                        2                   /* Include SSD. */

#define GPU_VENDOR_NVIDIA
#undef  GPU_VENDOR_ATI

/* OHM parameter define */
#define OHM_DATA_REFRESH_TIME           3                   /* In seconds */

/* Exported types ------------------------------------------------------------*/
typedef struct
{
    char name[32];
    char clock[16];                 /* CPU Core#1 */
    char temp[16];                  /* CPU Package */
    char load[16];                  /* CPU Total */
    char power[16];                 /* CPU Package */
}cpu_struct_t;

typedef struct
{
    char name[64];
    char load[16];
    char used[16];
    char free[16];
}ram_struct_t;


typedef struct
{
    char name[64];
    char clock[16];                 /* GPU Core */
    char temp[16];                  /* GPU Core */
    char load[16];                  /* GPU Core */
    char power[16];                 /* GPU Package */
    char mem_used[16];              /* GRAM */
    char mem_free[16];
    char mem_total[16];
}gpu_struct_t;


typedef struct
{
    char name[32];
    char temp[16];
    char usage[16];
}hdd_struct_t;                      /* Include SSD. */

typedef struct
{
    char pc_name[16];
    char main_board_name[16];

    cpu_struct_t    cpu[CPU_NUMS];
    ram_struct_t    ram;
    gpu_struct_t    gpu[GPU_NUMS];
    hdd_struct_t    hdd[HDD_NUMS];
}ohm_data_struct_t;


/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
uint8_t parse_ohm_static_data(char *raw_data);
void refresh_ohm_rt_data(char *raw_data);
ohm_data_struct_t *get_ohm_data(void);


#endif /* __OHM_DATA_PARSE_H__ */
/*********************************END OF FILE**********************************/
