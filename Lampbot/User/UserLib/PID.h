/**
*   @file 
*   @brief 
*   @author Wenxin HU
*   @date 25-5-4
*   @veersion 1.0
*   @note
*/

#ifndef PID_H
#define PID_H

#include "alg_math.h"

enum DFirst {
    DFirst_Disable = 0,
    DFirst_Enable = 1,
};

enum PID_Type {
    speed = 1,
    angle = 2,
};

typedef struct {
    float K_P = 0.0f;
    float K_I = 0.0f;
    float K_D = 0.0f;
    float K_F = 0.0f;

    //积分限幅, 0为不限制
    float I_Out_Max = 0;
    //输出限幅, 0为不限制
    float Out_Max = 0;

    //变速积分定速内段阈值, 0为不限制
    float I_Variable_Speed_A = 0.0f;
    //变速积分变速区间, 0为不限制
    float I_Variable_Speed_B = 0.0f;
    //积分分离阈值，需为正数, 0为不限制
    float I_Separate_Threshold = 0.0f;

    // PID计时器周期, s
    float D_T = 0.001f;
    //死区, Error在其绝对值内不输出
    float Dead_Zone = 0.0f;
    //Bang-bang控制区间
    float BangBang_Threshold = 0.0f;
    //微分先行
    DFirst D_First = DFirst_Disable;
} PID_Params_t;

typedef struct {
    //之前的当前值
    float Pre_Now = 0.0f;
    //之前的目标值
    float Pre_Target = 0.0f;
    //之前的输出值
    float Pre_Out = 0.0f;
    //前向误差
    float Pre_Error = 0.0f;

    float Error = 0.0f;
    float P_Out = 0.0f;
    float I_Out = 0.0f;
    float D_Out = 0.0f;
    float F_Out = 0.0f;

    //目标值
    float Target = 0.0f;
    //当前值
    float Now = 0.0f;
    //积分值
    float Integral_Error = 0.0f;

    float Out = 0.0f;
} PID_Info_t;

class PID {
public:
    PID(PID_Type pid_type,
        float K_P, float K_I, float K_D, float K_F,
        float I_Out_Max, float Out_Max,
        float D_T, float Dead_Zone,
        float I_Variable_Speed_A,
        float I_Variable_Speed_B, float I_Separate_Threshold,
        DFirst D_First)
        : type(pid_type) {
        params.K_P = K_P;
        params.K_I = K_I;
        params.K_D = K_D;
        params.K_F = K_F;
        params.I_Out_Max = I_Out_Max;
        params.Out_Max = Out_Max;
        params.D_T = D_T;
        params.Dead_Zone = Dead_Zone;
        params.I_Variable_Speed_A = I_Variable_Speed_A;
        params.I_Variable_Speed_B = I_Variable_Speed_B;
        params.I_Separate_Threshold = I_Separate_Threshold;
        params.D_First = D_First;
    }

    ~PID() = default;


    bool initialized{false};
    bool enabled{false};
    bool started{false};

    PID_Params_t params;
    PID_Info_t info;
    PID_Type type{speed};

    bool Init();
    bool enable();
    bool disable();
    bool start();
    bool stop();

    float Get_Integral_Error();
    float Get_Out();
    //pid基础值设置
    bool Set_params(PID_Params_t* Params);
    bool Set_K_P(float K_P);
    bool Set_K_I(float K_I);
    bool Set_K_D(float K_D);
    bool Set_K_F(float K_F);
    //pid输出限幅
    bool Set_I_Out_Max(float I_Out_Max);
    bool Set_Out_Max(float Out_Max);
    //变速积分设置
    bool Set_I_Variable_Speed_A(float Variable_Speed_I_A);
    bool Set_I_Variable_Speed_B(float Variable_Speed_I_B);
    //积分分离阈值
    bool Set_I_Separate_Threshold(float I_Separate_Threshold);
    //设置目标/当前值
    bool Set_Target(float Target);
    bool Set_Now(float Now);
    //设置积分误差：在控制器断电的时候防止误差累计在通电的时候产生一个很大的I
    bool Set_Integral_Error(float Integral_Error);
    //设置微分先行
    bool Set_DFirst(DFirst DFirst);

    //用定时器保证PID的时间片精确
    bool TIM_Adjust_PeriodElapsedCallback();
};

#endif //PID_H
