/********************************************************************************************************
* @file     user_http_request.c
* 
* @brief    user_http_request c file
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

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"


#include "esp_http_client.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "user_http_request.h"

/* External variables --------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static const char *TAG = "HTTP_REQUEST";

esp_http_client_handle_t client;

/* Private function prototypes -----------------------------------------------*/

/* User code -----------------------------------------------------------------*/
 /**
  * @brief  _http_event_handle
  * @note   None.
  * @param  None.
  * @retval None.
  */
esp_err_t _http_event_handle(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER");
            printf("%.*s", evt->data_len, (char*)evt->data);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) {
                printf("%.*s", evt->data_len, (char*)evt->data);
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}



 /**
  * @brief  http_cilent_init
  * @note   None.
  * @param  None.
  * @retval None.
  */
void http_cilent_init(void)
{
    esp_http_client_config_t config = 
    {
        .url = HTTP_REQ_URI,
    };

    client = esp_http_client_init(&config);
}

 /**
  * @brief  http_stream_reader
  * @note   None.
  * @param  None.
  * @retval None.
  */
esp_err_t http_stream_reader(char *buffer, uint32_t buffer_size)
{
    if (buffer == NULL) 
    {
        return 1;
    }

    esp_err_t err;
    if ((err = esp_http_client_open(client, 0)) != ESP_OK) 
    {
        ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
        return err;
    }

    int content_length =  esp_http_client_fetch_headers(client);
    int total_read_len = 0, read_len;

    if(content_length > buffer_size)
    {
        ESP_LOGE(TAG, "data len > buffer len, %d > %d...", content_length, buffer_size);
        return 2;
    }

    if (total_read_len < content_length) 
    {
        read_len = esp_http_client_read(client, buffer, content_length);
        if (read_len <= 0) {
            ESP_LOGE(TAG, "Error read data");
        }
        buffer[read_len] = 0;
        ESP_LOGD(TAG, "read_len = %d", read_len);
    }

    ESP_LOGI(TAG, "HTTP Stream reader Status = %d, content_length = %d",
                    esp_http_client_get_status_code(client),
                    esp_http_client_get_content_length(client));
    
    // ESP_LOGI(TAG, "%s", buffer);

    esp_http_client_close(client);
    // esp_http_client_cleanup(client);

    return ESP_OK;
}


/*********************************END OF FILE**********************************/
