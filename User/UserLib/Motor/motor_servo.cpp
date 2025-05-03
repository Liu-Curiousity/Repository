#include "motor_servo.h"
#include "numbers"

bool motor_servo::initialize() {
    if (!motor.initialize()) return false;
    initialized = true;
    return true;
}

bool motor_servo::enable() {
    if (!initialized) return false;
    if (!motor.enable()) return false;
    enabled = true;
    return true;
}

bool motor_servo::disable() {
    if (!initialized) return false;
    if (!enabled) return false;
    if (!motor.disable()) return false;
    enabled = false;
    return true;
}

bool motor_servo::start() {
    if (!initialized) return false;
    if (!enabled) return false;
    started = true;
    return true;
}

bool motor_servo::stop() {
    if (!initialized) return false;
    if (!enabled) return false;
    started = false;
    return true;
}

void motor_servo::set_torque(const float torque) {
    target_torque = torque;
    ctrl_mode = CtrlMode_Torque;
}

void motor_servo::set_speed(const float speed) {
    speed_pid.target = speed;
    ctrl_mode = CtrlMode_Speed;
}

void motor_servo::set_angle(const float angle) {
    angle_pid.target = angle;
    ctrl_mode = CtrlMode_Angle;
}

void motor_servo::ctrl() {
    if (!initialized || !enabled || !started) return;
    switch (ctrl_mode) {
        case CtrlMode_Angle:
            if (motor.angle - angle_pid.target > numbers::pi_v<float>)
                speed_pid.target = angle_pid.clac(motor.angle - 2 * numbers::pi_v<float>);
            else if (motor.angle - angle_pid.target < -numbers::pi_v<float>)
                speed_pid.target = angle_pid.clac(motor.angle + 2 * numbers::pi_v<float>);
            else
                speed_pid.target = angle_pid.clac(motor.angle);
        case CtrlMode_Speed:
            motor.set_torque(speed_pid.clac(motor.speed));
            break;
        case CtrlMode_Torque:
            motor.set_torque(target_torque);
            break;
        default:
            break;
    }
}
