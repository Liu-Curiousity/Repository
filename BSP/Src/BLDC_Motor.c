#include "BLDC_Motor.h"

void Motor_Init(BLDC_Motor_TypeDef *Motor, uint8_t MotorID) {
    Motor->MotorID = MotorID;
}

void Motor_SetSpeed(BLDC_Motor_TypeDef *Motor, int16_t TargetSpeed) {
    PID_SetTarget(&Motor->PID_CtrlModule, TargetSpeed);
}

int16_t Motor_ReadSpeed(BLDC_Motor_TypeDef *Motor) {
    int32_t Sum = 0;
    for (uint8_t i = 0; i < FILTER_LEN; ++i) {
        Sum += Motor->Speed[i];
    }
    return (int16_t) (Sum / FILTER_LEN);
}

float Motor_ReadAngle(BLDC_Motor_TypeDef *Motor) {
    return ((float) Motor->Angle) / 8191 * 360;
}

void Chassis_Init(Chassis_HandlerTypeDef *Chassis, CAN_HandleTypeDef *hcan) {
    Chassis->hcan = hcan;
    for (uint8_t i = 0; i < 4; ++i) {
        Motor_Init(&Chassis->Motors[i], i + 1);
        PID_Init(&Chassis->Motors[i].PID_CtrlModule, PID_TYPE_DELTA, 0, 6, 0.9f, 0);
        PID_SetLimit(&Chassis->Motors[i].PID_CtrlModule, 0, 32750);
    }
}

void Chassis_ReadMotor(Chassis_HandlerTypeDef *Chassis, CAN_HandleTypeDef *hcan,
                       const CAN_RxHeaderTypeDef *RxHeader, const uint8_t *CAN_RxData) {
    if (hcan != Chassis->hcan) return;
    for (uint8_t i = 0; i < 4; ++i) {
        /*找到对应电机*/
        if (RxHeader->StdId == (Chassis->Motors[i].MotorID + 0x200)) {
            /*移动滤波队列*/
            for (uint8_t j = FILTER_LEN - 1; j > 0; --j) {
                Chassis->Motors[i].Speed[j] = Chassis->Motors[i].Speed[j - 1];
            }
            Chassis->Motors[i].Angle = (int16_t) (CAN_RxData[0] << 8 | CAN_RxData[1]);
            Chassis->Motors[i].Speed[0] = (int16_t) (CAN_RxData[2] << 8 | CAN_RxData[3]);
            Chassis->Motors[i].Temperature = (int8_t) CAN_RxData[6];
        }
    }
}

void Chassis_Ctrl_IRQ(Chassis_HandlerTypeDef *Chassis) {
    /*声明CAN发送数据包缓存数组*/
    static uint8_t CAN_TxData[8];
    /*定义CAN数据包头*/
    static CAN_TxHeaderTypeDef TxHeader = {
            0x0200, 0x0000, CAN_ID_STD, CAN_RTR_DATA, 8, 0
    };
    /*计算PID输出值并写入到CAN发送数据包数组*/
    for (uint8_t i = 0; i < 4; ++i) {
        Chassis->Motors[i].TargetCurrent = PID_ISR(&Chassis->Motors[i].PID_CtrlModule,
                                                   Motor_ReadSpeed(&Chassis->Motors[i]));
        CAN_TxData[2 * i] = (int16_t) (Chassis->Motors[i].TargetCurrent) >> 8;
        CAN_TxData[2 * i + 1] = (int16_t) (Chassis->Motors[i].TargetCurrent) & 0xff;
    }
    /*若hcan发送邮箱不为空,则添加数据到发送邮箱*/
    if (HAL_CAN_GetTxMailboxesFreeLevel(Chassis->hcan))
        HAL_CAN_AddTxMessage(Chassis->hcan, &TxHeader, CAN_TxData, (uint32_t *) CAN_TX_MAILBOX0);
}

void Chassis_MoveSet(Chassis_HandlerTypeDef *Chassis, float Speed_X, float Speed_Y, float Speed_Yaw) {
    float Speed[4] = {
            Speed_X + Speed_Y + Speed_Yaw,
            Speed_X - Speed_Y + Speed_Yaw,
            -Speed_X + Speed_Y + Speed_Yaw,
            -Speed_X - Speed_Y + Speed_Yaw
    };
    for (uint8_t i = 0; i < 4; ++i) {
        Motor_SetSpeed(&Chassis->Motors[i], (int16_t) Speed[i]);
    }
}

void TripodHead_Init(TripodHead_HandleTypeDef *TripodHead, CAN_HandleTypeDef *hcan) {
    TripodHead->hcan = hcan;
    Motor_Init(&TripodHead->YawMotor, 5);
    PID_Init(&TripodHead->YawMotor.PID_CtrlModule, PID_TYPE_POSITION, 0, 200, 15, 300);
    PID_SetLimit(&TripodHead->YawMotor.PID_CtrlModule, 1000, 25000);
}

void TripodHead_ReadMotor(TripodHead_HandleTypeDef *TripodHead, CAN_HandleTypeDef *hcan,
                          const CAN_RxHeaderTypeDef *RxHeader, const uint8_t *CAN_RxData) {
    if (hcan != TripodHead->hcan) return;
    /*找到对应电机*/
    if (RxHeader->StdId == (TripodHead->YawMotor.MotorID + 0x204)) {
        /*移动滤波队列*/
        for (uint8_t j = FILTER_LEN - 1; j > 0; --j) {
            TripodHead->YawMotor.Speed[j] = TripodHead->YawMotor.Speed[j - 1];
        }
        TripodHead->YawMotor.Angle = (int16_t) (CAN_RxData[0] << 8 | CAN_RxData[1]);
        TripodHead->YawMotor.Speed[0] = (int16_t) (CAN_RxData[2] << 8 | CAN_RxData[3]);
        TripodHead->YawMotor.Temperature = (int8_t) CAN_RxData[6];
    }
}

void TripodHead_Ctrl_IRQ(TripodHead_HandleTypeDef *TripodHead) {
    /*声明CAN发送数据包缓存数组*/
    static uint8_t CAN_TxData[8];
    /*定义CAN数据包头,设置电压值*/
    uint32_t Stdid;
    if (TripodHead->YawMotor.MotorID > 4 && TripodHead->YawMotor.MotorID < 8) {
        Stdid = 0x2FF;
    } else if (TripodHead->YawMotor.MotorID > 0 && TripodHead->YawMotor.MotorID <= 4) {
        Stdid = 0x1FF;
    } else {
        return;
    }
    CAN_TxHeaderTypeDef TxHeader = {
            Stdid, 0x0000, CAN_ID_STD, CAN_RTR_DATA, 8, 0
    };
    /*计算PID输出值并写入到CAN发送数据包数组*/
    TripodHead->YawMotor.TargetCurrent = PID_ISR(&TripodHead->YawMotor.PID_CtrlModule,
                                                 Motor_ReadSpeed(&TripodHead->YawMotor));
    CAN_TxData[2 * ((TripodHead->YawMotor.MotorID - 1) % 4)] =
            (int16_t) (TripodHead->YawMotor.TargetCurrent) >> 8;
    CAN_TxData[2 * ((TripodHead->YawMotor.MotorID - 1) % 4) + 1] =
            (int16_t) (TripodHead->YawMotor.TargetCurrent) & 0xff;
    /*若hcan发送邮箱不为空,则添加数据到发送邮箱*/
    if (HAL_CAN_GetTxMailboxesFreeLevel(TripodHead->hcan))
        HAL_CAN_AddTxMessage(TripodHead->hcan, &TxHeader, CAN_TxData, (uint32_t *) CAN_TX_MAILBOX0);
}