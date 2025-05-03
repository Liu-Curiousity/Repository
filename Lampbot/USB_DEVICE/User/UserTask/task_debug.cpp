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

void AppTask_Debug(void const *argument) {
    for (;;) {

        osDelay(1);
    }
}
