/**
*   @file m2006.cpp
*   @brief 实现M2006电机的方法
*   @author Wenxin HU
*   @date 25-5-6
*   @veersion 1.0
*   @note
*/

#include "m2006.h"
#include "can_base.h"
#include "lib_public.h"

extern can can1;
extern can can2;

bool m2006::initialize() {
    return false;
}

bool m2006::initialize(PID_Params_t* speed_params, PID_Params_t* angle_params) {
    if (initialized) return false;

    pid_speed.Init();
    pid_angle.Init();

    while (!zeroed) Reset_Zero();

    pid_speed.Set_params(speed_params);
    pid_speed.Set_DFirst(DFirst_Enable);

    pid_angle.Set_params(angle_params);
    pid_angle.Set_DFirst(DFirst_Enable);

    initialized = true;
    return true;
}

bool m2006::enable() {
    if (!initialized) return false;
    if (enabled) return false;

    pid_speed.enable();
    pid_angle.enable();

    enabled = true;
    return true;
}

bool m2006::disable() {
    if (!initialized) return false;
    if (!enabled) return false;

    pid_speed.disable();
    pid_angle.disable();

    ctrl_target.current = 0.0f;

    enabled = false;
    return true;
}

bool m2006::buffer_update(uint8_t buf[], uint32_t can_id) {
    if (!initialized) return false;

    rx_buffer[0] = buf[0];
    rx_buffer[1] = buf[1];
    rx_buffer[2] = buf[2];
    rx_buffer[3] = buf[3];
    rx_buffer[4] = buf[4];
    rx_buffer[5] = buf[5];
    rx_buffer[6] = buf[6];
    rx_buffer[7] = buf[7];
    rx_id = can_id;

    return true;
}

bool m2006::update() {
    // if (!initialized) return false;

    raw_angle = (rx_buffer[0] << 8) | rx_buffer[1];
    raw_speed = (rx_buffer[2] << 8) | rx_buffer[3];
    raw_torque = (rx_buffer[4] << 8) | rx_buffer[5];

    angle_calc();

    torque = raw_torque;
    speed = dji_speed_transform(raw_speed, gear_ratio);
    angle = dji_angle_transform(raw_angle, gear_ratio);

    return true;
}

bool m2006::start() {
    if (!initialized) return false;
    if (!enabled) return false;
    if (started) return false;

    pid_angle.start();
    pid_speed.start();

    started = true;

    return true;
}

bool m2006::stop() {
    if (!initialized) return false;
    if (!enabled) return false;
    if (!started) return false;

    pid_angle.stop();
    pid_speed.stop();

    started = false;

    return true;
}

bool m2006::Ctrl_current(float current) {
    //允许在没有回零的时候使用电流控制
    if (!zeroed) {
        Math_Constrain(current, -10000.0f, 10000.0f);

        uint8_t TxBuffer[8];
        TxBuffer[0] = (int16_t)current >> 8;
        TxBuffer[1] = (int16_t)current;
        TxBuffer[2] = 0x0;
        TxBuffer[3] = 0x0;
        TxBuffer[4] = 0x0;
        TxBuffer[5] = 0x0;
        TxBuffer[6] = 0x0;
        TxBuffer[7] = 0x0;

        can1.tx_data(TxBuffer,0x200);

        return true;
    }

    if (!initialized) return false;
    if (!enabled) return false;
    if (!started) return false;

    Math_Constrain(current, -10000.0f, 10000.0f);

    ctrl_target.current = current;

    return true;
}

bool m2006::Servo_speed(float Speed, float Max_Speed) {
    if (!initialized) return false;
    if (!enabled) return false;
    if (!started) return false;

    Math_Constrain(Max_Speed, 0.0f, 1000.0f);
    float Max = Max_Speed;
    float Min = -Max_Speed;
    Math_Constrain(Speed, Min, Max);
    ctrl_target.speed = dji_speed_inverter_transform(Speed, gear_ratio);

    pid_speed.Set_Now(raw_speed);
    pid_speed.Set_Target(ctrl_target.speed);
    pid_speed.TIM_Adjust_PeriodElapsedCallback();

    Ctrl_current(pid_speed.Get_Out());

    return true;
}

bool m2006::Servo_angle(float Angle, float Max_Speed, float Max_angle, float Min_angle) {
    if (!initialized) return false;
    if (!enabled) return false;
    if (!started) return false;
    if (!zeroed) return false;

    Math_Constrain(Max_angle, 0.0f, 2.0f * PI);
    Math_Constrain(Min_angle, 0.0f, 2.0f * PI);
    float Max = Max_angle;
    float Min = Min_angle;
    Math_Constrain(Angle, Min, Max);
    ctrl_target.angle = Angle;

    pid_angle.Set_Now(sum_angle/gear_ratio/8192.0f*2*PI);
    pid_angle.Set_Target(ctrl_target.angle);
    pid_angle.TIM_Adjust_PeriodElapsedCallback();

    Servo_speed(pid_angle.Get_Out(), Max_Speed);

    return true;
}

bool m2006::Reset_Zero() {
    if (initialized) return false;

    delay(100);

    for (uint8_t i = 0; i < 120; i++) {
        float current = -1500;
        Math_Constrain(current, -10000.0f, 10000.0f);

        uint8_t TxBuffer[8];
        TxBuffer[0] = (int16_t)current >> 8;
        TxBuffer[1] = (int16_t)current;
        TxBuffer[2] = 0x0;
        TxBuffer[3] = 0x0;
        TxBuffer[4] = 0x0;
        TxBuffer[5] = 0x0;
        TxBuffer[6] = 0x0;
        TxBuffer[7] = 0x0;

        can1.tx_data(TxBuffer,0x200);
        delay(3);
    }

    while (raw_speed < -100) {
        float current = -1500;
        Math_Constrain(current, -10000.0f, 10000.0f);

        uint8_t TxBuffer[8];
        TxBuffer[0] = (int16_t)current >> 8;
        TxBuffer[1] = (int16_t)current;
        TxBuffer[2] = 0x0;
        TxBuffer[3] = 0x0;
        TxBuffer[4] = 0x0;
        TxBuffer[5] = 0x0;
        TxBuffer[6] = 0x0;
        TxBuffer[7] = 0x0;

        can1.tx_data(TxBuffer,0x200);
        delay(3);
    }

    sum_angle = 0;
    last_angle = raw_angle;
    sum_angle += 5000;

    zeroed = true;
    return true;
}

bool m2006_group::check_initialize() {
    if (m2006_1_Handle != nullptr && !m2006_1_Handle->initialized) return false;
    if (m2006_2_Handle != nullptr && !m2006_2_Handle->initialized) return false;
    if (m2006_3_Handle != nullptr && !m2006_3_Handle->initialized) return false;
    if (m2006_4_Handle != nullptr && !m2006_4_Handle->initialized) return false;

    initialized = true;
    return true;
}

bool m2006_group::check_enable() {
    if (m2006_1_Handle != nullptr && !m2006_1_Handle->enabled) return false;
    if (m2006_2_Handle != nullptr && !m2006_2_Handle->enabled) return false;
    if (m2006_3_Handle != nullptr && !m2006_3_Handle->enabled) return false;
    if (m2006_4_Handle != nullptr && !m2006_4_Handle->enabled) return false;

    enabled = true;
    return true;
}

bool m2006_group::check_start() {
    if (m2006_1_Handle != nullptr && !m2006_1_Handle->started) return false;
    if (m2006_2_Handle != nullptr && !m2006_2_Handle->started) return false;
    if (m2006_3_Handle != nullptr && !m2006_3_Handle->started) return false;
    if (m2006_4_Handle != nullptr && !m2006_4_Handle->started) return false;

    started = true;
    return true;
}

bool m2006_group::buffer_update(uint8_t buf[], uint32_t rx_id) {
    if (!check_initialize()) return false;

    if (hcan->Instance == CAN1) {
        switch (rx_id) {
            case 0x201: m2006_1_Handle->buffer_update(buf, rx_id);
                break;
            case 0x202: m2006_2_Handle->buffer_update(buf, rx_id);
                break;
            case 0x203: m2006_3_Handle->buffer_update(buf, rx_id);
                break;
            case 0x204: m2006_4_Handle->buffer_update(buf, rx_id);
                break;
            default: break;
        }
    }
    else if (hcan->Instance == CAN2) {
        switch (rx_id) {
            case 0x201: m2006_1_Handle->buffer_update(buf, rx_id);
                break;
            case 0x202: m2006_2_Handle->buffer_update(buf, rx_id);
                break;
            case 0x203: m2006_3_Handle->buffer_update(buf, rx_id);
                break;
            case 0x204: m2006_4_Handle->buffer_update(buf, rx_id);
                break;
            default: break;
        }
    }

    return true;
}

bool m2006_group::update() {
    if (!check_initialize()) return false;

    if (m2006_1_Handle != nullptr) m2006_1_Handle->update();
    if (m2006_2_Handle != nullptr) m2006_2_Handle->update();
    if (m2006_3_Handle != nullptr) m2006_3_Handle->update();
    if (m2006_4_Handle != nullptr) m2006_4_Handle->update();

    return true;
}

bool m2006_group::ctrl_current() {
    if (!check_initialize()) return false;
    if (!check_enable()) return false;

    uint8_t TxBuffer[8] = {0};

    if (!check_start()) {
        TxBuffer[0] = 0x01;
        TxBuffer[1] = 0x00;
        TxBuffer[2] = 0x0;
        TxBuffer[3] = 0x0;
        TxBuffer[4] = 0x0;
        TxBuffer[5] = 0x0;
        TxBuffer[6] = 0x0;
        TxBuffer[7] = 0x0;

        if (hcan->Instance == CAN1) {
            can1.tx_data(TxBuffer, tx_id);
        }
        else if (hcan->Instance == CAN2) {
            can2.tx_data(TxBuffer, tx_id);
        }
        else {
            return false;
        }

        return false;
    }


    if (m2006_1_Handle != nullptr) {
        TxBuffer[0] = m2006_1_Handle->ctrl_target.current >> 8;
        TxBuffer[1] = m2006_1_Handle->ctrl_target.current;
    }
    else {
        TxBuffer[0] = 0x0;
        TxBuffer[1] = 0x0;
    }
    if (m2006_2_Handle != nullptr) {
        TxBuffer[2] = m2006_2_Handle->ctrl_target.current >> 8;
        TxBuffer[3] = m2006_2_Handle->ctrl_target.current;
    }
    else {
        TxBuffer[2] = 0x0;
        TxBuffer[3] = 0x0;
    }
    if (m2006_3_Handle != nullptr) {
        TxBuffer[4] = m2006_3_Handle->ctrl_target.current >> 8;
        TxBuffer[5] = m2006_3_Handle->ctrl_target.current;
    }
    else {
        TxBuffer[4] = 0x0;
        TxBuffer[5] = 0x0;
    }
    if (m2006_4_Handle != nullptr) {
        TxBuffer[6] = m2006_4_Handle->ctrl_target.current >> 8;
        TxBuffer[7] = m2006_4_Handle->ctrl_target.current;
    }
    else {
        TxBuffer[6] = 0x0;
        TxBuffer[7] = 0x0;
    }

    if (hcan->Instance == CAN1) {
        can1.tx_data(TxBuffer, tx_id);
    }
    else if (hcan->Instance == CAN2) {
        can2.tx_data(TxBuffer, tx_id);
    }
    else {
        return false;
    }

    return true;
}

bool m2006::angle_calc() {
    if (!initialized) return false;

    int16_t delta_angle = 0;
    if (raw_angle - last_angle > 6000) delta_angle = raw_angle - last_angle - 8192;
    else if (raw_angle - last_angle < -6000) delta_angle = raw_angle - last_angle + 8192;
    else delta_angle = raw_angle - last_angle;

    if (sum_angle + delta_angle > 8191 * 36) sum_angle = sum_angle + delta_angle - 8191*36;
    else if (sum_angle + delta_angle < 0) sum_angle = sum_angle + delta_angle + 8191*36;
    else sum_angle = sum_angle + delta_angle;

    // if (!zeroed) {
    //     sum_angle = 0;
    //     zeroed = true;
    // }

    last_angle = raw_angle;

    return true;
}

m2006 m2006_1(1, &hcan1);

m2006_group m2006_group_1(&m2006_1, nullptr, nullptr, nullptr,
                          0x200, &hcan1);
