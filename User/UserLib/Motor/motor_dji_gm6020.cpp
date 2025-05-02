#include "motor_dji_gm6020.h"

bool gm6020::initialize() {
    initialized = true;
    return true;
}

bool gm6020::enable() {
    if (!initialized) return false;
    enabled = true;
    return true;
}

bool gm6020::disable() {
    if (!initialized) return false;
    if (enabled) enabled = false;
    return true;
}

bool gm6020::start() {
    if (!initialized || !enabled) return false;
    if (!started) {
        started = true;
    }
    return true;
}

bool gm6020::stop() {
    if (!initialized || !enabled) return false;
    if (started) {}
    return true;
}

void gm6020::update(uint8_t *data) {
    angle = (int16_t)((data[0] << 8) | data[1]) / 8192.0f * 2 * numbers::pi_v<float>;
    speed = (int16_t)((data[2] << 8) | data[3]);
    current = ((int16_t)((data[4] << 8) | data[5])) * 20.0f / INT16_MAX;
    temperature = data[6];
}

void gm6020::set_torque(float torque) {
    CAN_TxHeaderTypeDef TxHeader;
    TxHeader.IDE = CAN_ID_STD;   //使用标准数据帧模式
    TxHeader.RTR = CAN_RTR_DATA; //数据帧类型
    TxHeader.DLC = 0x08;         //数据帧长度
    TxHeader.StdId = 0x1FF;

    uint8_t TxBuffer[8]{0};
    uint16_t current = torque;
    switch (id) {
        case 1:
            TxBuffer[0] = current >> 8;
            TxBuffer[1] = current;
            break;
        case 2:
            TxBuffer[2] = current >> 8;
            TxBuffer[3] = current;
            break;
        case 3:
            TxBuffer[4] = current >> 8;
            TxBuffer[5] = current;
            break;
        case 4:
            TxBuffer[6] = current >> 8;
            TxBuffer[7] = current;
            break;
        default:
            break;
    }
    if (HAL_CAN_AddTxMessage(hcan, &TxHeader, TxBuffer, (uint32_t *)CAN_TX_MAILBOX0) != HAL_OK) {
        if (HAL_CAN_AddTxMessage(hcan, &TxHeader, TxBuffer, (uint32_t *)CAN_TX_MAILBOX1) != HAL_OK) {
            HAL_CAN_AddTxMessage(hcan, &TxHeader, TxBuffer, (uint32_t *)CAN_TX_MAILBOX2);
        }
    }
}
