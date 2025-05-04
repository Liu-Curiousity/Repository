/**
 * @brief 		motor_base.h库文件
 * @detail
 * @author 	    Haoqi Liu
 * @date        25-5-2
 * @version 	V1.0.0
 * @note 		
 * @warning	    
 * @par 		历史版本
                V1.0.0创建于25-5-2
 * */

#ifndef MOTOR_BASE_H
#define MOTOR_BASE_H

class motor {
public:
    virtual ~motor() = default;

    bool initialized; // 是否初始化
    bool enabled;     // 是否使能
    bool started;     // 是否启动

    uint32_t id;       // 电机ID
    float speed;       // 输出轴转速,单位:rpm
    float angle;       // 输出轴角度,单位:rad,范围[0,2pi]
    float temperature; // 温度,单位:℃
    float voltage;     // 电压
    float current;     // 电流
    float power;       // 功率

    virtual bool initialize() = 0; // true 成功，false 失败
    virtual bool enable() = 0;     // true 成功，false 失败
    virtual bool disable() = 0;    // true 成功，false 失败
    virtual bool start() = 0;      // true 成功，false 失败
    virtual bool stop() = 0;       // true 成功，false 失败
};

class bldc_motor : public motor {
public:
    virtual void set_torque(float torque) = 0;
};

#endif //MOTOR_BASE_H
