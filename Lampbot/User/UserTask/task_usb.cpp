/**
* @file task_usb.cpp
 * @brief USBVCP任务
 * @author Wenxin Hu
 * @date 25-5-3
 * @version 1.0
 * @note
 */

#include "cmsis_os.h"
#include "task_pubilc.h"
#include "usb.h"
#include <cmath>
#include "usb_device.h"

extern usb usb_1;

void AppTask_USB(void const* argument) {
    MX_USB_DEVICE_Init();

    uint16_t c = 0;
    usb_1.initialize();
    usb_1.enable();
    usb_1.start();

    while (1) {
        osDelay(2000);
        c = (c > 1000) ? 0 : c + 1;
        usb_1.rx_Msg();
        usb_1.tx_Msg(
            3.145f / 2.0f * sin(c * 0.01f),
            5.314f / 2.0f * sin(c * 0.01f));
    }
}
