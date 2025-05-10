/**
*   @file arm.h
*   @brief 声明机械臂类
*   @author Wenxin HU
*   @date 25-5-10
*   @veersion 1.0
*   @note
*/

#include "can.h"
#include "dm4310.h"
#include "light.h"
#include "m2006.h"

#ifndef ARM_H
#define ARM_H

extern m2006 m2006_1;
extern m2006_group m2006_group_1;
extern dm4310 dm4310_1;
extern dm4310 dm4340_2;
extern dm4310 dm4310_3;
extern light light1;

typedef struct {
    float q1{0.0f};
    float q2{0.0f};
    float q3{0.0f};
    float q4{0.0f};
} q_t;

class arm {
public:
    arm(CAN_HandleTypeDef *hcan) : hcan(hcan) {};
    ~arm() = default;

    bool initialized{false};
    bool enabled{false};
    bool started{false};

    bool initialize();
    bool enable();
    bool disable();

private:
    //所有关节
    dm4310* joint_1{&dm4310_1};
    dm4310* joint_2{&dm4340_2};
    dm4310* joint_3{&dm4310_3};
    m2006* joint_4{&m2006_1};

    m2006_group* m2006_group{&m2006_group_1};

    light* light{&light1};

    CAN_HandleTypeDef* hcan;
};



#endif //ARM_H
