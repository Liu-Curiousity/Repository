/**
*   @file light.h
*   @brief 灯盘类
*   @author Wenxin HU
*   @date 25-5-8
*   @veersion 1.0
*   @note
*/
#ifndef LIGHT_H
#define LIGHT_H

//以下为默认的台灯配置
#define BASIC_LIGHT (500) //默认光照强度
#define BASIC_TEMPERATURE (5300) //默认色温

#include "can.h"

class light {
public:
    explicit light(CAN_HandleTypeDef* hcan) : hcan(hcan) {
    };
    ~light() = default;

    bool initialized{false};
    bool enabled{false};
    bool started{false};

    bool initialize();
    bool enable();
    bool disable();
    bool start();
    bool stop();

    bool on_slowly();
    bool off_slowly();
    bool ctrl(uint16_t channel1, uint16_t channel2);
    bool ctrl_light(uint16_t temperature, uint16_t light);

private:
    uint16_t channel[2] = {}; //两个不同色温的灯分别是一个通道，连读范围0-1000
    CAN_HandleTypeDef* hcan;
};


#endif //LIGHT_H
