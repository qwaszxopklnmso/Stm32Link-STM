# Stm32Link_Keil — STM32 Firmware

[中文](README.zh-CN.md) | English

Keil MDK project for the STM32F103C8T6 side of the
[Stm32Link](https://github.com/qwaszxopklnmso/Stm32Link) project. It drives
the link lamps, telemetry transmitter/receiver, buzzer and OLED, and speaks
the 115200 8N1 line protocol used by the Minecraft mod.

## Build and flash

1. Open `main.uvprojx` in Keil MDK (ARM Compiler 5).
2. Build (F7): expect `0 Error(s), 0 Warning(s)`.
3. Connect ST-Link and Download (F8), or flash `Objects/main.hex` over
   serial ISP.

## Pin map

| Function | Pin |
|---|---|
| Buzzer V1 (melody) | PA0 / TIM2_CH1 |
| Buzzer V2 (harmony) | PA8 / TIM1_CH1 |
| Buzzer V3 (fill) | PA6 / TIM3_CH1 |
| Buzzer V4 (piezo beeper) | PA5 (software square wave) |
| LED1–LED3 | PA1–PA3 |
| T-LED | PA4 |
| CH340 TX/RX | PA9 / PA10 |
| Buttons B1–B4 | PB12–PB15 |
| OLED SCL/SDA | PB8 / PB9 |

Default buzzer trigger polarity is **V1–V3 high (NPN electromagnetic)** and
**V4 low (piezo)**. See the project hub's `STM32_Set.md` for wiring,
including the 16 Ω electromagnetic buzzers, S8050 driver and 1N4007 flyback
diode.

Button roles: **B1** = spare; **B2** = pulse receiver; **B3** = toggle
receiver; **B4** = level receiver (hold = on, release = off). LED1–LED3 are
output-only: the firmware sends no L report back to MC.

## Related repositories

- **Minecraft mod**:
  <https://github.com/qwaszxopklnmso/Stm32Link-Mod>
- **Project hub and wiring/deployment docs**:
  <https://github.com/qwaszxopklnmso/Stm32Link>

## License

The original project code is MIT; see [LICENSE](LICENSE). The STM32F10x
Standard Peripheral Library and CMSIS/startup files retain ST's licenses; see
[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).
