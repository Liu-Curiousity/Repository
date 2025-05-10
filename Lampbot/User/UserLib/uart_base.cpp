/**
* @file uart_base.cpp
 * @brief 重写uart中断等
 * @author Wenxin Hu
 * @date 25-5-3
 * @version 1.0
 * @note
 */

#include "uart_remote.h"
#include "lib_public.h"

extern remote_dr16 dr16_1;

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t Size) {
    if (huart->Instance == dr16_1.huart->Instance) {
        //收到一包数据，进行数据处理
        dr16_1.decode_data();
        //重新开启DMA空闲接收
        HAL_UARTEx_ReceiveToIdle_DMA(huart, dr16_1.get_rx_buffer(), dr16_1.get_rx_buffer_size());
    }
}
