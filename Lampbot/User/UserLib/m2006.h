/**
*   @file m2006.h
*   @brief 声明M2006电机类
*   @author Wenxin HU
*   @date 25-5-6
*   @veersion 1.0
*   @note
*/

#ifndef M2006_H
#define M2006_H

#include "can.h"
#include "motor_base.h"
#include "PID.h"

typedef struct {
    float speed = 0.0f, angle = 0.0f;
    int16_t current = 0.0f;
} CtrlTarget_t;

class m2006 final : public motor {
public:
    explicit m2006(uint8_t _id, CAN_HandleTypeDef* _hcan)
        : hcan(_hcan),
          pid_speed(PID_Type::speed, 0.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.001f, 0.0f,
                    0.0f, 0.0f, 0.0f,
                    DFirst_Disable),
          pid_angle(PID_Type::angle, 0.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.001f, 0.05f,
                    0.0f, 0.0f, 0.0f,
                    DFirst_Disable) {
        id = _id;
    }

    CtrlTarget_t ctrl_target = {}; //控制目标直接针对电机传回的原始值
    PID pid_speed;
    PID pid_angle;
    bool zeroed{false};

    bool initialize() override;
    bool initialize(PID_Params_t* speed_params, PID_Params_t* angle_params); // true 成功，false 失败
    bool enable() override;                                                  // true 成功，false 失败
    bool disable() override;                                                 // true 成功，false 失败
    bool start() override;                                                   // true 成功，false 失败
    bool stop() override;                                                    // true 成功，false 失败
    bool update() override;                                                  // true 成功，false 失败

    bool buffer_update(uint8_t buf[], uint32_t rx_id);
    bool Reset_Zero();
    bool Ctrl_current(float current);                                                 //电机对象仅作值传递，group对象发报
    bool Servo_speed(float Speed, float Max_Speed);                                   //电机对象仅作值传递，group对象发报
    bool Servo_angle(float Angle, float Max_Speed, float Max_angle, float Min_angle); //电机对象仅作值传递，group对象发报
    bool angle_calc();

private:
    CAN_HandleTypeDef* hcan;
    uint16_t raw_angle{0};
    uint16_t last_angle{0};
    int32_t sum_angle{0};
    int16_t raw_speed{0};
    int16_t raw_torque{0};
    uint8_t gear_ratio{36};
};

class m2006_group final : public motor_group {
public:
    m2006_group(m2006* m2006_1, m2006* m2006_2, m2006* m2006_3, m2006* m2006_4, uint32_t can_id,
                CAN_HandleTypeDef* hcan)
        : m2006_1_Handle(m2006_1), m2006_2_Handle(m2006_2), m2006_3_Handle(m2006_3), m2006_4_Handle(m2006_4),
          tx_id(can_id), hcan(hcan) {
    }

    int16_t target_current[4] = {}; //四个电机的目标

    bool check_enable() override;
    bool check_start() override;
    bool check_initialize() override;

    bool update() override;
    bool buffer_update(uint8_t buf[], uint32_t rx_id) override;
    bool ctrl_current() override;

private:
    m2006* m2006_1_Handle{nullptr};
    m2006* m2006_2_Handle{nullptr};
    m2006* m2006_3_Handle{nullptr};
    m2006* m2006_4_Handle{nullptr};

    uint32_t tx_id{0x00};
    CAN_HandleTypeDef* hcan;
};

#endif //M2006_H
