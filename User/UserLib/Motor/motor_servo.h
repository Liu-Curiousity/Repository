/**
 * @brief 		motor_servo.h库文件
 * @detail
 * @author 	    Haoqi Liu
 * @date        25-5-2
 * @version 	V1.0.0
 * @note 		
 * @warning	    
 * @par 		历史版本
                V1.0.0创建于25-5-2
 * */

#ifndef MOTOR_SERVO_H
#define MOTOR_SERVO_H

#include "motor_base.h"
#include "PID.h"

using namespace std;

class motor_servo {
public:
    enum CtrlMode {
        CtrlMode_Torque,
        CtrlMode_Speed,
        CtrlMode_Angle,
    };

    motor_servo() = delete;

    motor_servo(bldc_motor& motor, const PID& speed_pid, const PID& angle_pid):
        motor(motor), speed_pid(speed_pid), angle_pid(angle_pid) {}

    bool initialized{false};
    bool enabled{false};
    bool started{false};

    float target_torque{0};
    CtrlMode ctrl_mode{CtrlMode_Torque};
    bldc_motor& motor;

    bool initialize();
    bool enable();
    bool disable();
    bool start();
    bool stop();

    void set_torque(float torque);
    void set_speed(float speed);
    void set_angle(float angle);

    /**
     * @brief
     * @note 需要定时调用
     */
    void ctrl();

private
:
    PID speed_pid;
    PID angle_pid;
};

#endif //MOTOR_SERVO_H
