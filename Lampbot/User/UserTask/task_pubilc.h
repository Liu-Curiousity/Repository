/**
 * @file task_pubilc.h
 * @brief FreeRTOS任务公共头文件
 * @author Wenxin Hu
 * @date 25-5-3
 * @version 1.0
 * @note 任务函数必须在此文件定义，否则在freertos.c中找不到该函数符号
 */
#ifndef TASK_PUBILC_H
#define TASK_PUBILC_H

#ifdef __cplusplus
extern "C" {
#endif

/**=======================================C头文件内容======================================**/
//任务句柄和任务函数声明
//任务函数必须在此声明(在extern "C"块中),否则在freertos.c中找不到该函数符号
void AppTask_Debug(void const *argument);
void AppTask_USB(void const * argument);
void AppTask_Motor(void const *argument);
void AppTask_M2006(void const *argument);
/**======================================================================================**/


#ifdef __cplusplus
}
#endif

#endif //TASK_PUBILC_H
