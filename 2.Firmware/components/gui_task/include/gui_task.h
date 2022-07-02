/***********************************************************************
 * @file       gui_task.h
 * @brief      gui_task header file
 * @version    v0.0.1
 * @date       2022-06-12
 *
 * Copyright(C) 2022 .404Zen. all right reserved
***********************************************************************/
#ifndef __GUI_TASK_H__
#define __GUI_TASK_H__

/*Include-------------------------------------------------------------*/

/*Defines-------------------------------------------------------------*/

typedef struct{
    void        *label;
    char        label_name[32];
    int16_t     x_offset;
    int16_t     y_offset;
}info_label_t;


/*Function prototypes-------------------------------------------------*/

void gui_task(void *arg);

void refresh_label_text(info_label_t *label, char *str);

#endif /* __GUI_TASK_H__ */
/***** END OF FILE *****/
