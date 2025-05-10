/**
*   @file light.cpp
*   @brief 实现灯盘类的方法
*   @author Wenxin HU
*   @date 25-5-8
*   @veersion 1.0
*   @note
*/

#include "light.h"
#include "can_base.h"
#

extern can can1;
extern can can2;

bool light::initialize() {
    initialized = true;

    return true;
}

bool light::ctrl(uint16_t channel1, uint16_t channel2) {
    if (!initialized) return false;

    channel[0] = channel1;
    channel[1] = channel2;

    uint8_t tx_buffer[8];
    *(uint16_t*)tx_buffer = channel[0];
    *(uint16_t*)(tx_buffer + 2) = channel[1];

    if (hcan->Instance == CAN1) {
        can1.tx_data(tx_buffer, 0x150);
    }
    else if (hcan->Instance == CAN2) {
        can2.tx_data(tx_buffer, 0x150);
    }

    return true;
}


bool light::enable() {
    if (!initialized) return false;
    if (enabled) return false;

    ctrl(0,0);

    enabled = true;
    return true;
}

bool light::disable() {
    if (!initialized) return false;
    if (!enabled) return false;

    ctrl(0,0);

    enabled = false;
    return true;
}

bool light::start() {
    if (!initialized) return false;
    if (!enabled) return false;
    if (started) return false;

    ctrl(0,0);

    started = true;
    return true;
}

bool light::stop() {
    if (!initialized) return false;
    if (!enabled) return false;
    if (!started) return false;

    ctrl(0,0);

    started = false;
    return true;
}

bool light::ctrl_light(uint16_t temperature, uint16_t light) {
    if (!initialized) return false;
    if (!enabled) return false;
    if (!started) return false;

    const uint16_t channel1 = (temperature - 3000) / 3500.0 * light;
    const uint16_t channel2 = (6500 - temperature) / 3500.0 * light;
    channel[0] = (channel1 > 1000 ? 1000 : channel1) < 0 ? 0 : channel1 > 1000 ? 1000 : channel1;
    channel[1] = (channel2 > 1000 ? 1000 : channel2) < 0 ? 0 : channel2 > 1000 ? 1000 : channel2;

    ctrl(channel1,channel2);

    return true;
}


light light1(&hcan1);
