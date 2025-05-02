#ifndef RM_TEST_BLDC_MOTOR_H
#define RM_TEST_BLDC_MOTOR_H

#include "main.h"
#include "PID.h"

#define FILTER_LEN 5

typedef struct {
    /***由于不同电机有不同控制方法,故本结构体以及BLDC基础库中不包含电机控制代码,仅提供统一的结构体变量***/
    /*电机属性*/
    uint8_t MotorID;            //电机ID
    /*实时读取参数*/
    int16_t Angle;              //单位:0-8191对应0-360°
    int8_t Temperature;         //电机温度
    int16_t Speed[FILTER_LEN];  //单位:rpm
/*控制变量*/
    PID_TypeDef PID_CtrlModule; //PID对象,速度环
    float TargetCurrent;        //目标电流值,即待发送的电流值
} BLDC_Motor_TypeDef;

typedef struct {
    CAN_HandleTypeDef *hcan;
    BLDC_Motor_TypeDef Motors[4];   //代码针对C620/C610电调
    float TargetSpeed_X;
    float TargetSpeed_Y;
} Chassis_HandlerTypeDef;

typedef struct {
    CAN_HandleTypeDef *hcan;
    BLDC_Motor_TypeDef YawMotor;    //代码针对6020电机
    BLDC_Motor_TypeDef PitchMotor;
} TripodHead_HandleTypeDef;

/**
 * @brief 电机初始化函数
 * @param Motor 电机结构体指针
 * @param MotorID 电机CanID
 * */
extern void Motor_Init(BLDC_Motor_TypeDef *Motor, uint8_t MotorID);

/**
 * @brief 设置电机转速
 * @param Motor 电机结构体指针
 * @param TargetSpeed 电机目标速度
 * @note 注意输入不要超出范围!
 * */
extern void Motor_SetSpeed(BLDC_Motor_TypeDef *Motor, int16_t TargetSpeed);

/**
 * @brief 读取电机转速
 * @param Motor 电机结构体指针
 * @return 电机转速,单位:rpm
 * */
extern int16_t Motor_ReadSpeed(BLDC_Motor_TypeDef *Motor);

/**
 * @brief 读取电机角度
 * @param Motor 电机结构体指针
 * @return 电机角度,0~360°
 * */
extern float Motor_ReadAngle(BLDC_Motor_TypeDef *Motor);

/**
 * @brief 底盘初始化函数
 * @param Chassis 底盘句柄指针
 * @param hcan CAN句柄指针
 * */
extern void Chassis_Init(Chassis_HandlerTypeDef *Chassis, CAN_HandleTypeDef *hcan);

/**
 * @brief 底盘读取电机状态
 * @param Chassis 底盘句柄指针
 * @param RxHeader CAN包头句柄指针
 * @param hcan CAN句柄指针,用于判断CAN数据是否属于底盘
 * @param CAN_RxData CAN接收的数据
 * */
extern void Chassis_ReadMotor(Chassis_HandlerTypeDef *Chassis, CAN_HandleTypeDef *hcan,
                              const CAN_RxHeaderTypeDef *RxHeader, const uint8_t *CAN_RxData);

/**
 * @brief 底盘控制函数
 * @param Chassis 底盘句柄指针
 * */
extern void Chassis_Ctrl_IRQ(Chassis_HandlerTypeDef *Chassis);

/**
 * @brief 底盘速度控制
 * @param Chassis 底盘句柄指针
 * @param Speed_X 底盘X轴方向移动速度
 * @param Speed_Y 底盘Y轴方向移动速度
 * @param Speed_Yaw 底盘Yaw轴方向转速,顺时针为正
 * @warning 底盘姿态没有闭环控制，Speed_X和Speed_Y千万要设得保守
 * */
extern void Chassis_MoveSet(Chassis_HandlerTypeDef *Chassis, float Speed_X, float Speed_Y, float Speed_Yaw);

/**
 * @brief 云台初始化函数
 * @param TripodHead 云台句柄指针
 * @param hcan CAN句柄指针
 * */
extern void TripodHead_Init(TripodHead_HandleTypeDef *TripodHead, CAN_HandleTypeDef *hcan);

/**
 * @brief 云台读取电机状态
 * @param TripodHead 云台句柄指针
 * @param RxHeader CAN包头句柄指针
 * @param hcan CAN句柄指针,用于判断CAN数据是否属于云台
 * @param CAN_RxData CAN接收的数据
 * */
extern void TripodHead_ReadMotor(TripodHead_HandleTypeDef *TripodHead, CAN_HandleTypeDef *hcan,
                                 const CAN_RxHeaderTypeDef *RxHeader, const uint8_t *CAN_RxData);

/**
 * @brief 云台控制函数
 * @param TripodHead 云台句柄指针
 * */
extern void TripodHead_Ctrl_IRQ(TripodHead_HandleTypeDef *TripodHead);

#endif //RM_TEST_BLDC_MOTOR_H
