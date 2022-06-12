/********************************************************************************************************
* @file     ohm_data_parse.c
* 
* @brief    ohm_data_parse c file
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

/* Includes ------------------------------------------------------------------*/
#include "ohm_data_parse.h"

#include <string.h>

#include "esp_log.h"
#include "cJSON.h"

/* External variables --------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define TAG         "OHM"
/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
ohm_data_struct_t ohm_rt_data;          /* ohm realtime data */
/* Private function prototypes -----------------------------------------------*/

/* User code -----------------------------------------------------------------*/
 /**
  * @brief  funtion_brief
  * @note   None.
  * @param  None.
  * @retval None.
  */
ohm_data_struct_t *get_ohm_data(void)
{
    return &ohm_rt_data;
}

 /**
  * @brief  funtion_brief
  * @note   None.
  * @param  None.
  * @retval None.
  */
uint8_t parse_ohm_static_data(char *raw_data)
{
    cJSON *root = NULL;
    uint8_t strlen = 16;
    char *str = NULL;

    root = cJSON_Parse(raw_data);

    if(root != NULL)
    {
        /* PC Name */
        cJSON *pc_name = cJSON_GetObjectItem(root, "PCName");
        cJSON *pc_name_name = cJSON_GetObjectItem(pc_name, "Name");
        if(cJSON_IsString(pc_name_name))
        {
            memset(ohm_rt_data.pc_name, 0, sizeof(char)*16);
            strcpy(ohm_rt_data.pc_name, pc_name_name->valuestring);
        }

        /* Mainboard */
        cJSON *mainboard = cJSON_GetObjectItem(root, "Mainboard0");
        cJSON *mainboard_name = cJSON_GetObjectItem(mainboard, "Name");
        if(cJSON_IsString(mainboard_name))
        {
            memset(ohm_rt_data.main_board_name, 0, sizeof(char)*16);
            strcpy(ohm_rt_data.main_board_name, mainboard_name->valuestring);
        }


        str = malloc(strlen * sizeof(char));

        for (size_t i = 0; i < CPU_NUMS; i++)
        {
            /* cpu name */
            memset(str, 0, strlen);
            sprintf(str, "CPU%d", i);

            cJSON *cpu = cJSON_GetObjectItem(root, str);
            cJSON *cpu_name = cJSON_GetObjectItem(cpu, "Name");
            if(cJSON_IsString(cpu_name))
            {
                memset(ohm_rt_data.cpu[i].name, 0, sizeof(char)*32);
                strcpy(ohm_rt_data.cpu[i].name, cpu_name->valuestring);
            }
        }

        {
            /* ram name */
            cJSON *ram = cJSON_GetObjectItem(root, "RAM0");
            cJSON *ram_name = cJSON_GetObjectItem(ram, "Name");
            if(cJSON_IsString(ram_name))
            {
                memset(ohm_rt_data.ram.name, 0, sizeof(char)*32);
                strcpy(ohm_rt_data.ram.name, ram_name->valuestring);
            }
        }

        for (size_t i = 0; i < GPU_NUMS; i++)
        {
            /* gpu name */
            #ifdef GPU_VENDOR_NVIDIA
            memset(str, 0, strlen);
            sprintf(str, "GpuNvidia%d", i);
            #endif
            
            #ifdef GPU_VENDOR_ATI
            #error Please check the original data and fill this program

            memset(str, 0, strlen);
            sprintf(str, "GpuAti%d", i);                    /* Please check this parameter? I am not sure. I don't have a ATI video card */
            #endif

            cJSON *gpu = cJSON_GetObjectItem(root, str);
            cJSON *gpu_name = cJSON_GetObjectItem(gpu, "Name");
            if(cJSON_IsString(gpu_name))
            {
                memset(ohm_rt_data.gpu[i].name, 0, sizeof(char)*64);
                strcpy(ohm_rt_data.gpu[i].name, gpu_name->valuestring);
            }
        }

        /* Harddisk */
        for (size_t i = 0; i < HDD_NUMS; i++)
        {
            memset(str, 0, strlen);
            sprintf(str, "HDD%d", i);

            cJSON *hdd = cJSON_GetObjectItem(root, str);
            cJSON *hdd_name= cJSON_GetObjectItem(hdd, "Name");
            if(cJSON_IsString(hdd_name))
            {
                memset(ohm_rt_data.hdd[i].name, 0, sizeof(char)*32);
                strcpy(ohm_rt_data.hdd[i].name, hdd_name->valuestring);
            }
        }
    }
    else
    {
        ESP_LOGI(TAG, "JSON data parse fail!");

        return 1;
    }

    return 0;
}

 /**
  * @brief  funtion_brief
  * @note   None.
  * @param  None.
  * @retval None.
  */
void refresh_ohm_rt_data(char *raw_data)
{
    cJSON *root = NULL;
    uint8_t strlen = 16;
    char *str = NULL;
    
    root = cJSON_Parse(raw_data);

    str = malloc(strlen * sizeof(char));

    if(root != NULL)
    {
        /* CPU */
        for (size_t i = 0; i < CPU_NUMS; i++)
        {
            memset(str, 0, strlen);
            sprintf(str, "CPU%d", i);

            /* json data, cpu root */
            cJSON *cpu = cJSON_GetObjectItem(root, str);

            /* cpu clock */
            cJSON *cpu_clock = cJSON_GetObjectItem(cpu, "Clocks");
            cJSON *cpu_core1_clock = cJSON_GetObjectItem(cpu_clock, "CPU Core #1");
            if(cJSON_IsString(cpu_core1_clock))
            {
                memset(ohm_rt_data.cpu[i].clock, 0, sizeof(char)*16);
                strcpy(ohm_rt_data.cpu[i].clock, cpu_core1_clock->valuestring);
            }
            

            /* cpu temperature */
            cJSON *cpu_temp = cJSON_GetObjectItem(cpu, "Temperatures");
            cJSON *cpu_package_temp = cJSON_GetObjectItem(cpu_temp, "CPU Package");
            if(cJSON_IsString(cpu_package_temp))
            {
                memset(ohm_rt_data.cpu[i].temp, 0, sizeof(char)*16);
                strcpy(ohm_rt_data.cpu[i].temp, cpu_package_temp->valuestring);
            }
            

            /* cpu load */
            cJSON *cpu_load = cJSON_GetObjectItem(cpu, "Load");
            cJSON *cpu_total_load = cJSON_GetObjectItem(cpu_load, "CPU Total");
            if(cJSON_IsString(cpu_total_load))
            {
                memset(ohm_rt_data.cpu[i].load, 0, sizeof(char)*16);
                strcpy(ohm_rt_data.cpu[i].load, cpu_total_load->valuestring);
            }
            

            /* cpu power */
            cJSON *cpu_power = cJSON_GetObjectItem(cpu, "Powers");
            cJSON *cpu_package_power = cJSON_GetObjectItem(cpu_power, "CPU Package");
            if(cJSON_IsString(cpu_package_power))
            {
                memset(ohm_rt_data.cpu[i].power, 0, sizeof(char)*16);
                strcpy(ohm_rt_data.cpu[i].power, cpu_package_power->valuestring);
            }
            
        }

        /* RAM */
        {            
            cJSON *ram = cJSON_GetObjectItem(root, "RAM0");

            /* ram load */
            cJSON *ram_load = cJSON_GetObjectItem(ram, "Load");
            cJSON *ram_load_percent = cJSON_GetObjectItem(ram_load, "Memory");
            if(cJSON_IsString(ram_load_percent))
            {
                memset(ohm_rt_data.ram.load, 0, sizeof(char)*16);
                strcpy(ohm_rt_data.ram.load, ram_load_percent->valuestring);
            }
            

            /* ram usage */
            cJSON *ram_data = cJSON_GetObjectItem(ram, "Data");
            cJSON *ram_used_mem = cJSON_GetObjectItem(ram_data, "Used Memory");
            cJSON *ram_free_mem = cJSON_GetObjectItem(ram_data, "Available Memory");
            if(cJSON_IsString(ram_used_mem))
            {
                memset(ohm_rt_data.ram.used, 0, sizeof(char)*16);
                strcpy(ohm_rt_data.ram.used, ram_used_mem->valuestring);
            }

            if(cJSON_IsString(ram_free_mem))
            {
                memset(ohm_rt_data.ram.free, 0, sizeof(char)*16);
                strcpy(ohm_rt_data.ram.free, ram_free_mem->valuestring);
            }
        }

        for (size_t i = 0; i < GPU_NUMS; i++)
        {
            #ifdef GPU_VENDOR_NVIDIA
            memset(str, 0, strlen);
            sprintf(str, "GpuNvidia%d", i);
            #endif
            
            #ifdef GPU_VENDOR_ATI
            #error Please check the original data and fill this program

            memset(str, 0, strlen);
            sprintf(str, "GpuAti%d", i);                    /* Please check this parameter? I am not sure. I don't have a ATI video card */
            #endif

            cJSON *gpu = cJSON_GetObjectItem(root, str);

            /* gpu clock */
            cJSON *gpu_clock = cJSON_GetObjectItem(gpu, "Clocks");
            cJSON *gpu_core_clock = cJSON_GetObjectItem(gpu_clock, "GPU Core");
            if(cJSON_IsString(gpu_core_clock))
            {
                memset(ohm_rt_data.gpu[i].clock, 0, sizeof(char)*16);
                strcpy(ohm_rt_data.gpu[i].clock, gpu_core_clock->valuestring);
            }
            

            /* gpu temp */
            cJSON *gpu_temp = cJSON_GetObjectItem(gpu, "Temperatures");
            cJSON *gpu_core_temp = cJSON_GetObjectItem(gpu_temp, "GPU Core");
            if(cJSON_IsString(gpu_core_temp))
            {
                memset(ohm_rt_data.gpu[i].temp, 0, sizeof(char)*16);
                strcpy(ohm_rt_data.gpu[i].temp, gpu_core_temp->valuestring);
            }
            
            
            /* gpu load */
            cJSON *gpu_load = cJSON_GetObjectItem(gpu, "Load");
            cJSON *gpu_core_load = cJSON_GetObjectItem(gpu_load, "GPU Core");
            if(cJSON_IsString(gpu_core_load))
            {
                memset(ohm_rt_data.gpu[i].load, 0, sizeof(char)*16);
                strcpy(ohm_rt_data.gpu[i].load, gpu_core_load->valuestring);
            }
            

            /* gpu power */
            cJSON *gpu_power = cJSON_GetObjectItem(gpu, "Powers");
            cJSON *gpu_power_power = cJSON_GetObjectItem(gpu_power, "GPU Power");
            if(cJSON_IsString(gpu_power_power))
            {
                memset(ohm_rt_data.gpu[i].power, 0, sizeof(char)*16);
                strcpy(ohm_rt_data.gpu[i].power, gpu_power_power->valuestring);
            }
            
            /* gpu mem */
            cJSON *gpu_data = cJSON_GetObjectItem(gpu, "Data");
            cJSON *gpu_mem_free = cJSON_GetObjectItem(gpu_data, "GPU Memory Free");
            cJSON *gpu_mem_used = cJSON_GetObjectItem(gpu_data, "GPU Memory Used");
            cJSON *gpu_mem_total = cJSON_GetObjectItem(gpu_data, "GPU Memory Total");
            if(cJSON_IsString(gpu_mem_free))
            {
                memset(ohm_rt_data.gpu[i].mem_free, 0, sizeof(char)*16);
                strcpy(ohm_rt_data.gpu[i].mem_free, gpu_mem_free->valuestring);
            }
            
            if(cJSON_IsString(gpu_mem_used))
            {
                memset(ohm_rt_data.gpu[i].mem_used, 0, sizeof(char)*16);
                strcpy(ohm_rt_data.gpu[i].mem_used, gpu_mem_used->valuestring);
            }

            if(cJSON_IsString(gpu_mem_total))
            {
                memset(ohm_rt_data.gpu[i].mem_total, 0, sizeof(char)*16);
                strcpy(ohm_rt_data.gpu[i].mem_total, gpu_mem_total->valuestring);
            }

        }

        /* Harddisk */
        for (size_t i = 0; i < HDD_NUMS; i++)
        {
            memset(str, 0, strlen);
            sprintf(str, "HDD%d", i);

            cJSON *hdd = cJSON_GetObjectItem(root, str);

            /* hdd temp */
            cJSON *hdd_temp = cJSON_GetObjectItem(hdd, "Temperatures");
            cJSON *hdd_temp_temp = cJSON_GetObjectItem(hdd_temp, "Temperature");
            if(cJSON_IsString(hdd_temp_temp))
            {
                memset(ohm_rt_data.hdd[i].temp, 0, sizeof(char)*16);
                strcpy(ohm_rt_data.hdd[i].temp, hdd_temp_temp->valuestring);
            }

            /* hdd usage */
            cJSON *hdd_load = cJSON_GetObjectItem(hdd, "Load");
            cJSON *hdd_load_usage = cJSON_GetObjectItem(hdd_load, "Used Space");
            if(cJSON_IsString(hdd_load_usage))
            {
                memset(ohm_rt_data.hdd[i].usage, 0, sizeof(char)*16);
                strcpy(ohm_rt_data.hdd[i].usage, hdd_load_usage->valuestring);
            }
        }
        
        free(str);
    }
    else
    {
        ESP_LOGI(TAG, "Parse json data error!");
    }

    cJSON_Delete(root);  
}









/*********************************END OF FILE**********************************/
