#include "task_public.h"
#include "tim.h"
#include "motor_dji_gm6020.h"
#include "motor_servo.h"
#include "memory"

static gm6020 GM6020_1(&hcan1, 1);
static motor_servo GM6020_Servo_1(GM6020_1,
                           PID(PID::position_type,
                               30, 2, 5,
                               NAN,NAN,
                               16383, -16383
                           ),
                           PID(PID::position_type,
                               0, 0, 0,
                               NAN,NAN,
                               16383, -16383
                           ));

void CAN_Filter_Init(void);

void AppTask_Debug(void const *argument) {
    // RetargetInit(&huart1); //串口重定向
    CAN_Filter_Init();
    GM6020_Servo_1.initialize();
    GM6020_Servo_1.enable();
    GM6020_Servo_1.start();
    GM6020_Servo_1.set_speed(30);
    while (true) {
        delay(100);
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim == &htim14) {}
}

void CAN_Filter_Init(void) {
    CAN_FilterTypeDef can1_filter_st, can2_filter_st;

    can1_filter_st.FilterIdHigh = 0x0000;
    can1_filter_st.FilterIdLow = 0x0000;
    can1_filter_st.FilterMaskIdHigh = 0x0000;
    can1_filter_st.FilterMaskIdLow = 0x0000;
    can1_filter_st.FilterFIFOAssignment = CAN_RX_FIFO0;
    can1_filter_st.FilterActivation = ENABLE;
    can1_filter_st.FilterMode = CAN_FILTERMODE_IDMASK;
    can1_filter_st.FilterScale = CAN_FILTERSCALE_32BIT;
    can1_filter_st.FilterBank = 0;
    can1_filter_st.SlaveStartFilterBank = 14;

    can2_filter_st.FilterIdHigh = 0x0000;
    can2_filter_st.FilterIdLow = 0x0000;
    can2_filter_st.FilterMaskIdHigh = 0x0000;
    can2_filter_st.FilterMaskIdLow = 0x0000;
    can2_filter_st.FilterFIFOAssignment = CAN_RX_FIFO1;
    can2_filter_st.FilterActivation = ENABLE;
    can2_filter_st.FilterMode = CAN_FILTERMODE_IDMASK;
    can2_filter_st.FilterScale = CAN_FILTERSCALE_32BIT;
    can2_filter_st.FilterBank = 14;
    can2_filter_st.SlaveStartFilterBank = 14;

    if (HAL_CAN_ConfigFilter(&hcan1, &can1_filter_st) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_CAN_Start(&hcan1) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) {
        Error_Handler();
    }
    HAL_Delay(10);
    if (HAL_CAN_ConfigFilter(&hcan2, &can2_filter_st) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_CAN_Start(&hcan2) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO1_MSG_PENDING) != HAL_OK) // Ê¹ÄÜ CAN2 ½ÓÊÕ FIFO1 ÏûÏ¢ÖÐ¶Ï
    {
        Error_Handler();
    }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    if (hcan == &hcan1) {
        CAN_RxHeaderTypeDef rx_header;
        uint8_t rx_data[8];
        HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_data);
        if (rx_header.StdId > 0x204 && rx_header.StdId < 0x20C) {
            GM6020_1.update(rx_data);
            GM6020_Servo_1.ctrl();
        }
    }
}
