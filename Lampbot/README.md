# Lampbot 

## 项目简介 📝

Lampbot是一个基于FreeRTOS的嵌入式系统控制项目，主要用于机器人控制。本项目实现了DR16遥控器通信、电机控制等功能，为机器人提供稳定可靠的控制系统。

## 功能特点 ✨

- 🎮 DR16遥控器通信接口
- 📊 调试任务支持
- 🔌 USB通信支持
- 🏍️ 电机控制系统
- 🔄 基于FreeRTOS的多任务管理
- 🔧 PID控制算法实现
- 📡 CAN总线通信支持

## 开发环境 🛠️

- 开发语言: C/C++
- 操作系统: FreeRTOS
- 推荐IDE: CLion/STM32CubeIDE
- 编译工具: CMake
- MCU平台: STM32系列

## 项目结构 📂

```
Lampbot/
├── User/                  # 用户代码
│   ├── UserTask/          # 任务代码
│   ├── UserLib/           # 用户库
│   └── Algorithm/         # 算法实现
├── Drivers/               # 驱动代码
│   ├── BSP/               # 板级支持包
│   └── CMSIS/             # CMSIS接口
├── Middlewares/           # 中间件
│   ├── FreeRTOS/          # FreeRTOS源码
│   └── USB_Device/        # USB设备库
├── Core/                  # 核心文件
├── .gitignore             # Git忽略文件
└── README.md              # 项目说明文档
```

## 主要模块 📚

### 遥控器模块 (DR16)

实现了DR16遥控器数据的接收和解析，支持摇杆、按键、鼠标等输入。

### 任务模块

包含以下主要任务：
- 调试任务 (AppTask_Debug)
- USB通信任务 (AppTask_USB)
- 电机控制任务 (AppTask_Motor)

### 控制算法


## 如何使用 🚀

1. 克隆仓库到本地
   ```bash
   git clone https://github.com/your-username/Lampbot.git
   ```

2. 使用支持的IDE打开项目（如CLion）

3. 编译项目并下载到硬件设备

4. 连接DR16遥控器进行控制

## 调试指南 🔍

1. 通过USB接口连接到计算机
2. 使用串口调试工具（如：串口助手、PuTTY）
3. 波特率设置为115200，8位数据位，1位停止位，无奇偶校验
4. 发送特定指令进行调试，详见`UserTask/debug_task.c`中的命令列表

## 注意事项 ⚠️

- 确保硬件连接正确，特别是UART接口
- 任务函数必须在`task_pubilc.h`中声明，否则在`freertos.c`中将找不到相应的函数符号
- 开发新功能时请新建分支，不要直接在main分支上开发

## 贡献指南 👥

欢迎提交问题和建议，也欢迎提交Pull Request贡献代码。

## 作者 👨‍💻

- Wenxin Hu

## 许可证 📄

本项目采用MIT许可证 - 详见LICENSE文件

---

*最后更新于 2023-11-20*
