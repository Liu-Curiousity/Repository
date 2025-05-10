/**
*   @file motor_base.h
*   @brief 电机基类，声明抽象类，用于规定电机接口
*   @author Wenxin HU
*   @date 25-5-4
*   @veersion 1.0
*   @note
*/

#ifndef MOTOR_BASE_H
#define MOTOR_BASE_H

#include "alg_math.h"
#include "stdint.h"

class motor {
public:
    virtual  ~motor() = default;

    volatile bool initialized{false}; // 是否初始化
    volatile bool enabled{false};     // 是否使能
    volatile bool started{false};     // 是否启动

    volatile uint8_t id{0};           // 电机ID
    volatile float speed{0.0f};       // 输出轴转速,单位:rpm
    volatile float angle{0.0f};       // 输出轴角度,单位:rad,范围[0,2pi]
    volatile float temperature{0.0f}; // 温度,单位:℃
    volatile float voltage{0.0f};     // 电压
    volatile float current{0.0f};     // 电流
    volatile float power{0.0f};       // 功率
    volatile float torque{0.0f};      // 功率

    volatile uint8_t rx_buffer[8] = {}; // 接收数据缓存
    volatile uint32_t rx_id{0x000};     //接收的CAN_id

    virtual bool initialize() = 0; // true 成功，false 失败
    virtual bool enable() = 0;     // true 成功，false 失败
    virtual bool disable() = 0;    // true 成功，false 失败
    virtual bool start() = 0;      // true 成功，false 失败
    virtual bool stop() = 0;       // true 成功，false 失败
    virtual bool update() = 0;     // true 成功，false 失败
};

/**
 * @brief 电机集合体基类，用于dji的一拖四模式
 */
class motor_group {
public:
    virtual ~motor_group() = default;

    bool initialized{false};
    bool enabled{false};
    bool started{false};

    virtual bool check_initialize() = 0; //true 所有电机初始化了，false 仍有电机未初始化
    virtual bool check_enable() = 0;     //true 所有电机都使能了，false 仍有电机未使能
    virtual bool check_start() = 0;      //true 所有电机都启动了， false 仍有电机没有启动

    virtual bool update() = 0;                                 // true 成功，false 失败
    virtual bool buffer_update(uint8_t buf[], uint32_t rx_id) = 0; // true 成功，false 失败
    virtual bool ctrl_current() = 0;                           // true 成功，false 失败, 电机对象仅作值传递，这里发报文
};

/**
 * @brief DJI电机用将原始电机编码器速度（ecd）转换为输出轴的速度（rad/s）
 * @param speed 电机反馈的原始速度
 * @param gear_ratio 电机齿轮箱减速比
 * @return 转换后的速度
 */
inline float dji_speed_transform(int16_t speed, uint8_t gear_ratio) {
    return (float)speed / gear_ratio;
}

/**
 * @brief DJI电机用将原始电机编码器角度（ecd）转换为输出轴的角度（rad）
 * @param angle 电机反馈的原始编码器角度
 * @param gear_ratio 电机齿轮箱的减速比
 * @return 转换后的输出轴角度
 */
inline float dji_angle_transform(uint16_t angle, uint8_t gear_ratio) {
    return (float)angle / 8192.0f * 2 * PI / gear_ratio;
}

/**
 * @brief DJI电机用将输出轴的速度（rad/s）转换为原始电机编码器速度（ecd）
 * @param speed 输出轴的速度
 * @param gear_ratio 电机齿轮箱的减速比
 * @return 转换后的原始电机编码器速度
 */
inline int16_t dji_speed_inverter_transform(float speed, uint8_t gear_ratio) {
    return (int16_t)(speed * gear_ratio);
}

/**
 * @brief DJI电机用将输出轴的角度（rad）转换为原始电机编码器角度（ecd）
 * @param angle 输出轴的角度
 * @param gear_ratio 电机齿轮箱的减速比
 * @return 转换后的原始电机编码器角度
 */
inline uint16_t dji_angle_inverter_transform(float angle, uint8_t gear_ratio) {
    return (uint16_t)(angle * 8192.0f / (2 * PI) * gear_ratio);
}

#endif //MOTOR_BASE_H
