/**
* @file uart_remote.cpp
 * @brief DR16遥控器类
 * @author Wenxin Hu
 * @date 25-5-3
 * @version 1.0
 * @note 实现遥控器类中的所有方法
 * @warning 现在为找到合理实现DMAstop后可以解决DMA接收数据错位的问题，请不要使用stop方法
 */

#include "uart_remote.h"

bool remote_dr16::initialize(UART_HandleTypeDef* handle) {
    if (initialized) return false;

    huart = handle;

    initialized = true;
    return true;
}

bool remote_dr16::start() {
    if (!initialized) return false;

    HAL_UARTEx_ReceiveToIdle_DMA(huart, rx_buffer, sizeof(rx_buffer));

    started = true;
    return true;
}

bool remote_dr16::stop() {
    if (!initialized) return false;
    if (!started) return false;

    // started = false;
    return false;
}

bool remote_dr16::encode_data() {
    if (!initialized) return false;
    if (!started) return false;
    return true;
}

bool remote_dr16::rx_data() {
    if (!initialized) return false;
    if (!started) return false;
    return true;
}

bool remote_dr16::tx_data() {
    if (!initialized) return false;
    if (!started) return false;
    return true;
}

bool remote_dr16::decode_data() {
    if (!initialized) return false;
    if (!started) return false;

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
    if (!initialized) return nullptr;
    if (!started) return nullptr;
    return rx_buffer;
}

remote_t* remote_dr16::get_info() {
    if (!initialized) return nullptr;
    if (!started) return nullptr;
    return &info;
}

uint8_t remote_dr16::get_rx_buffer_size() {
    if (!initialized) return 0;
    if (!started) return 0;
    return sizeof(rx_buffer);
}

int16_t remote_dr16::get_ch0() {
    if (!initialized) return 0;
    if (!started) return 0;
    return info.ch0;
}

int16_t remote_dr16::get_ch1() {
    if (!initialized) return 0;
    if (!started) return 0;
    return info.ch1;
}

int16_t remote_dr16::get_ch2() {
    if (!initialized) return 0;
    if (!started) return 0;
    return info.ch2;
}

int16_t remote_dr16::get_ch3() {
    if (!initialized) return 0;
    if (!started) return 0;
    return info.ch3;
}

int8_t remote_dr16::get_s1() {
    if (!initialized) return 0;
    if (!started) return 0;
    return info.s1;
}

int8_t remote_dr16::get_s2() {
    if (!initialized) return 0;
    if (!started) return 0;
    return info.s2;
}

int16_t remote_dr16::get_x() {
    if (!initialized) return 0;
    if (!started) return 0;
    return info.x;
}

int16_t remote_dr16::get_y() {
    if (!initialized) return 0;
    if (!started) return 0;
    return info.y;
}

int16_t remote_dr16::get_z() {
    if (!initialized) return 0;
    if (!started) return 0;
    return info.z;
}

uint8_t remote_dr16::get_press_l() {
    if (!initialized) return 0;
    if (!started) return 0;
    return info.press_l;
}

uint8_t remote_dr16::get_press_r() {
    if (!initialized) return 0;
    if (!started) return 0;
    return info.press_r;
}

uint16_t remote_dr16::get_key() {
    if (!initialized) return 0;
    if (!started) return 0;
    return info.key;
}

int16_t remote_dr16::get_wheel() {
    if (!initialized) return 0;
    if (!started) return 0;
    return info.wheel;
}

remote_dr16 dr16_1;
