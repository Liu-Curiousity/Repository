/**
* @file task_debug.cpp
 * @brief Debug任务
 * @author Wenxin Hu
 * @date 25-5-3
 * @version 1.0
 * @note
 */

#include "cmsis_os.h"
#include "task_pubilc.h"
#include "uart_remote.h"
#include "can_base.h"
#include "usb_device.h"
#include "light.h"

extern remote_dr16 dr16_1;
extern can can1;
extern can can2;
extern light light1;

uint16_t temperature = 5500;
uint16_t light = 300;


void AppTask_Debug(void const* argument) {
    MX_USB_DEVICE_Init();

    dr16_1.initialize(&huart3);
    dr16_1.start();

    can1.initialize();
    can2.initialize();
    can1.enable();
    can2.enable();
    can1.start();
    can2.start();

    light1.initialize();
    light1.enable();
    light1.start();


    for (;;) {
        // for (int i = 0; i < 1000; ++i) {
        //     light = 0;
        //     light1.ctrl_light(temperature, light);
        //     osDelay(1);
        // }
        //
        // for (int i = 0; i < 1000; ++i) {
        //     light = 500;
        //     light1.ctrl_light(temperature, light);
        //     osDelay(1);
        // }
    }
}
