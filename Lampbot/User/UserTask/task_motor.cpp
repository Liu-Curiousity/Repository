/**
* @file task_motor.cpp
 * @brief 电机控制任务
 * @author Wenxin Hu
 * @date 25-5-3
 * @version 1.0
 * @note
 */

#include "cmsis_os.h"
#include "task_pubilc.h"
#include "uart_remote.h"
#include "dm4310.h"
#include "m2006.h"

// #define DEV_GM6020
// #define DEV_DM4310
// #define DEV_DM4340
#define DEV_ARM

extern remote_dr16 dr16_1;
extern dm4310 dm4310_1;
extern dm4310 dm4310_4;
extern dm4310 dm4340_2;


void AppTask_Motor(void const* argument) {

#ifdef DEV_DM4310
    PID_Params_t speed_params = {
        0.25f, 0.55f, 10.0f, 0.6f,
        3.0f, 18.0f
    };
    PID_Params_t angle_params = {
        150.0f, 20.0f, 0.0f, 35.0f,
        60.0f, 60.0f
    };

    dm4310_4.initialize(&speed_params,&angle_params);
    dm4310_4.enable();
    dm4310_4.start();
#elifdef DEV_DM4340
    dm4340_2.initialize(&speed_params,&angle_params);
    dm4340_2.enable();
    dm4340_2.start();
#elifdef DEV_ARM

#endif

    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 1  / portTICK_PERIOD_MS; // 周期为1ms
    xLastWakeTime = xTaskGetTickCount();
    int cnt = 0;

    for (;;) {

#ifdef DEV_GM6020
        //生成正弦波信号
        // cnt = (cnt == 500) ? 0 : cnt + 1;
        // P = 3 + 2.5 * sin(2 * PI * (1.0f / 500) * cnt);
        // GM6020_4.Ctrl_Angle(P);
        GM6020_4.Ctrl_Current(0.0f);
        GM6020_All1.Ctrl_Current();

#elifdef DEV_DM4310
        dm4310_4.update();
        cnt = (cnt == 4000) ? 0 : cnt + 1;
        P = 3+ 3 * sin(2 * PI * (1.0f / 4000) * cnt);
        dm4310_4.Servo_Angle(P,60.0f,2*PI,0.0f);
#elifdef DEV_DM4340
        dm4340_2.update();
        cnt = (cnt == 8000) ? 0 : cnt + 1;
        P = 3+ 3 * sin(2 * PI * (1.0f / 8000) * cnt);
        dm4340_2.Ctrl_SpeedPosition_Mode(P,5.0f);
#elifdef DEV_ARM

#endif
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
