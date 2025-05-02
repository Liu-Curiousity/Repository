#ifndef RM_TEST_DBUS_H
#define RM_TEST_DBUS_H

#include "main.h"


typedef struct {
    struct {
        uint16_t Channel0;
        uint16_t Channel1;
        uint16_t Channel2;
        uint16_t Channel3;
        struct {
            uint8_t S1: 2;
            uint8_t S2: 2;
        };
//        uint8_t S1;
//        uint8_t S2;
        int16_t MouseX;
        int16_t MouseY;
        int16_t MouseZ;
        struct {
            uint8_t MouseKeyL: 1;
            uint8_t MouseKeyR: 1;
        };
        struct {
            uint8_t W: 1;
            uint8_t S: 1;
            uint8_t A: 1;
            uint8_t D: 1;
            uint8_t Q: 1;
            uint8_t E: 1;
            uint8_t Shift: 1;
            uint8_t Ctrl: 1;
        };
    };
} DBUS_DataTypeDef;

typedef struct {
    UART_HandleTypeDef *huart;
    DBUS_DataTypeDef Data;
    uint8_t OriginData[18];
} DBUS_HandleTypeDef;

extern void DBUS_Decode(DBUS_HandleTypeDef *DBUS);

extern void DBUS_Start(DBUS_HandleTypeDef *DBUS);

extern void DBUS_Stop(DBUS_HandleTypeDef *DBUS);

#endif //RM_TEST_DBUS_H
