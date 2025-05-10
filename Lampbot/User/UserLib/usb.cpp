/**
* @file usb.cpp
 * @brief USB类
 * @author Wenxin Hu
 * @date 25-5-3
 * @version 1.0
 * @note 实现USB类中虚拟串口通讯的方法
 */

#include "usb.h"
#include "usbd_cdc_if.h"
#include "lib_public.h"

bool usb::initialize() {
    if (initialized) return false;

    initialized = true;
    return true;
}

bool usb::enable() {
    if (!initialized) return false;
    if (enabled) return false;

    enabled = true;
    return true;
}

bool usb::disable() {
    if (!initialized) return false;
    if (!enabled) return false;

    enabled = false;
    return true;
}

bool usb::start() {
    if (!initialized) return false;
    if (!enabled) return false;
    if (started) return false;

    started = true;
    return true;
}

bool usb::stop() {
    if (!initialized) return false;
    if (!enabled) return false;
    if (!started) return false;

    started = false;
    return true;
}

bool usb::rx_data(uint8_t buf[], uint32_t len) {
    if (!initialized) return false;
    if (!enabled) return false;
    if (!started) return false;

    memset(rx_buffer, 0, 64);    //清空接收缓存区
    memcpy(rx_buffer, buf, len); //将接收的数据复制到缓存区
    rx_len = len;

    return true;
}

bool usb::tx_data(uint8_t buf[], uint8_t len) {
    if (!initialized) return false;
    if (!enabled) return false;
    if (!started) return false;

    memset(tx_buffer, 0, 64);    //清空发送缓存区
    memcpy(tx_buffer, buf, len); //将数据复制到发送缓存区
    tx_len = len;

    CDC_Transmit_FS(tx_buffer, tx_len);
    memset(tx_buffer, 0, 64); //发送完成后清空发送缓存区

    return true;
}

bool usb::tx_Msg(float yaw, float pitch) {
    if (!initialized) return false;
    if (!enabled) return false;
    if (!started) return false;

    msg_to_minipc.start = 's';
    msg_to_minipc.end = 'e';
    msg_to_minipc.type = 0xB0;
    msg_to_minipc.yaw = yaw;
    msg_to_minipc.pitch = pitch;

    char float_yaw[4], float_pitch[4];
    float_yaw[0] = *(char*)(&msg_to_minipc.yaw);
    float_yaw[1] = *((char*)(&msg_to_minipc.yaw) + 1);
    float_yaw[2] = *((char*)(&msg_to_minipc.yaw) + 2);
    float_yaw[3] = *((char*)(&msg_to_minipc.yaw) + 3);
    float_pitch[0] = *(char*)(&msg_to_minipc.pitch);
    float_pitch[1] = *((char*)(&msg_to_minipc.pitch) + 1);
    float_pitch[2] = *((char*)(&msg_to_minipc.pitch) + 2);
    float_pitch[3] = *((char*)(&msg_to_minipc.pitch) + 3);

    tx_len = 32;
    tx_buffer[0] = msg_to_minipc.start;
    tx_buffer[1] = msg_to_minipc.type;
    tx_buffer[2] = float_yaw[0];
    tx_buffer[3] = float_yaw[1];
    tx_buffer[4] = float_yaw[2];
    tx_buffer[5] = float_yaw[3];
    tx_buffer[6] = float_pitch[0];
    tx_buffer[7] = float_pitch[1];
    tx_buffer[8] = float_pitch[2];
    tx_buffer[9] = float_pitch[3];
    for (char i = 10; i < 30; i++) {
        //中间留空
        tx_buffer[i] = 0x00;
    }
    tx_buffer[31] = msg_to_minipc.end;

    CDC_Transmit_FS(tx_buffer, tx_len);
    memset(tx_buffer, 0, 64); //数据发送后清空缓存区
    tx_len = 0;               //有利于判断，置0

    return true;
}

bool usb::rx_Msg() {
    if (!initialized) return false;
    if (!enabled) return false;
    if (!started) return false;

    if (rx_len) {
        if (rx_buffer[1] == 0xA0) {
            msg_from_minipc.start = rx_buffer[0];
            msg_from_minipc.type = rx_buffer[1];
            msg_from_minipc.find_bool = rx_buffer[2];
            msg_from_minipc.yaw = *((float*)&(rx_buffer[3]));
            msg_from_minipc.pitch = *((float*)&(rx_buffer[7]));
            msg_from_minipc.end = rx_buffer[31];
            memset(rx_buffer, 0, 64); //数据处理后清空缓存区
            rx_len = 0;               //有利于判断，置0
        }
        memset(rx_buffer, 0, 64); //数据处理后清空缓存区
        rx_len = 0;               //有利于判断，置0,防止空包
    }

    return true;
}

usb usb_1;

void USBData_GetData(uint8_t buf[], uint32_t len) {
    usb_1.rx_data(buf, len);
}
