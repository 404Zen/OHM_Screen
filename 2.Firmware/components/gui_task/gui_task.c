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

/* Variables --------------------------------------------------------- */
SemaphoreHandle_t xGuiSemaphore;

lv_indev_t * indev_encoder;
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

static void gui_layout(void)
{
    /*Create an Arc*/
    // lv_obj_t * arc = lv_arc_create(lv_scr_act());
    // lv_arc_set_rotation(arc, 270);
    // lv_arc_set_bg_angles(arc, 0, 360);
    // lv_obj_remove_style(arc, NULL, LV_PART_KNOB);   /*Be sure the knob is not displayed*/
    // lv_obj_clear_flag(arc, LV_OBJ_FLAG_CLICKABLE);  /*To not allow adjusting by click*/
    // lv_obj_center(arc);

    lv_disp_t * dispp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    ui_Screen1_screen_init();
    lv_disp_load_scr(ui_Screen1);

    // lv_anim_t a;
    // lv_anim_init(&a);
    // lv_anim_set_var(&a, arc);
    // lv_anim_set_exec_cb(&a, set_angle);
    // lv_anim_set_time(&a, 1000);
    // lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);    /*Just for the demo*/
    // lv_anim_set_repeat_delay(&a, 500);
    // lv_anim_set_values(&a, 0, 100);
    // lv_anim_start(&a);
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
