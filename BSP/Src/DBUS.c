#include "DBUS.h"

void DBUS_Decode(DBUS_HandleTypeDef *DBUS) {
    DBUS->Data.Channel0 = ((int16_t) DBUS->OriginData[0] | ((int16_t) DBUS->OriginData[1] << 8)) & 0x07FF;
    DBUS->Data.Channel1 = (((int16_t) DBUS->OriginData[1] >> 3) | ((int16_t) DBUS->OriginData[2] << 5)) & 0x07FF;
    DBUS->Data.Channel2 = (((int16_t) DBUS->OriginData[2] >> 6) | ((int16_t) DBUS->OriginData[3] << 2) |
                           ((int16_t) DBUS->OriginData[4] << 10)) & 0x07FF;
    DBUS->Data.Channel3 = (((int16_t) DBUS->OriginData[4] >> 1) | ((int16_t) DBUS->OriginData[5] << 7)) & 0x07FF;
    DBUS->Data.S1 = ((DBUS->OriginData[5] >> 4) & 0x000C) >> 2;
    DBUS->Data.S2 = ((DBUS->OriginData[5] >> 4) & 0x0003);
    DBUS->Data.MouseX = (int16_t) (((int16_t) DBUS->OriginData[6]) | ((int16_t) DBUS->OriginData[7] << 8));
    DBUS->Data.MouseY = (int16_t) (((int16_t) DBUS->OriginData[8]) | ((int16_t) DBUS->OriginData[9] << 8));
    DBUS->Data.MouseZ = (int16_t) ((((int16_t) DBUS->OriginData[10]) | ((int16_t) DBUS->OriginData[11] << 8)));
    DBUS->Data.MouseKeyL = DBUS->OriginData[12];
    DBUS->Data.MouseKeyR = DBUS->OriginData[13];
    uint16_t Keys = ((int16_t) DBUS->OriginData[14]) | ((int16_t) DBUS->OriginData[15] << 8);
    DBUS->Data.W = Keys & 0x01 << 0;
    DBUS->Data.S = Keys & 0x01 << 1;
    DBUS->Data.A = Keys & 0x01 << 2;
    DBUS->Data.D = Keys & 0x01 << 3;
    DBUS->Data.Q = Keys & 0x01 << 4;
    DBUS->Data.E = Keys & 0x01 << 5;
    DBUS->Data.Shift = Keys & 0x01 << 6;
    DBUS->Data.Ctrl = Keys & 0x01 << 7;

}

void DBUS_Start(DBUS_HandleTypeDef *DBUS) {
    HAL_UART_Receive_DMA(DBUS->huart, DBUS->OriginData, 18);
}

void DBUS_Stop(DBUS_HandleTypeDef *DBUS) {
    HAL_UART_AbortReceive(DBUS->huart);
}