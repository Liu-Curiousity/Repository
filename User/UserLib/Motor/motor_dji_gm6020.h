/**
 * @brief 		motor_DJI_GM6020.h库文件
 * @detail
 * @author 	    Haoqi Liu
 * @date        25-5-2
 * @version 	V1.0.0
 * @note 		
 * @warning	    
 * @par 		历史版本
                V1.0.0创建于25-5-2
 * */

#ifndef MOTOR_DJI_GM6020_H
#define MOTOR_DJI_GM6020_H

#include "numbers"
#include "motor_base.h"
#include "can.h"

using namespace std;

class gm6020 final : public bldc_motor {
public:
    enum CtrlMode {
        CtrlMode_Voltage = 0,
        CtrlMode_Current = 1,
    };

    gm6020() = delete;

    gm6020(CAN_HandleTypeDef *hcan, uint32_t id) : hcan(hcan) {
        this->id = id;
    }

    CtrlMode ctrl_mode{CtrlMode_Current};

    bool initialize() override;
    bool enable() override;
    bool disable() override;
    bool start() override;
    bool stop() override;
    void update(uint8_t *data);
    void set_torque(float torque) override;

private:
    CAN_HandleTypeDef *hcan; // CAN句柄
};

#endif //MOTOR_DJI_GM6020_H
