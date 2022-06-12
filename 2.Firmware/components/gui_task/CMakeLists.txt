idf_component_register(SRCS "gui_task.c"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES   lvgl
                    PRIV_REQUIRES   lv_examples
                    PRIV_REQUIRES   lvgl_esp32_drivers)
