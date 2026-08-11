# STM32-FreeRTOS-CAN

基于 STM32F103C8T6 + FreeRTOS 的 CAN 接收端项目，500kbps 波特率，中断直接入队，双任务处理。

## 功能特性

- CAN 总线 500kbps 通信
- FreeRTOS 多任务：数据处理任务 + 串口打印任务
- 中断中直接向队列发送 CAN 数据（绕过信号量）
- IDLIST 过滤器仅接收标准 ID 0x100
- 串口（USART1）实时打印帧 ID、DLC 及 8 字节数据

## 硬件平台

| 组件 | 型号 |
| :--- | :--- |
| MCU | STM32F103C8T6 |
| CAN 收发器 | SN65HVD230（微雪模块，带 ESD 保护） |
| 调试串口 | CH340 (USB 转 TTL) |

## 引脚连接

| 功能 | STM32 引脚 | 外设引脚 |
| :--- | :--- | :--- |
| CAN_RX | PA11 | SN65HVD230 RXD |
| CAN_TX | PA12 | SN65HVD230 TXD |
| USART1_TX | PA9 | CH340 RX |
| USART1_RX | PA10 | CH340 TX |
| LED | PC13 | 板载 LED |

## CAN 配置参数

| 参数 | 值 |
| :--- | :--- |
| 波特率 | 500 kbps |
| 工作模式 | Normal 模式 |
| 过滤器模式 | IDLIST（仅接收 ID 0x100） |
| 时钟源 | HSE 外部晶振 (8MHz → 72MHz PLL) |

## 使用说明

1. 给 STM32 板供电（USB 或 3.3V）
2. 连接 CAN 总线（CAN_H ↔ CAN_H，CAN_L ↔ CAN_L）
3. 两端各接一个 120Ω 终端电阻
4. 串口连接：PA9 → CH340 RX，PA10 → CH340 TX
5. 打开串口助手，波特率 115200
6. 发送端发送 ID 为 0x100 的 CAN 帧，接收端打印：
ID:0x100 Data:11 22 33 44 55 66 77 88


## 测试结果

发送端以 500ms 周期发送 ID 为 0x100 的 CAN 数据帧，数据字节逐帧递增。接收端上电后打印启动信息，并持续输出接收到的 CAN 数据帧：

```text
FreeRTOS + CAN 接收端启动
ID:0x100 Data:11 22 33 44 55 66 77 88
ID:0x100 Data:12 23 34 45 56 67 78 89
ID:0x100 Data:13 24 35 46 57 68 79 8A
ID:0x100 Data:14 25 36 47 58 69 7A 8B
...
数据逐帧递增，通信稳定，无丢帧或误码。


## 操作步骤

1. 在 GitHub 仓库页面，点击 `README.md` 文件。
2. 点击右上角的 **铅笔图标（Edit）**。
3. 找到“测试结果”部分，用上面的内容替换。
4. 提交修改（Commit changes）。

---

## 完整 README 的“测试结果”部分预览

修改后，README 中测试结果部分会显示为：

> **测试结果**
>
> 发送端以 500ms 周期发送 ID 为 0x100 的 CAN 数据帧，数据字节逐帧递增。接收端上电后打印启动信息，并持续输出接收到的 CAN 数据帧：
>
> ```
> FreeRTOS + CAN 接收端启动
> ID:0x100 Data:11 22 33 44 55 66 77 88
> ID:0x100 Data:12 23 34 45 56 67 78 89
> ID:0x100 Data:13 24 35 46 57 68 79 8A
> ID:0x100 Data:14 25 36 47 58 69 7A 8B
> ...
> ```
>
> 数据逐帧递增，通信稳定，无丢帧或误码。


数据逐帧递增，通信稳定。

## 工程结构

```text
STM32-FreeRTOS-CAN/
├── Core/
│   ├── Inc/               # 头文件
│   │   ├── FreeRTOSConfig.h
│   │   ├── main.h
│   │   ├── can.h
│   │   ├── usart.h
│   │   └── gpio.h
│   └── Src/               # 源文件
│       ├── main.c
│       ├── freertos.c
│       ├── can.c
│       ├── usart.c
│       └── gpio.c
├── Drivers/               # HAL 库驱动
│   ├── CMSIS/
│   └── STM32F1xx_HAL_Driver/
├── Middlewares/           # FreeRTOS 中间件
│   └── Third_Party/
│       └── FreeRTOS/
└── Debug/                 # 编译输出（已忽略）
```


## 作者

- **GitHub**: [jia-sheng2026](https://github.com/jia-sheng2026)

## 许可

MIT License
