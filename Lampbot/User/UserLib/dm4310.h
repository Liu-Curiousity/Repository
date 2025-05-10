/**
*   @file dm4310.h
*   @brief 声明了dm4310类
*   @author Wenxin HU
*   @date 25-5-4
*   @veersion 1.0
*   @note
*/

#ifndef DM4310_H
#define DM4310_H

#include "motor_base.h"
#include "can.h"
#include "pid.h"

typedef enum {
    DM_CtrlMode_MIT = 0,
    DM_CtrlMode_SpeedPosition = 1,
    DM_CtrlMode_Speed = 2,
} DM_CtrlMode;

typedef enum {
    DM_4310 = 1,
    DM_4340 = 2,
} DM_Type;

typedef struct {
    float position = 0.0f;
    float position_raw = 0.0f;
    float velocity = 0.0f;
    float torque = 0.0f;
    bool Err{false};
} DM4310_t;

typedef struct {
    float position = 0.0f, velocity = 0.0f, torque = 0.0f,
          Kp = 0.0f, Kd = 0.0f;
} DM_CtrlTarget;

class dm4310 final : public motor {
public:
    dm4310(DM_Type _motor_type, uint8_t _id, CAN_HandleTypeDef* _hcan, DM_CtrlMode _ctrl_mode)
        : type(_motor_type), can_handle(_hcan), ctrl_mode(_ctrl_mode),
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

    DM_Type type{DM_4310};
    CAN_HandleTypeDef* can_handle{nullptr};
    DM_CtrlMode ctrl_mode{DM_CtrlMode_MIT};
    DM4310_t info;
    PID pid_speed;
    PID pid_angle;

    DM_CtrlTarget ctrl_target;

    bool initialize() override;
    bool initialize(PID_Params_t* speed_params, PID_Params_t* angle_params); // true 成功，false 失败
    bool enable() override;                                                  // true 成功，false 失败
    bool disable() override;                                                 // true 成功，false 失败
    bool start() override;                                                   // true 成功，false 失败
    bool stop() override;                                                    // true 成功，false 失败
    bool update() override;                                                  // true 成功，false 失败

    bool buffer_update(uint8_t buf[], uint32_t rx_id);
    bool Ctrl_MIT_Mode(float Kp, float Kd, float pos, float vel, float tor);
    bool Ctrl_SpeedPosition_Mode(float pos, float vel);
    bool Ctrl_SpeedMode(float vel);
    bool Servo_Speed(float Speed, float Max_Speed);
    bool Servo_Angle(float Angle, float Max_Speed, float Max_angle, float Min_angle);
};

/**
 * @brief  采用浮点数据等比例转换成整数
 * @param  x_int     	要转换的无符号整数
 * @param  x_min      目标浮点数的最小值
 * @param  x_max    	目标浮点数的最大值
 * @param  bits      	无符号整数的位数
 */
inline float uint_to_float(int x_int, float x_min, float x_max, int bits) {
    /// converts unsigned int to float, given range and number of bits ///
    float span = x_max - x_min;
    float offset = x_min;
    return ((float)x_int) * span / ((float)((1 << bits) - 1)) + offset;
}

/**
 * @brief  将浮点数转换为无符号整数
 * @param  x     			要转换的浮点数
 * @param  x_min      浮点数的最小值
 * @param  x_max    	浮点数的最大值
 * @param  bits      	无符号整数的位数
 */

inline int float_to_uint(float x, float x_min, float x_max, int bits) {
    /// Converts a float to an unsigned int, given range and number of bits///
    float span = x_max - x_min;
    float offset = x_min;
    return (int)((x - offset) * ((float)((1 << bits) - 1)) / span);
}

#endif //DM4310_H
