/**
* @file uart_remote.h
 * @brief DR16遥控器类
 * @author Wenxin Hu
 * @date 25-5-3
 * @version 1.0
 * @note UART的子类，用于遥控器的数据传输
 */

#ifndef UART_REMOTE_H
#define UART_REMOTE_H

#include "stdint.h"
#include "uart_base.h"

//遥控器按键宏定义
#define    KEY_PRESSED_OFFSET_W        ((uint16_t)0x01<<0)
#define    KEY_PRESSED_OFFSET_S        ((uint16_t)0x01<<1)
#define    KEY_PRESSED_OFFSET_A        ((uint16_t)0x01<<2)
#define    KEY_PRESSED_OFFSET_D        ((uint16_t)0x01<<3)
#define    KEY_PRESSED_OFFSET_SHIFT    ((uint16_t)0x01<<4)
#define    KEY_PRESSED_OFFSET_CTRL     ((uint16_t)0x01<<5)
#define    KEY_PRESSED_OFFSET_Q        ((uint16_t)0x01<<6)
#define    KEY_PRESSED_OFFSET_E        ((uint16_t)0x01<<7)
#define    KEY_PRESSED_OFFSET_R        ((uint16_t)0x01<<8)
#define    KEY_PRESSED_OFFSET_F        ((uint16_t)0x01<<9)
#define    KEY_PRESSED_OFFSET_G        ((uint16_t)0x01<<10)
#define    KEY_PRESSED_OFFSET_Z        ((uint16_t)0x01<<11)
#define    KEY_PRESSED_OFFSET_X        ((uint16_t)0x01<<12)
#define    KEY_PRESSED_OFFSET_C        ((uint16_t)0x01<<13)
#define    KEY_PRESSED_OFFSET_V        ((uint16_t)0x01<<14)
#define    KEY_PRESSED_OFFSET_B        ((uint16_t)0x01<<15)
/* 检测键盘按键状态
   若对应按键被按下，则逻辑表达式的值为1，否则为0 */
#define    IF_KEY_PRESSED         (  DBUS.key.v  )
#define    IF_KEY_PRESSED_W       ( (DBUS.key.v & KEY_PRESSED_OFFSET_W)    != 0 )
#define    IF_KEY_PRESSED_S       ( (DBUS.key.v & KEY_PRESSED_OFFSET_S)    != 0 )
#define    IF_KEY_PRESSED_A       ( (DBUS.key.v & KEY_PRESSED_OFFSET_A)    != 0 )
#define    IF_KEY_PRESSED_D       ( (DBUS.key.v & KEY_PRESSED_OFFSET_D)    != 0 )
#define    IF_KEY_PRESSED_Q       ( (DBUS.key.v & KEY_PRESSED_OFFSET_Q)    != 0 )
#define    IF_KEY_PRESSED_E       ( (DBUS.key.v & KEY_PRESSED_OFFSET_E)    != 0 )
#define    IF_KEY_PRESSED_G       ( (DBUS.key.v & KEY_PRESSED_OFFSET_G)    != 0 )
#define    IF_KEY_PRESSED_X       ( (DBUS.key.v & KEY_PRESSED_OFFSET_X)    != 0 )
#define    IF_KEY_PRESSED_Z       ( (DBUS.key.v & KEY_PRESSED_OFFSET_Z)    != 0 )
#define    IF_KEY_PRESSED_C       ( (DBUS.key.v & KEY_PRESSED_OFFSET_C)    != 0 )
#define    IF_KEY_PRESSED_B       ( (DBUS.key.v & KEY_PRESSED_OFFSET_B)    != 0 )
#define    IF_KEY_PRESSED_V       ( (DBUS.key.v & KEY_PRESSED_OFFSET_V)    != 0 )
#define    IF_KEY_PRESSED_F       ( (DBUS.key.v & KEY_PRESSED_OFFSET_F)    != 0 )
#define    IF_KEY_PRESSED_R       ( (DBUS.key.v & KEY_PRESSED_OFFSET_R)    != 0 )
#define    IF_KEY_PRESSED_CTRL    ( (DBUS.key.v & KEY_PRESSED_OFFSET_CTRL) != 0 )
#define    IF_KEY_PRESSED_SHIFT   ( (DBUS.key.v & KEY_PRESSED_OFFSET_SHIFT) != 0 )

/**
 * @brief 遥控器数据结构体
 */
typedef struct {
    //摇杆
    int16_t ch0{0};
    int16_t ch1{0};
    int16_t ch2{0};
    int16_t ch3{0};

    //三位开关
    int8_t s1{0};
    int8_t s2{0};

    //鼠标
    int16_t x{0};
    int16_t y{0};
    int16_t z{0};
    uint8_t press_l{0};
    uint8_t press_r{0};

    //按键（Bit0 -- W 键Bit1 -- S 键Bit2 -- A 键Bit3 -- D 键Bit4 -- Q 键Bit5 -- E 键Bit6 -- Shift 键Bit7 -- Ctrl 键）
    uint16_t key{0};

    //wheel
    int16_t wheel{0};
} remote_t;

class remote_dr16 final : public uart {
public:
    remote_dr16() = default;
    ~remote_dr16() = default;

    bool initialize(UART_HandleTypeDef* handle) override;
    bool start() override;
    bool stop() override;

    bool rx_data() override;
    bool tx_data() override;
    bool encode_data() override;
    bool decode_data() override;
    //获取内部数据指针方法
    uint8_t* get_rx_buffer();
    uint8_t get_rx_buffer_size();
    remote_t* get_info();
    //直接获取指定值的方法
    int16_t get_ch0();
    int16_t get_ch1();
    int16_t get_ch2();
    int16_t get_ch3();
    int8_t get_s1();
    int8_t get_s2();
    int16_t get_x();
    int16_t get_y();
    int16_t get_z();
    uint8_t get_press_l();
    uint8_t get_press_r();
    uint16_t get_key();
    int16_t get_wheel();

private:
    uint8_t rx_buffer[18]{};
    remote_t info{};
};

#endif //UART_REMOTE_H
