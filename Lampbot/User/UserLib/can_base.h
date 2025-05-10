/**
*   @file can_base.h
*   @brief 声明CAN类
*   @author Wenxin HU
*   @date 25-5-4
*   @veersion 1.0
*   @note
*/

#ifndef CAN_H
#define CAN_H

#include "can.h"

class can {
public:
    can(CAN_HandleTypeDef* hcan) : hcan(hcan) {
    };
    ~can() = default;

    bool initialized{false};
    bool enabled{false};
    bool started{false};

    bool initialize();
    bool enable();
    bool disable();
    bool start();
    bool stop();

    bool tx_data(uint8_t TxData[], uint32_t CAN_id);
    bool rx_data();

    bool get_data(uint8_t data[], uint32_t* can_id);

    uint8_t tx_buffer[8] = {};
    uint32_t tx_id{0x00};
    uint8_t rx_buffer[8] = {};
    uint32_t rx_id{0x00};
    CAN_HandleTypeDef* hcan;
};


#endif //CAN_H
