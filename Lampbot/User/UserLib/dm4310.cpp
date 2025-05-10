/**
*   @file dm4310.cpp
*   @brief 实现4310/4340类声明的方法
*   @author Wenxin HU
*   @date 25-5-4
*   @veersion 1.0
*   @note
*/

#include "can_base.h"
#include "dm4310.h"

extern can can1;
extern can can2;

//不是使用基类继承的方法，因为传参不够
bool dm4310::initialize() {
    return false;
}

bool dm4310::initialize(PID_Params_t* speed_params, PID_Params_t* angle_params) {
    if (initialized) return false;

    pid_speed.Init();
    pid_angle.Init();

    // PID_Params_t speed_params = {
    //     0.25f, 0.55f, 10.0f, 0.6f,
    //     3.0f, 18.0f
    // };
    pid_speed.Set_params(speed_params);
    pid_speed.Set_DFirst(DFirst_Enable);

    // PID_Params_t angle_params = {
    //     150.0f, 20.0f, 0.0f, 35.0f,
    //     60.0f, 60.0f
    // };
    pid_angle.Set_params(angle_params);
    pid_angle.Set_DFirst(DFirst_Enable);

    initialized = true;
    return true;
}

bool dm4310::enable() {
    if (!initialized) return false;
    if (enabled) return false;

    uint32_t CAN_id = 0xBB;
    uint8_t TxBuffer[8];



    if (ctrl_mode == DM_CtrlMode_SpeedPosition) {
        CAN_id = 0x100 + id;
    }
    else if (ctrl_mode == DM_CtrlMode_Speed) {
        CAN_id = 0x200 + id;
    }
    else if (ctrl_mode == DM_CtrlMode_MIT) {
        CAN_id = 0x000 + id;
    }
    TxBuffer[0] = 0xFF;
    TxBuffer[1] = 0xFF;
    TxBuffer[2] = 0xFF;
    TxBuffer[3] = 0xFF;
    TxBuffer[4] = 0xFF;
    TxBuffer[5] = 0xFF;
    TxBuffer[6] = 0xFF;
    TxBuffer[7] = 0xFC;

    while (info.Err != 1) {
        if (can_handle->Instance == CAN1) {
            can1.tx_data(TxBuffer, CAN_id);
            update();
        }
        else if (can_handle->Instance == CAN2) {
            can2.tx_data(TxBuffer, CAN_id);
            update();
        }
    }

    pid_speed.enable();
    pid_angle.enable();

    enabled = true;
    return true;
}

bool dm4310::disable() {
    if (!initialized) return false;
    if (!enabled) return false;

    pid_speed.disable();
    pid_angle.disable();

    uint32_t CAN_id = 0xBB;
    uint8_t TxBuffer[8];
    if (ctrl_mode == DM_CtrlMode_SpeedPosition) {
        CAN_id = 0x100 + id;
    }
    else if (ctrl_mode == DM_CtrlMode_Speed) {
        CAN_id = 0x200 + id;
    }
    else if (ctrl_mode == DM_CtrlMode_MIT) {
        CAN_id = 0x000 + id;
    }
    TxBuffer[0] = 0xFF;
    TxBuffer[1] = 0xFF;
    TxBuffer[2] = 0xFF;
    TxBuffer[3] = 0xFF;
    TxBuffer[4] = 0xFF;
    TxBuffer[5] = 0xFF;
    TxBuffer[6] = 0xFF;
    TxBuffer[7] = 0xFD;

    while (info.Err != 0) {
        if (can_handle->Instance == CAN1) {
            can1.tx_data(TxBuffer, CAN_id);
        }
        else if (can_handle->Instance == CAN2) {
            can2.tx_data(TxBuffer, CAN_id);
        }
    }

    enabled = false;
    return true;
}

bool dm4310::Ctrl_MIT_Mode(float Kp, float Kd, float pos, float vel, float tor) {
    if (!initialized) return false;
    if (!enabled) return false;

    ctrl_target.position = pos;
    ctrl_target.velocity = vel;
    ctrl_target.torque = tor;
    ctrl_target.Kd = Kd;
    ctrl_target.Kp = Kp;

    uint8_t TxBuffer[8];
    uint32_t CAN_id;
    if (ctrl_mode == DM_CtrlMode_MIT) {
        CAN_id = 0x000 + id;
    }
    else return false;

    if (!started) {
        //如果只是使能但是没有启动，只发送0
        int tmp_pos = 0.0f;
        int tmp_vel = 0.0f;
        int tmp_tor = 0.0f;
        int tmp_kp = 0.0f;
        int tmp_kd = 0.0f;

        TxBuffer[0] = (tmp_pos >> 8);
        TxBuffer[1] = tmp_pos;
        TxBuffer[2] = (tmp_vel >> 4);
        TxBuffer[3] = ((tmp_vel & 0xF) << 4) | (tmp_kp >> 8);
        TxBuffer[4] = tmp_kp;
        TxBuffer[5] = (tmp_kd >> 4);
        TxBuffer[6] = ((tmp_kd & 0xF) << 4) | (tmp_tor >> 8);
        TxBuffer[7] = tmp_tor;

        if (can_handle->Instance == CAN1) {
            can1.tx_data(TxBuffer, CAN_id);
        }
        else if (can_handle->Instance == CAN2) {
            can2.tx_data(TxBuffer, CAN_id);
        }

        return false;
    }
    int tmp_pos = float_to_uint(ctrl_target.position, -12.5f, 12.5f, 16);
    int tmp_vel = 0;
    int tmp_tor = 0;

    if (type == DM_4310) {
        tmp_vel = float_to_uint(ctrl_target.velocity, -45, 45, 12);
        tmp_tor = float_to_uint(ctrl_target.torque, -18, 18, 12);
    }
    else if (type == DM_4340) {
        tmp_vel = uint_to_float(ctrl_target.velocity, -10, 10, 12); // (-10.0,10.0
        tmp_tor = uint_to_float(ctrl_target.torque, -28, 28, 12);   // (-28.0,28.0)
    }
    else return false;

    int tmp_kp = float_to_uint(ctrl_target.Kp, 0, 500.0f, 12);
    int tmp_kd = float_to_uint(ctrl_target.Kd, 0, 5.0f, 12);

    TxBuffer[0] = (tmp_pos >> 8);
    TxBuffer[1] = tmp_pos;
    TxBuffer[2] = (tmp_vel >> 4);
    TxBuffer[3] = ((tmp_vel & 0xF) << 4) | (tmp_kp >> 8);
    TxBuffer[4] = tmp_kp;
    TxBuffer[5] = (tmp_kd >> 4);
    TxBuffer[6] = ((tmp_kd & 0xF) << 4) | (tmp_tor >> 8);
    TxBuffer[7] = tmp_tor;

    if (can_handle->Instance == CAN1) {
        can1.tx_data(TxBuffer, CAN_id);
    }
    else if (can_handle->Instance == CAN2) {
        can2.tx_data(TxBuffer, CAN_id);
    }

    return true;
}

bool dm4310::Ctrl_SpeedPosition_Mode(float pos, float vel) {
    if (!initialized) return false;
    if (!enabled) return false;

    ctrl_target.position = pos;
    ctrl_target.velocity = vel;

    uint8_t TxBuffer[8];
    uint32_t CAN_id;
    if (ctrl_mode == DM_CtrlMode_SpeedPosition) {
        CAN_id = 0x100 + id;
    }
    else return false;

    if (!started) {
        //如果只是使能但是没有启动，只发送0
        ctrl_target.position = 0.0f;
        ctrl_target.velocity = 0.0f;

        *(float*)TxBuffer = ctrl_target.position;
        *(float*)(TxBuffer + 4) = ctrl_target.velocity;

        if (can_handle->Instance == CAN1) {
            can1.tx_data(TxBuffer, CAN_id);
        }
        else if (can_handle->Instance == CAN2) {
            can2.tx_data(TxBuffer, CAN_id);
        }

        return false;
    }

    *(float*)TxBuffer = ctrl_target.position;
    *(float*)(TxBuffer + 4) = ctrl_target.velocity;

    if (can_handle->Instance == CAN1) {
        can1.tx_data(TxBuffer, CAN_id);
    }
    else if (can_handle->Instance == CAN2) {
        can2.tx_data(TxBuffer, CAN_id);
    }

    return true;
}

bool dm4310::Ctrl_SpeedMode(float vel) {
    if (!initialized) return false;
    if (!enabled) return false;

    ctrl_target.velocity = vel;

    uint8_t TxBuffer[8];
    uint32_t CAN_id;
    if (ctrl_mode == DM_CtrlMode_Speed) {
        CAN_id = 0x200 + id;
    }
    else return false;

    if (!started) {
        //如果只是使能但是没有启动，只发送0
        ctrl_target.velocity = 0.0f;

        *(float*)TxBuffer = ctrl_target.velocity;

        if (can_handle->Instance == CAN1) {
            can1.tx_data(TxBuffer, CAN_id);
        }
        else if (can_handle->Instance == CAN2) {
            can2.tx_data(TxBuffer, CAN_id);
        }

        return false;
    }

    *(float*)TxBuffer = ctrl_target.velocity;

    if (can_handle->Instance == CAN1) {
        can1.tx_data(TxBuffer, CAN_id);
    }
    else if (can_handle->Instance == CAN2) {
        can2.tx_data(TxBuffer, CAN_id);
    }

    return true;
}

bool dm4310::start() {
    if (!initialized) return false;
    if (!enabled) return false;
    if (started) return false;

    if (ctrl_mode == DM_CtrlMode_MIT) {
        Ctrl_MIT_Mode(0.0f,0.0f,0.0f,0.0f,0.0f);
    }
    else if (ctrl_mode == DM_CtrlMode_Speed) {
        Ctrl_SpeedMode(0.0f);
    }
    else if (ctrl_mode == DM_CtrlMode_SpeedPosition) {
        Ctrl_SpeedPosition_Mode(0.0f, 0.0f);
    }
    else return false;

    pid_angle.start();
    pid_speed.start();

    started = true;
    return true;
}

bool dm4310::stop() {
    if (!initialized) return false;
    if (!enabled) return false;
    if (!started) return false;

    pid_angle.stop();
    pid_speed.stop();
    
    if (ctrl_mode == DM_CtrlMode_MIT) {
        Ctrl_MIT_Mode(0.0f,0.0f,0.0f,0.0f,0.0f);
    }
    else if (ctrl_mode == DM_CtrlMode_Speed) {
        Ctrl_SpeedMode(0.0f);
    }
    else if (ctrl_mode == DM_CtrlMode_SpeedPosition) {
        Ctrl_SpeedPosition_Mode(0.0f, 0.0f);
    }
    else return false;

    started = false;

    return true;
}

bool dm4310::buffer_update(uint8_t buf[], uint32_t can_id) {
    if (!initialized) return false;
    //只有更新了之后才释放锁允许写入
    // if (updated) return false;

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

bool dm4310::update() {
    if (!initialized) return false;
    // if (updated) return false;
    if ((rx_buffer[0] & 0x0F) != id) return false;

    const int p_int = (rx_buffer[1] << 8) | rx_buffer[2];
    const int v_int = (rx_buffer[3] << 4) | (rx_buffer[4] >> 4);
    const int t_int = ((rx_buffer[4] & 0xF) << 8) | rx_buffer[5];
    info.position_raw = uint_to_float(p_int, -12.5f, 12.5f, 16); // (-12.5,12.5)

    switch (type) {
        case DM_4310: {
            speed = uint_to_float(v_int, -45, 45, 12); // (-45.0,45.0)
            torque = uint_to_float(t_int, -18, 18, 12); // (-18.0,18.0)
            break;
        }
        case DM_4340: {
            speed = uint_to_float(v_int, -10, 10, 12); // (-10.0,10.0)
            torque = uint_to_float(t_int, -28, 28, 12); // (-28.0,28.0)
            break;
        }
    }

    if(info.position_raw > 6.25f) angle = info.position_raw-6.25f;
    else if (info.position_raw < 0.0f) angle = info.position_raw+6.25f;
    else angle = info.position_raw;
    if(angle> 6.25f) angle = angle-6.25f;
    else if (angle < 0.0f) angle = angle+6.25f;

    info.Err = (rx_buffer[0] & 0xF0) >> 4;
    if (info.Err == 1) enabled = true;
    else if (info.Err == 0) {
        enabled = false;
    }

    return true;
}

bool dm4310::Servo_Speed(float Speed, float Max_Speed) {
    if (!initialized) return false;
    if (!enabled) return false;
    if (!started) return false;

    Math_Constrain(Max_Speed, 0.0f, 60.0f);
    float Max = Max_Speed;
    float Min = -Max_Speed;
    Math_Constrain(Speed, Min, Max);

    pid_speed.Set_Now(speed);
    pid_speed.Set_Target(Speed);
    ctrl_target.velocity = Speed;
    pid_speed.TIM_Adjust_PeriodElapsedCallback();
    Ctrl_MIT_Mode(0.0f, 0.0f, 0.0f, 0.0f, pid_speed.Get_Out());

    return true;
}

bool dm4310::Servo_Angle(float Angle, float Max_Speed, float Max_angle, float Min_angle) {
    if (!initialized) return false;
    if (!enabled) return false;
    if (!started) return false;

    Math_Constrain(Max_angle, 0.0f, 2.0f*PI);
    Math_Constrain(Min_angle, 0.0f, 2.0f*PI);
    float Max = Max_angle;
    float Min = Min_angle;
    Math_Constrain(Angle, Min, Max);

    pid_angle.Set_Now(angle);
    pid_angle.Set_Target(Angle);
    ctrl_target.position = Angle;
    pid_angle.TIM_Adjust_PeriodElapsedCallback();
    Servo_Speed(pid_angle.Get_Out(),Max_Speed);

    return true;
}

dm4310 dm4310_1{DM_4310,1,&hcan1,DM_CtrlMode_SpeedPosition};
dm4310 dm4340_2{DM_4340,2,&hcan1,DM_CtrlMode_SpeedPosition};
dm4310 dm4310_3{DM_4310,3,&hcan1,DM_CtrlMode_SpeedPosition};
dm4310 dm4310_4{DM_4310,4,&hcan1,DM_CtrlMode_MIT};
