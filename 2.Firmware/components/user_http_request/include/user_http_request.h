/********************************************************************************************************
* @file     user_http_request.h
* 
* @brief    user_http_request header file
* 
* @author   404Zen
* 
* @date     2022-05-12  
* 
* @version  Ver: 0.1
* 
* @attention 
* 
* None.
* 
*******************************************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USER_HTTP_REQUEST_H__
#define __USER_HTTP_REQUEST_H__

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>

/* Private includes ----------------------------------------------------------*/
#include "esp_err.h"

/* Exported defines ----------------------------------------------------------*/
/* Constants that aren't configurable in menuconfig */

#if 0       /* For AIDA64 */
#define HTTP_REQ_SERVER "192.168.1.33"
#define HTTP_REQ_PORT "6464"
#define HTTP_REQ_PATH "/sse"
#endif 

#if 1       /* For OpenHardwareMonitor 需要在防火墙中放行端口 */
#define HTTP_REQ_SERVER "192.168.2.163"                     /* Home PC */
// #define HTTP_REQ_SERVER "192.168.1.33"                   /* Company PC, DHCP, Check ip before use */
#define HTTP_REQ_PORT "8085"
#define HTTP_REQ_PATH "/puredata.json"

#define HTTP_REQ_URI    "http://192.168.2.163:8085/puredata.json"       /* Please use this. */
#endif

#if 0       /* For HWiNFO64 with PromDapter */
#define HTTP_REQ_SERVER "192.168.1.22"
#define HTTP_REQ_PORT "10445"
#define HTTP_REQ_PATH "/metrics/json"
#endif


/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void http_cilent_init(void);
esp_err_t http_stream_reader(char *buffer, uint32_t buffer_size);




#endif /* __USER_HTTP_REQUEST_H__ */
/*********************************END OF FILE**********************************/
