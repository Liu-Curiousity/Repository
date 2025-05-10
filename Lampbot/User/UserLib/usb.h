/**
* @file usb.h
 * @brief USB虚拟串口类
 * @author Wenxin Hu
 * @date 25-5-3
 * @version 1.0
 * @note 用于USB虚拟串口数据传输
 */

#ifndef USB_H
#define USB_H

#include "stdint.h"

//上位机发报
typedef struct {
    char start{'s'};         //0 帧头取 's'
    int16_t type{0xA0}; //1 消息类型：上->下：0xA0
    bool find_bool{false};   //2 是否追踪
    float yaw{0.0f};         //3-6 yaw数据
    float pitch{0.0f};       //7-10 pitch数据
    char end{'e'};           //31 帧尾取'e'
} msg_rx_t;

//下位机发报
typedef struct {
    char start{'s'};         //0 帧头取 's'
    int16_t type{0xB0}; //1 消息类型：下->上：0xB0
    float yaw{0.0f};         //2-5 yaw数据
    float pitch{0.0f};       //6-9 pitch数据
    char end{'e'};           //31 帧尾取'e'
} msg_tx_t;

class usb {
public:
    bool initialized{false};
    bool enabled{false};
    bool started{false};

    bool initialize();
    bool enable();
    bool disable();
    bool start();
    bool stop();

    bool rx_data(uint8_t buf[], uint32_t len);
    bool tx_data(uint8_t buf[], uint8_t len);
    bool tx_Msg(float yaw, float pitch);
    bool rx_Msg();

private:
    uint8_t rx_buffer[64] = {};
    uint16_t rx_len{64};
    uint8_t tx_buffer[64] = {};
    uint16_t tx_len{64};
    msg_rx_t msg_from_minipc;
    msg_tx_t msg_to_minipc;
};

#endif //USB_H
