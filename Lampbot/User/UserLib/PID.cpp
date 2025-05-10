/**
*   @file PID.cpp
*   @brief 实现pid类的方法
*   @author Wenxin HU
*   @date 25-5-4
*   @veersion 1.0
*   @note 在系统初始化后
*/

#include "PID.h"

bool PID::Init() {
    if (initialized) return false;

    initialized = true;
    return true;
}

bool PID::enable() {
    if (!initialized) return false;
    if (enabled) return false;

    enabled = true;
    return true;
}

bool PID::disable() {
    if (!initialized) return false;
    if (!enabled) return false;

    enabled = false;
    return true;
}

bool PID::start() {
    if (!initialized) return false;
    if (!enabled) return false;

    info.Now = 0.0f;
    info.Out = 0.0f;
    info.Target = 0.0f;
    info.Error = 0.0f;
    info.Integral_Error = 0.0f;

    info.Pre_Error = 0.0f;
    info.Pre_Now = 0.0f;
    info.Pre_Target = 0.0f;
    info.Pre_Out = 0.0f;

    started = true;
    return true;
}

bool PID::stop() {
    if (!initialized) return false;
    if (!enabled) return false;
    if (!started) return false;

    info.Now = 0.0f;
    info.Out = 0.0f;
    info.Target = 0.0f;
    info.Error = 0.0f;
    info.Integral_Error = 0.0f;

    info.Pre_Error = 0.0f;
    info.Pre_Now = 0.0f;
    info.Pre_Target = 0.0f;
    info.Pre_Out = 0.0f;

    started = false;
    return true;
}

float PID::Get_Integral_Error() {
    if (!initialized) return false;
    if (!enabled) return false;
    if (!started) return false;

    return info.Integral_Error;
}

float PID::Get_Out() {
    if (!initialized) return false;
    if (!enabled) return false;
    if (!started) return false;

    return info.Out;
}

bool PID::Set_params(PID_Params_t* Params) {
    if (!initialized) return false;

    params.K_P = Params->K_P;
    params.K_I = Params->K_I;
    params.K_D = Params->K_D;
    params.K_F = Params->K_F;
    params.I_Out_Max = Params->I_Out_Max;
    params.Out_Max = Params->Out_Max;
    params.I_Variable_Speed_A = Params->I_Variable_Speed_A;
    params.I_Variable_Speed_B = Params->I_Variable_Speed_B;
    params.I_Separate_Threshold = Params->I_Separate_Threshold;
    params.D_T = Params->D_T;
    params.Dead_Zone = Params->Dead_Zone;
    params.BangBang_Threshold = Params->BangBang_Threshold;
    params.D_First = Params->D_First;

    return true;
}

bool PID::Set_K_P(float K_P) {
    if (!initialized) return false;

    params.K_P = K_P;

    return true;
}

bool PID::Set_K_I(float K_I) {
    if (!initialized) return false;

    params.K_I = K_I;

    return true;
}

bool PID::Set_K_D(float K_D) {
    if (!initialized) return false;

    params.K_D = K_D;

    return true;
}

bool PID::Set_K_F(float K_F) {
    if (!initialized) return false;

    params.K_F = K_F;

    return true;
}

bool PID::Set_I_Out_Max(float I_Out_Max) {
    if (!initialized) return false;

    params.I_Out_Max = I_Out_Max;

    return true;
}

bool PID::Set_Out_Max(float Out_Max) {
    if (!initialized) return false;

    params.Out_Max = Out_Max;

    return true;
}

bool PID::Set_I_Separate_Threshold(float I_Separate_Threshold) {
    if (!initialized) return false;

    params.I_Separate_Threshold = I_Separate_Threshold;

    return true;
}

bool PID::Set_I_Variable_Speed_A(float Variable_Speed_I_A) {
    if (!initialized) return false;

    params.I_Variable_Speed_A = Variable_Speed_I_A;

    return true;
}

bool PID::Set_I_Variable_Speed_B(float Variable_Speed_B) {
    if (!initialized) return false;

    params.I_Variable_Speed_B = Variable_Speed_B;

    return true;
}

bool PID::Set_Target(float Target) {
    if (!initialized) return false;
    if (!enabled) return false;

    info.Target = Target;

    return true;
}

bool PID::Set_Now(float Now) {
    if (!initialized) return false;
    if (!enabled) return false;

    info.Now = Now;

    return true;
}

bool PID::Set_DFirst(DFirst DFirst) {
    if (!initialized) return false;

    params.D_First = DFirst;

    return true;
}

bool PID::Set_Integral_Error(float Integral_Error) {
    if (!initialized) return false;

    info.Integral_Error = Integral_Error;

    return true;
}

bool PID::TIM_Adjust_PeriodElapsedCallback() {
    if (!initialized) return false;
    if (!enabled) return false;
    if (!started) return false;

        // P输出
    float p_out = 0.0f;
    // I输出
    float i_out = 0.0f;
    // D输出
    float d_out = 0.0f;
    // F输出
    float f_out = 0.0f;
    //误差
    float error;
    //绝对值误差
    float abs_error;
    //线性变速积分
    float speed_ratio;

    //判断是否需要过圈保护
    switch (type) {
    case speed: break;
    case angle: {
        //过圈保护
        if (info.Target - info.Now > PI) {
            info.Now += 2*PI;
        }
        else if (info.Target - info.Now < -PI) {
            info.Target += 2*PI;
        }
        break;
    }
    }

    error = info.Target - info.Now;
    abs_error = Math_Abs(error);
    info.Error = error;

    //判断死区
    if (abs_error < params.Dead_Zone) {
        info.Target = info.Now;
        error = 0.0f;
        abs_error = 0.0f;
        info.Integral_Error = 0.0f;
    }

    //计算p项

    p_out = params.K_P * error;
    info.P_Out = p_out;

    //计算i项

    if (params.I_Variable_Speed_A == 0.0f && params.I_Variable_Speed_A == 0.0f) {
        //非变速积分
        speed_ratio = 1.0f;
    }
    else {
        //变速积分
        if (abs_error <= params.I_Variable_Speed_B) {
            speed_ratio = 1.0f;
        }
        else if (params.I_Variable_Speed_B < abs_error && abs_error < params.I_Variable_Speed_A + params.
            I_Variable_Speed_B) {
            speed_ratio = (params.I_Variable_Speed_A + params.I_Variable_Speed_B - abs_error) / params.
                I_Variable_Speed_A;
        }
        if (abs_error >= params.I_Variable_Speed_B) {
            speed_ratio = 0.0f;
        }
    }
    //积分限幅
    if (params.I_Out_Max != 0.0f) {
        Math_Constrain(&info.Integral_Error, -params.I_Out_Max / params.K_I,
                       params.I_Out_Max / params.K_I);
    }
    if (params.I_Separate_Threshold == 0.0f) {
        //没有积分分离
        info.Integral_Error += speed_ratio * params.D_T * error;
        i_out = params.K_I * info.Integral_Error;
        info.I_Out = i_out;
    }
    else {
        //积分分离使能
        if (abs_error < params.I_Separate_Threshold) {
            info.Integral_Error += speed_ratio * params.D_T * error;
            i_out = params.K_I * info.Integral_Error;
            info.I_Out = i_out;
        }
        else {
            info.Integral_Error = 0.0f;
            i_out = 0.0f;
            info.I_Out = i_out;
        }
    }


    //计算d项

    if (params.D_First == DFirst_Disable) {
        //没有微分先行
        d_out = params.K_D * (error - info.Pre_Error) / params.D_T;
        info.D_Out = d_out;
    }
    else {
        //微分先行使能
        d_out = params.K_D * (info.Out - info.Pre_Out) / params.D_T;
        info.D_Out = d_out;
    }

    //计算前馈

    //判断是否需要前馈过圈保护
    float temp_Pre_target = info.Pre_Target;
    float temp_Target = info.Target;
    switch (type) {
    case speed: break;
    case angle: {
        //过圈保护
        if (temp_Target - temp_Pre_target > PI) {
            temp_Pre_target += 2*PI;
        }
        else if (temp_Target - temp_Pre_target < -PI) {
            temp_Target += 2*PI;
        }
        break;
    }
    }
    f_out = (temp_Target - temp_Pre_target) * params.K_F;
    info.F_Out = f_out;

    //计算总共的输出

    info.Out = p_out + i_out + d_out + f_out;
    //输出限幅
    if (params.Out_Max != 0.0f) {
        Math_Constrain(&info.Out, -params.Out_Max, params.Out_Max);
    }

    //善后工作
    info.Pre_Now = info.Now;
    info.Pre_Target = info.Target;
    info.Pre_Out = info.Out;
    info.Pre_Error = error;

    if (params.K_I == 0.0f) info.Integral_Error = 0.0f;

    return true;
}

