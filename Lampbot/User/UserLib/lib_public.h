/**
* @file lib_public.h
 * @brief 所有需要与.c文件连接的库文件的公共头文件
 * @author Wenxin Hu
 * @date 25-5-3
 * @version 1.0
 * @note
 */
#ifndef LIB_PUBLIC_H
#define LIB_PUBLIC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cmsis_os.h"

/**=======================================C头文件内容======================================**/
//中断重写等声明
//中断函数重写的声明必须在此声明(在extern "C"块中),否则在对应.c中找不到该函数符号
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t Size);

void USBData_GetData(uint8_t buf[], uint32_t len);

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan);
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef* hcan);

/**======================================================================================**/
#define delay(ms)               osDelay(ms)         //延时函数
#define get_tick()              xTaskGetTickCount() //获取系统滴答计数
#define sys_log_function(...)   printf(__VA_ARGS__)
    /**=======================================================================================**/

#ifdef __cplusplus
}
#endif

#endif //LIB_PUBLIC_H
