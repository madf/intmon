# IntMon

A toy project for STM32F401CDU6.

Internal conditions monitor. Will utilize BME280 as a conditions sensor and SSD1306-based OLED display.

Bare metal, Cube/CubeMX is not used. Hardware initialization is derived from the MCU docs.

Additional info: https://github.com/cpq/bare-metal-programming-guide

### Building

This project assumes that the following software is available in your PATH:
 * `arm-none-eabi-gcc`
 * `arm-none-eabi-g++`
 * `arm-none-eabi-as`
 * `arm-none-eabi-objcopy`
 * `arm-none-eabi-objdump`
 * `arm-none-eabi-strip`
 * `arm-none-eabi-size`

To create ready-to-flash binary just type `make`. Your binary will be `firmware.bin`.

### Flashing

```
st-flash write firmware.bin 0x08000000
```
