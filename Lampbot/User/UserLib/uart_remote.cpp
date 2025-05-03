/**
* @file uart_remote.cpp
 * @brief DR16遥控器类
 * @author Wenxin Hu
 * @date 25-5-3
 * @version 1.0
 * @note 实现遥控器类中的所有方法
 */

#include "uart_remote.h"

bool remote_dr16::initialize(UART_HandleTypeDef* handle) {
    if (!this->initialized) return false;

    this->huart = handle;

    this->initialized = true;
    return true;
}

bool remote_dr16::start() {
    if (!this->initialized) return false;

    HAL_UART_DMAResume(huart);

    HAL_UARTEx_ReceiveToIdle_DMA(huart, rx_buffer, sizeof(rx_buffer));

    this->started = true;
    return true;
}

bool remote_dr16::stop() {
    if (!this->initialized) return false;
    if (!this->started) return false;

    HAL_UART_DMAPause(huart);

    this->started = false;
    return true;
}

bool remote_dr16::encode_data() {
    return true;
}

bool remote_dr16::rx_data() {
    return true;
}

bool remote_dr16::tx_data() {
    return true;
}

bool remote_dr16::decode_data() {
    if (!this->initialized) return false;
    if (!this->started) return false;

    info.ch0 = (((int16_t)rx_buffer[0] | ((int16_t)rx_buffer[1] << 8)) & 0x07FF)-1024;
    info.ch1 = ((((int16_t)rx_buffer[1] >> 3) | ((int16_t)rx_buffer[2] << 5)) & 0x07FF)-1024;
    info.ch2 = ((((int16_t)rx_buffer[2] >> 6) | ((int16_t)rx_buffer[3] << 2) |((int16_t)rx_buffer[4] << 10)) & 0x07FF)-1024;
    info.ch3 = ((((int16_t)rx_buffer[4] >> 1) | ((int16_t)rx_buffer[5]<<7)) & 0x07FF)-1024;
    info.s1 = ((rx_buffer[5] >> 4) & 0x000C) >> 2;
    info.s2 = ((rx_buffer[5] >> 4) & 0x0003);
    info.x = ((int16_t)rx_buffer[6]) | ((int16_t)rx_buffer[7] << 8);
    info.y = ((int16_t)rx_buffer[8]) | ((int16_t)rx_buffer[9] << 8);
    info.z = ((int16_t)rx_buffer[10]) | ((int16_t)rx_buffer[11] << 8);
    info.press_l = rx_buffer[12];
    info.press_r = rx_buffer[13];
    info.key = ((int16_t)rx_buffer[14]) | ((int16_t)rx_buffer[15] << 8);
    info.wheel = ((int16_t)rx_buffer[16] | (int16_t)rx_buffer[17] << 8) - 1024;

    return true;
}

uint8_t* remote_dr16::get_rx_buffer() {
    return rx_buffer;
}

remote_t* remote_dr16::get_info() {
    return &info;
}

uint8_t remote_dr16::get_rx_buffer_size() {
    return sizeof(rx_buffer);
}

int16_t remote_dr16::get_ch0() {
    return info.ch0;
}

int16_t remote_dr16::get_ch1() {
    return info.ch1;
}

int16_t remote_dr16::get_ch2() {
    return info.ch2;
}

int16_t remote_dr16::get_ch3() {
    return info.ch3;
}

int8_t remote_dr16::get_s1() {
    return info.s1;
}

int8_t remote_dr16::get_s2() {
    return info.s2;
}

int16_t remote_dr16::get_x() {
    return info.x;
}

int16_t remote_dr16::get_y() {
    return info.y;
}

int16_t remote_dr16::get_z() {
    return info.z;
}

uint8_t remote_dr16::get_press_l() {
    return info.press_l;
}

uint8_t remote_dr16::get_press_r() {
    return info.press_r;
}

uint16_t remote_dr16::get_key() {
    return info.key;
}

int16_t remote_dr16::get_wheel() {
    return info.wheel;
}
remote_dr16 dr16_1;
