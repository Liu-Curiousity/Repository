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

 /**=======================================C头文件内容======================================**/
 //中断重写等声明
 //中断函数重写的声明必须在此声明(在extern "C"块中),否则在对应.c中找不到该函数符号
 void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);
 /**======================================================================================**/


#ifdef __cplusplus
}
#endif

#endif //LIB_PUBLIC_H
