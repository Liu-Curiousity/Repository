/**
* @file uart_base.h
 * @brief UART基类
 * @author Wenxin Hu
 * @date 25-5-3
 * @version 1.0
 * @note 所有UART设备的基类，提供了基本的接口和属性
 */
#ifndef UART_BASE_H
#define UART_BASE_H

#include "usart.h"

class uart {
public:
    virtual ~uart() = default;
    uart() = default;

    bool initialized{false}; //是否初始化
    bool started{false};     //是否启动

    UART_HandleTypeDef* huart{nullptr}; //UART句柄

    virtual bool initialize(UART_HandleTypeDef* handle) = 0; //true 成功， false 失败
    virtual bool start() = 0;                                //true 成功， false 失败
    virtual bool stop() = 0;                                 //true 成功， false 失败

    //rx和tx仅表示将缓存区的数据发送，不包含编码解码
    virtual bool rx_data() = 0;     //接收数据
    virtual bool tx_data() = 0;     //发送数据
    virtual bool encode_data() = 0; //编码数据
    virtual bool decode_data() = 0; //解码数据
};

#endif //UART_BASE_H
