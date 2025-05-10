/**
*   @file can_base.cpp
*   @brief 实现CAN类的方法
*   @author Wenxin HU
*   @date 25-5-4
*   @veersion 1.0
*   @note
*/

#include "can_base.h"
#include "can.h"
#include "dm4310.h"
#include "m2006.h"
#include "lib_public.h"

bool can::initialize() {
    if (initialized) return false;

    if (hcan->Instance == CAN1){
        CAN_FilterTypeDef can1_filter_st;
        can1_filter_st.FilterIdHigh = 0x0000;
        can1_filter_st.FilterIdLow = 0x0000;
        can1_filter_st.FilterMaskIdHigh = 0x0000;
        can1_filter_st.FilterMaskIdLow = 0x0000;
        can1_filter_st.FilterFIFOAssignment = CAN_RX_FIFO0;
        can1_filter_st.FilterActivation = ENABLE;
        can1_filter_st.FilterMode = CAN_FILTERMODE_IDMASK;
        can1_filter_st.FilterScale = CAN_FILTERSCALE_32BIT;
        can1_filter_st.FilterBank = 0;
        can1_filter_st.SlaveStartFilterBank = 14;

        HAL_CAN_ConfigFilter(hcan, &can1_filter_st);
        HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
        HAL_CAN_Start(hcan);

        initialized = true;
        return true;
    }else if (hcan->Instance == CAN2) {
        CAN_FilterTypeDef can2_filter_st;
        can2_filter_st.FilterIdHigh = 0x0000;
        can2_filter_st.FilterIdLow = 0x0000;
        can2_filter_st.FilterMaskIdHigh = 0x0000;
        can2_filter_st.FilterMaskIdLow = 0x0000;
        can2_filter_st.FilterFIFOAssignment = CAN_RX_FIFO1;
        can2_filter_st.FilterActivation = ENABLE;
        can2_filter_st.FilterMode = CAN_FILTERMODE_IDMASK;
        can2_filter_st.FilterScale = CAN_FILTERSCALE_32BIT;
        can2_filter_st.FilterBank = 14;
        can2_filter_st.SlaveStartFilterBank = 14;

        HAL_CAN_ConfigFilter(hcan, &can2_filter_st);
        HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO1_MSG_PENDING);
        HAL_CAN_Start(hcan);

        initialized = true;
        return true;
    }else {
        return false;
    }

}

bool can::enable() {
    if (!initialized) return false;
    if (enabled) return false;

    enabled = true;
    return true;
}

bool can::disable() {
    if (!initialized) return false;
    if (!enabled) return false;

    enabled = false;
    return true;
}

bool can::start() {
    if (!initialized) return false;
    if (!enabled) return false;
    if (started) return false;

    HAL_CAN_Start(hcan);

    started = true;
    return true;
}

bool can::stop() {
    if (!initialized) return false;
    if (!enabled) return false;
    if (!started) return false;

    HAL_CAN_Stop(hcan);

    started = false;
    return true;
}

bool can::rx_data() {
    if (!initialized) return false;
    if (!enabled) return false;
    if (!started) return false;

    CAN_RxHeaderTypeDef RxHeader;

    if (hcan->Instance == CAN1) {
        HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, rx_buffer);
        rx_id = RxHeader.StdId;
        return true;
    }else if (hcan->Instance == CAN2) {
        HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &RxHeader, rx_buffer);
        rx_id = RxHeader.StdId;
        return true;
    }else{
        return false;
    }
}

bool can::tx_data(uint8_t TxData[], uint32_t CAN_id) {
    if (!initialized) return false;
    if (!enabled) return false;
    if (!started) return false;

    CAN_TxHeaderTypeDef TxHeader;

    TxHeader.IDE = CAN_ID_STD;   //使用标准数据帧模式
    TxHeader.RTR = CAN_RTR_DATA; //数据帧类型
    TxHeader.DLC = 0x08;         //数据帧长度

    TxHeader.StdId = CAN_id;
    tx_buffer[0] = TxData[0];
    tx_buffer[1] = TxData[1];
    tx_buffer[2] = TxData[2];
    tx_buffer[3] = TxData[3];
    tx_buffer[4] = TxData[4];
    tx_buffer[5] = TxData[5];
    tx_buffer[6] = TxData[6];
    tx_buffer[7] = TxData[7];

    //逐个信箱发送
    if (HAL_CAN_AddTxMessage(hcan, &TxHeader, tx_buffer, (uint32_t*)CAN_TX_MAILBOX0) != HAL_OK) {
        if (HAL_CAN_AddTxMessage(hcan, &TxHeader, tx_buffer, (uint32_t*)CAN_TX_MAILBOX1) != HAL_OK) {
            HAL_CAN_AddTxMessage(hcan, &TxHeader, tx_buffer, (uint32_t*)CAN_TX_MAILBOX2);
        }
    }

    return true;
}


bool can::get_data(uint8_t* data, uint32_t* can_id) {
    if (!initialized) return false;
    if (!enabled) return false;
    if (!started) return false;

    *can_id = rx_id;
    data[0] = rx_buffer[0];
    data[1] = rx_buffer[1];
    data[2] = rx_buffer[2];
    data[3] = rx_buffer[3];
    data[4] = rx_buffer[4];
    data[5] = rx_buffer[5];
    data[6] = rx_buffer[6];
    data[7] = rx_buffer[7];

    return true;
}

can can1(&hcan1);
can can2(&hcan2);

extern dm4310 dm4310_1;
extern dm4310 dm4340_2;
extern dm4310 dm4310_4;
extern m2006_group m2006_group_1;

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    if (hcan->Instance != CAN1) return;
    can1.rx_data();
    switch(can1.rx_id) {
        case 0x201:case 0x202:case 0x203:case 0x204: {
            m2006_group_1.buffer_update(can1.rx_buffer,can1.rx_id);
            break;
        }
        case 0x205:case 0x206:case 0x207:case 0x208: {
            // GM6020_All1.Update();
            break;
        }
        case 0x011:case 0x111:case 0x211:{
            dm4310_1.buffer_update(can1.rx_buffer,can1.rx_id);
            break;
        }
        case 0x012:case 0x112:case 0x212:{
            // DM4310_2.Update();
            dm4340_2.buffer_update(can1.rx_buffer,can1.rx_id);
            break;
        }
        case 0x013:case 0x113:case 0x213:{
            // DM4310_3.Update();
            break;
        }
        case 0x014:case 0x114:case 0x214:{
            dm4310_4.buffer_update(can1.rx_buffer,can1.rx_id);
            break;
        }
        default: return;
    }
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    if (hcan->Instance != CAN2) return;
    can2.rx_data();
}