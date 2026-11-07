# Stm32Link_Keil — STM32 固件

[English](README.md) | 中文

[Stm32Link](https://github.com/qwaszxopklnmso/Stm32Link) 项目的 STM32F103C8T6
端 Keil MDK 工程。它驱动联控灯、遥测发射器/接收器、蜂鸣器和 OLED，并通过
115200 8N1 行协议与 Minecraft 模组通信。

## 构建与烧录

1. 用 Keil MDK（ARM Compiler 5）打开 `main.uvprojx`。
2. Build（F7）：应为 `0 Error(s), 0 Warning(s)`。
3. 接 ST-Link，Download（F8）；或用串口 ISP 烧 `Objects/main.hex`。

## 引脚

| 功能 | 引脚 |
|---|---|
| 蜂鸣器 V1（旋律） | PA0 / TIM2_CH1 |
| 蜂鸣器 V2（和声） | PA8 / TIM1_CH1 |
| 蜂鸣器 V3（填充） | PA6 / TIM3_CH1 |
| 蜂鸣器 V4（压电提示音） | PA5（软件方波） |
| LED1–LED3 | PA1–PA3 |
| T-LED | PA4 |
| CH340 TX/RX | PA9 / PA10 |
| 按键 B1–B4 | PB12–PB15 |
| OLED SCL/SDA | PB8 / PB9 |

蜂鸣器默认触发极性：**V1–V3 高触发（NPN 电磁式）**，**V4 低触发（压电）**。
接线、16Ω 电磁蜂鸣器、S8050 驱动和 1N4007 续流二极管见项目入口的
`STM32_Set.md`。

按键分工：**B1** 复位全部接收器输出（紧急清零）；**B2** 脉冲接收器；
**B3** 触发接收器；**B4** 电平接收器（按住开，松开灭）。LED1–LED3 只做输出，
固件不回传 LED 状态。

## 相关仓库

- **Minecraft 模组**：
  <https://github.com/qwaszxopklnmso/Stm32Link-Mod>
- **项目入口 / 接线与部署文档**：
  <https://github.com/qwaszxopklnmso/Stm32Link>

## 许可证

项目原始代码为 MIT，见 [LICENSE](LICENSE)。STM32F10x 标准外设库和
CMSIS/启动文件保留 ST 的许可证，见 [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)。
