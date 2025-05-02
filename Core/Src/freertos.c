/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart.h"
#include "can.h"
#include "tim.h"
#include "retarget.h"
#include "BLDC_Motor.h"
#include "DBUS.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
int16_t Angle;
int16_t Speed;


Chassis_HandlerTypeDef Chassis;
TripodHead_HandleTypeDef TripodHead;
DBUS_HandleTypeDef DBUS1 = {&huart3};
/* USER CODE END Variables */
osThreadId Task_DebugHandle;
osThreadId Task_PID_CtrlHandle;
osThreadId Task_InteractivHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void C610_Test(CAN_HandleTypeDef *hcan, int16_t Current);

void CAN_Filter_INIT(CAN_HandleTypeDef *hcan);

/* USER CODE END FunctionPrototypes */

void AppTask_Debug(void const *argument);

void AppTask_PID_Ctrl(void const *argument);

void AppTask_Interactive(void const *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
    /* USER CODE END RTOS_MUTEX */

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    /* USER CODE END RTOS_TIMERS */

    /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
    /* USER CODE END RTOS_QUEUES */

    /* Create the thread(s) */
    /* definition and creation of Task_Debug */
    osThreadDef(Task_Debug, AppTask_Debug, osPriorityLow, 0, 1024);
    Task_DebugHandle = osThreadCreate(osThread(Task_Debug), NULL);

    /* definition and creation of Task_PID_Ctrl */
    osThreadDef(Task_PID_Ctrl, AppTask_PID_Ctrl, osPriorityRealtime, 0, 1024);
    Task_PID_CtrlHandle = osThreadCreate(osThread(Task_PID_Ctrl), NULL);

    /* definition and creation of Task_Interactiv */
    osThreadDef(Task_Interactiv, AppTask_Interactive, osPriorityNormal, 0, 1024);
    Task_InteractivHandle = osThreadCreate(osThread(Task_Interactiv), NULL);

    /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
    /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_AppTask_Debug */
/**
  * @brief  Function implementing the Task_Debug thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_AppTask_Debug */
void AppTask_Debug(void const *argument) {
    /* USER CODE BEGIN AppTask_Debug */

//    float TxBuffer[5];                              //定义发送缓冲区
//    *(uint32_t *) (TxBuffer + 4) = 0x7f800000;      //以特殊数据结尾,用于协议识别

    RetargetInit(&huart1);                  //串口重定向
    /* Infinite loop */
    for (;;) {
        /*VOFA+JustFloat模式发送四个电机的转速*/
//        TxBuffer[0] = (float) Motor_ReadSpeed(&Chassis.Motors[0]);
//        TxBuffer[1] = (float) Motor_ReadSpeed(&Chassis.Motors[1]);
//        TxBuffer[2] = (float) Motor_ReadSpeed(&Chassis.Motors[2]);
//        TxBuffer[3] = (float) Motor_ReadSpeed(&Chassis.Motors[3]);
//        HAL_UART_Transmit(&huart1, TxBuffer, sizeof(float) * 5, HAL_MAX_DELAY);

//        printf("Channel0:%d\n", DBUS1.Data.Channel0);
//        printf("Channel1:%d\n", DBUS1.Data.Channel1);
//        printf("Channel2:%d\n", DBUS1.Data.Channel2);
//        printf("Channel3:%d\n", DBUS1.Data.Channel3);
//        printf("S1:%d\n", DBUS1.Data.S1);
//        printf("S2:%d\n", DBUS1.Data.S2);
//        putchar('\n');
//        printf("YawAngle:%.2f\n", Motor_ReadAngle(&TripodHead.YawMotor));
//        printf("YawSpeed:%d\n", Motor_ReadSpeed(&TripodHead.YawMotor));
        printf("%d,%d\n", Speed, Angle);
        putchar('\n');
        putchar('\n');
        putchar('\n');


        osDelay(50);
    }
    /* USER CODE END AppTask_Debug */
}

/* USER CODE BEGIN Header_AppTask_PID_Ctrl */
/**
* @brief Function implementing the Task_PID_Ctrl thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_AppTask_PID_Ctrl */
void AppTask_PID_Ctrl(void const *argument) {
    /* USER CODE BEGIN AppTask_PID_Ctrl */
    CAN_Filter_INIT(&hcan2);                        //初始化CAN外设滤波器
    CAN_Filter_INIT(&hcan1);                        //初始化CAN外设滤波器
    Chassis_Init(&Chassis, &hcan2);                 //初始化底盘
    TripodHead_Init(&TripodHead, &hcan2);
    HAL_TIM_Base_Start_IT(&htim14);                 //开启中断,即开启PID控制
    /* Infinite loop */
    for (;;) {
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
        Chassis_Ctrl_IRQ(&Chassis);
        TripodHead_Ctrl_IRQ(&TripodHead);
        C610_Test(&hcan1, -300);
    }
    /* USER CODE END AppTask_PID_Ctrl */
}

/* USER CODE BEGIN Header_AppTask_Interactive */
/**
* @brief Function implementing the Task_Interactiv thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_AppTask_Interactive */
void AppTask_Interactive(void const *argument) {
    /* USER CODE BEGIN AppTask_Interactive */

//    float RxBuffer[2];
//    HAL_UART_Receive_IT(&huart1, RxBuffer, sizeof(float) * 2);

    DBUS_Start(&DBUS1);

    /* Infinite loop */
    for (;;) {
//        if (HAL_UART_GetState(&huart1) != HAL_UART_STATE_BUSY_RX
//            && HAL_UART_GetState(&huart1) != HAL_UART_STATE_BUSY_TX_RX) {
//            HAL_UART_Receive_IT(&huart1, RxBuffer, sizeof(float) * 2);
//            /**第一个float是yaw速度,第二个float是y轴速度**/
//            Chassis_MoveSet(&Chassis, 0, 1 * RxBuffer[1], 1 * RxBuffer[0]);
//        }
        if (DBUS1.Data.S1 == 2 && DBUS1.Data.S2 == 2) {
            Chassis_MoveSet(&Chassis, (float) DBUS1.Data.Channel2 - 1024,
                            (float) DBUS1.Data.Channel3 - 1024,
                            (float) DBUS1.Data.Channel0 - 1024);
            Motor_SetSpeed(&TripodHead.YawMotor, -50);
        } else {
            Chassis_MoveSet(&Chassis, 0, 0, 0);
            Motor_SetSpeed(&TripodHead.YawMotor, 0);
        }
        vTaskDelay(10);
    }
    /* USER CODE END AppTask_Interactive */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/**
 * @brief CAN外设初始化函数
 * */
void CAN_Filter_INIT(CAN_HandleTypeDef *hcan) {
    CAN_FilterTypeDef Filter;
    Filter.FilterMode = CAN_FILTERMODE_IDMASK;      //掩码模式过滤器
    Filter.FilterActivation = CAN_FILTER_ENABLE;    //使能过滤器
    Filter.FilterScale = CAN_FILTERSCALE_32BIT;     //32位过滤器
    Filter.FilterBank = 0;                          //设置过滤器编号
    Filter.SlaveStartFilterBank = 0;                //设置从机过滤器编号
    Filter.FilterFIFOAssignment = CAN_FILTER_FIFO0; //使用FIFO0
    /*一点都不过滤*/
    Filter.FilterIdHigh = 0x0000;
    Filter.FilterIdLow = 0x0000;
    Filter.FilterMaskIdHigh = 0x0000;
    Filter.FilterMaskIdLow = 0x0000;

    HAL_CAN_ConfigFilter(hcan, &Filter);
    HAL_CAN_Start(hcan);
    HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
}

/**
 * @brief CAN接收回调函数
 * */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    CAN_RxHeaderTypeDef RxHeader;
    uint8_t CAN_RxData[8];
    /*如果FIFO中有数据*/
    if (HAL_CAN_GetRxFifoFillLevel(hcan, CAN_RX_FIFO0))
        /*读取数据*/
        HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, CAN_RxData);
    Chassis_ReadMotor(&Chassis, hcan, &RxHeader, CAN_RxData);
    TripodHead_ReadMotor(&TripodHead, hcan, &RxHeader, CAN_RxData);
    if (hcan == &hcan1) {
        Angle = (int16_t) (CAN_RxData[0] << 8 | CAN_RxData[1]);
        Speed = (int16_t) (CAN_RxData[2] << 8 | CAN_RxData[3]);
    }
}

/**
 * @brief 定时器,通知PID任务进行PID运算
 * */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    BaseType_t TaskToWoken = pdFALSE;
    if (htim == &htim14) {
        vTaskNotifyGiveFromISR(Task_PID_CtrlHandle, &TaskToWoken);
        portYIELD_FROM_ISR(TaskToWoken);
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart == &huart3) {
        DBUS_Decode(&DBUS1);
    }
}

void C610_Test(CAN_HandleTypeDef *hcan, int16_t Current) {
    /*声明CAN发送数据包缓存数组*/
    static uint8_t CAN_TxData[8];
    /*定义CAN数据包头*/
    static CAN_TxHeaderTypeDef TxHeader = {
            0x0200, 0x0000, CAN_ID_STD, CAN_RTR_DATA, 8, 0
    };
    /*写入CAN发送数据包数组*/
    CAN_TxData[6] = Current >> 8;
    CAN_TxData[7] = Current & 0xff;
    /*若hcan发送邮箱不为空,则添加数据到发送邮箱*/
    if (HAL_CAN_GetTxMailboxesFreeLevel(hcan))
        HAL_CAN_AddTxMessage(hcan, &TxHeader, CAN_TxData, (uint32_t *) CAN_TX_MAILBOX0);
}
/* USER CODE END Application */
