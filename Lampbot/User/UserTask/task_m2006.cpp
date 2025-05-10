/**
*   @file 
*   @brief M2006电机控制任务
*   @author Wenxin HU
*   @date 25-5-9
*   @veersion 1.0
*   @note
*/

#include "cmsis_os.h"
#include "task_pubilc.h"
#include "m2006.h"

float P = 0.0f;

extern m2006 m2006_1;
extern m2006_group m2006_group_1;

void AppTask_M2006(void const *argument) {
    PID_Params_t speed_params = {
        8.0f, 0.4f, 0.0f, 0.0f,
        3000.0f, 10000.0f
    };
    PID_Params_t angle_params = {
        350.0f, 10.0f, 0.0f, 5000.0f,
        100.0f, 450.0f
    };//
    m2006_1.initialize(&speed_params,&angle_params);
    m2006_1.enable();
    m2006_1.start();

    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 1  / portTICK_PERIOD_MS; // 周期为1ms
    xLastWakeTime = xTaskGetTickCount();
    int cnt = 0;
    float A = 1;

    for (;;) {

        // 生成正弦波信号

         // A = (A >= 2.5f) ? 0 : A + 0.0005f;
         // cnt = (cnt >= 800) ? 0 : cnt + 1;
         // P = 3 + A * sin(2 * PI * (1.0f / 800) * cnt);
        m2006_1.Ctrl_current(0.0f);
        // m2006_1.Servo_speed(P,1000);
        // m2006_1.Servo_angle(P,450.0f,2.0f*PI,0.0f);

        m2006_group_1.update();
        m2006_group_1.ctrl_current();

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
