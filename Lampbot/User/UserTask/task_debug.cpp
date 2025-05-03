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

extern remote_dr16 dr16_1;

void AppTask_Debug(void const *argument) {
    dr16_1.initialize(&huart3);
    dr16_1.start();

    for (;;) {

        osDelay(1);
    }
}
