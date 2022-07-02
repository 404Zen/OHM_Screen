/***********************************************************************
 * @file       gui_task.c
 * @brief      gui_task c file
 * @version    v0.0.1
 * @date       2022-06-12
 *
 * Copyright(C) 2022 .404Zen. all right reserved
 ***********************************************************************/

/*Include-------------------------------------------------------------*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "lv_examples/src/lv_demo_widgets/lv_demo_widgets.h"
#include "lv_examples/src/lv_demo_music/lv_demo_music.h"
#include "lv_examples/src/lv_demo_benchmark/lv_demo_benchmark.h"
#include "lvgl_helpers.h"

#include "gui_task.h"
#include "encoder.h"

/*Defines-------------------------------------------------------------*/
typedef struct{
    lv_obj_t *pc_name;                  /* Line 1 */
    lv_obj_t *cpu_name;
}pc_text_info_t;



/* Variables --------------------------------------------------------- */
SemaphoreHandle_t xGuiSemaphore;

lv_indev_t * indev_encoder;

info_label_t pc_name;
info_label_t cpu_name;
info_label_t cpu_clock;
info_label_t cpu_load;
info_label_t cpu_temp;
info_label_t gpu_name;
info_label_t gpu_clock;
info_label_t gpu_load;
info_label_t gpu_temp;
info_label_t memory;
info_label_t hdd0;
info_label_t hdd1;
/*Function prototypes-------------------------------------------------*/
static void gui_layout(void);

static void gui_encoder_init(void);
static void gui_encoder_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data);
static void gui_encoder_handler(void);

/*User code-----------------------------------------------------------*/
/**
 * @brief  encoder_init
 * @note   None.
 * @param  None.
 * @retval None.
 */
static void lv_tick_task(void *arg)
{
    (void)arg;
    lv_tick_inc(10);
}

static void anim_x_cb(void * var, int32_t v)
{
    lv_obj_set_x(var, v);
}

static void anim_size_cb(void * var, int32_t v)
{
    lv_obj_set_size(var, v, v);
}

static void set_angle(void * obj, int32_t v)
{
    lv_arc_set_value(obj, v);
}

lv_obj_t * cpu_load_arc = NULL;



lv_obj_t * ui_Screen1;
lv_obj_t * ui_cpuload;

void set_cpu_load_arc(int16_t val)
{
    lv_arc_set_value(ui_cpuload, val);
}

void ui_Screen1_screen_init(void)
{

    // ui_Screen1
    ui_Screen1 = lv_obj_create(NULL);

    lv_obj_clear_flag(ui_Screen1, LV_OBJ_FLAG_SCROLLABLE);

    // ui_cpuload

    ui_cpuload = lv_arc_create(ui_Screen1);

    lv_obj_set_width(ui_cpuload, 80);
    lv_obj_set_height(ui_cpuload, 80);

    lv_obj_set_x(ui_cpuload, 0);
    lv_obj_set_y(ui_cpuload, 0);

    lv_obj_clear_flag(ui_cpuload, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_SNAPPABLE);

    lv_arc_set_range(ui_cpuload, 0, 100);
    lv_arc_set_bg_angles(ui_cpuload, 150, 30);

    lv_obj_set_style_arc_width(ui_cpuload, 2, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_arc_width(ui_cpuload, 1, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_blend_mode(ui_cpuload, LV_BLEND_MODE_ADDITIVE, LV_PART_KNOB | LV_STATE_DEFAULT);

}

static void creat_new_label(info_label_t *label)
{
    label->label = lv_label_create(lv_scr_act());
    lv_label_set_long_mode((lv_obj_t*)label->label, LV_LABEL_LONG_CLIP);
    lv_label_set_text_fmt((lv_obj_t*)label->label, "%s.", label->label_name);
    lv_obj_set_width((lv_obj_t*)label->label, 220);
    lv_obj_align((lv_obj_t*)label->label, LV_ALIGN_TOP_LEFT, label->x_offset, label->y_offset);
}

void refresh_label_text(info_label_t *label, char *str)
{
    lv_label_set_text_fmt((lv_obj_t*)label->label, "%s.", str);
}

static void gui_layout(void)
{
    strcpy(pc_name.label_name, "pc name");
    pc_name.x_offset = 10;
    pc_name.y_offset = 0;
    creat_new_label(&pc_name);

    strcpy(cpu_name.label_name, "cpu_name");
    cpu_name.x_offset = 10;
    cpu_name.y_offset = 20;
    creat_new_label(&cpu_name);

    strcpy(cpu_clock.label_name, "cpu_clock");
    cpu_clock.x_offset = 20;
    cpu_clock.y_offset = 40;
    creat_new_label(&cpu_clock);

    strcpy(cpu_load.label_name, "cpu_load");
    cpu_load.x_offset = 20;
    cpu_load.y_offset = 60;
    creat_new_label(&cpu_load);

    strcpy(cpu_temp.label_name, "cpu_temp");
    cpu_temp.x_offset = 20;
    cpu_temp.y_offset = 80;
    creat_new_label(&cpu_temp);

    strcpy(gpu_name.label_name, "gpu_name");
    gpu_name.x_offset = 10;
    gpu_name.y_offset = 100;
    creat_new_label(&gpu_name);

    strcpy(gpu_clock.label_name, "gpu_clock");
    gpu_clock.x_offset = 20;
    gpu_clock.y_offset = 120;
    creat_new_label(&gpu_clock);

    strcpy(gpu_load.label_name, "gpu_load");
    gpu_load.x_offset = 20;
    gpu_load.y_offset = 140;
    creat_new_label(&gpu_load);

    strcpy(gpu_temp.label_name, "gpu_temp");
    gpu_temp.x_offset = 20;
    gpu_temp.y_offset = 160;
    creat_new_label(&gpu_temp);

    strcpy(memory.label_name, "memory");
    memory.x_offset = 10;
    memory.y_offset = 180;
    creat_new_label(&memory);

    strcpy(hdd0.label_name, "hdd0");
    hdd0.x_offset = 10;
    hdd0.y_offset = 200;
    creat_new_label(&hdd0);

    strcpy(hdd1.label_name, "hdd1");
    hdd1.x_offset = 10;
    hdd1.y_offset = 220;
    creat_new_label(&hdd1);
}

void gui_task(void *arg)
{
    static lv_indev_drv_t indev_drv;
    xGuiSemaphore = xSemaphoreCreateMutex();
    lv_init();          // lvgl内核初始化
    lvgl_driver_init(); // lvgl显示接口初始化

    /* Example for 1) */
    static lv_disp_draw_buf_t draw_buf;

    lv_color_t *buf1 = heap_caps_malloc(DISP_BUF_SIZE * 2, MALLOC_CAP_DMA);
    lv_color_t *buf2 = heap_caps_malloc(DISP_BUF_SIZE * 2, MALLOC_CAP_DMA);

    // lv_color_t *buf1 = (lv_color_t *)heap_caps_malloc(DISP_BUF_SIZE * 2, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    // lv_color_t *buf2 = (lv_color_t *)heap_caps_malloc(DISP_BUF_SIZE * 2, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

    lv_disp_draw_buf_init(&draw_buf, buf1, buf2, DLV_HOR_RES_MAX * DLV_VER_RES_MAX); /*Initialize the display buffer*/

    static lv_disp_drv_t disp_drv;         /*A variable to hold the drivers. Must be static or global.*/
    lv_disp_drv_init(&disp_drv);           /*Basic initialization*/
    disp_drv.draw_buf = &draw_buf;         /*Set an initialized buffer*/
    disp_drv.flush_cb = disp_driver_flush; /*Set a flush callback to draw to the display*/
    disp_drv.hor_res = 240;                /*Set the horizontal resolution in pixels*/
    disp_drv.ver_res = 240;                /*Set the vertical resolution in pixels*/
    lv_disp_drv_register(&disp_drv);       /*Register the driver and save the created display objects*/
    

    /*Initialize your encoder if you have*/
    gui_encoder_init();

    /*Register a encoder input device*/
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_ENCODER;
    indev_drv.read_cb = gui_encoder_read;
    indev_encoder = lv_indev_drv_register(&indev_drv);
    

    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"};
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 10 * 1000));

    // lv_demo_widgets();
    // lv_demo_music();
    // lv_demo_benchmark();
    gui_layout();

    while (1)
    {
        /* Delay 1 tick (assumes FreeRTOS tick is 10ms */
        vTaskDelay(pdMS_TO_TICKS(10));

        /* Try to take the semaphore, call lvgl related function on success */
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
        {
            /* lv_timer_handler()的优先级应该低于 lv_tick_inc */
            lv_timer_handler();
            xSemaphoreGive(xGuiSemaphore);
        }
    }
}

static void gui_encoder_init(void)
{
    encoder_init();
}

static void gui_encoder_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    data->enc_diff = get_encoder_diff();
    data->state = LV_INDEV_STATE_PR;            //一直松开
}

static void gui_encoder_handler(void)
{

}

/***** END OF FILE *****/
