[MOOZ][], the industrial grade transformable metallic 3D printer, which takes all-in-one design with interchangeable heads for 3D printing, laser engraving and CNC carving. The industrial grade linear guideway, stepper motor and CNC-machined body keeps [MOOZ][] stable and 0.02mm high precision. Quick assembly, protective case, easy-to-use controller, and intuitive guide make [MOOZ][] suitable for beginners, makers and educators. [MOOZ][] **has won the iF DESIGN AWARD 2018 for the innovative design and multiple functions**.

For more information, refer to [Product details][].

---

# Table of Contents

* [Hardware Platform For MOOZ](#hardware-platform-for-mooz)
* [How is MOOZ developed](#how-is-mooz-developed)
  * [3D Printer Firmware](#3d-printer-firmware)
  * [Toolchain Selection](#toolchain-selection)
  * [Basic Hardware Adaption](#basic-hardware-adaption)
  * [Advanced Features](#advanced-features)
    * [Bootloader Application Architecture](#bootloader-application-architecture)
    * [Higher System Core Frequency](#higher-system-core-frequency)
    * [Fan PWM Control](#fan-pwm-control)
    * [External SPI Flash](#external-spi-flash)
    * [File System Support](#file-system-support)
    * [HMI Support](#hmi-support)
* [Why we share the source code](#why-we-share-the-source-code)

# Hardware Platform For MOOZ

The hardware platform for [MOOZ][] consists of pieces of PCBA boards, all developed by the [MOOZ][] team. The main controller is [STM32F407ZG][], a high performance [ARM Cortex-M4F][] MCU. In addition to basiclly develop a normal 3D printer, which communicates with host via Serial port, we added some extra features on the hardware platform and for the product, like USB disk and SD card read/write, UART HMI operation, etc.

# How is MOOZ developed

## 3D Printer Firmware

The original version of [MOOZ][] 3D Printer Firmware is forked from the repository [Marlin4ST 1.0.X][]. As it describes,

> This repository proposes an example firmware for the ST Microelectronics 3D printer board STEVAL-3DP001V1.
The Marlin4ST firmware relies on [STM32Cube0](http://www.st.com/web/catalog/tools/FM147/CL1794/SC961/SS1743/LN1897?s_searchtype=reco) and integers the 3D printer algorithms from the [Marlin 1.0.X](https://github.com/MarlinFirmware/Marlin/tree/1.0.x) firmware. The official Marlin 1.0.X firmware is now closed.

## Toolchain Selection

The original project [Marlin4ST 1.0.X][] uses [OpenSTM32][] toolchain. At the very beginning, the [MOOZ][] team chooses the same toolchain, but at last, the team chooses [MDK-ARM Microcontroller Development Kit][], for development efficiency and easy integration of drivers for new features.

## Basic Hardware Adaption

The hardware is almost all different from that of [STEVAL-3DP001V1][]. We did a lot of work to make it run, which consists of,

- Change clock source of the controller
- Change nearly all the I/O maps for motor direction and pulse control, temperature sampling, heating, etc., and the corresponding initialization code
- Adaption for any other hardware differences 

## Advanced Features

### Bootloader Application Architecture

On the [MOOZ][] product, we propose a `Bootloader`--`Application` architecture, to make it easily and safely for users to upgrade the application firmware, and no need to disassemble the integrated structure of [MOOZ][].

For users who want to customize [MOOZ][] 3D printer firmware, 

- Select suitable toolchains
- The total flash memory for [STM32F407ZG][] is 1M Bytes, and the bootloader memory is designed between 0x08000000 and 0x0803FFFF(256kB). The total memory size reserved for application is 768kB
- Change to start address of application code and the base address of interrupt vector table to 0x08040000
- Build the project
- Generate the application binary file using fromelf
- Rename the binary to `MOOZ1V1.2.3.bin` or `MOOZ2V3.4.5.bin` style name, prefix with `MOOZ1` for MOOZ-1Z product, and `MOOZ2` for MOOZ-2Z product
- Copy the binary file to the root directory of USB disk or SD card, and re-power the [MOOZ][]
- Wait for the application upgrading

### Higher System Core Frequency

The original project runs the controller on 16MHz frequency using internal clock source. To fully utilize the performance of the controller, we runs [MOOZ][] on 84MHz.

### Fan PWM Control

Add fan PWM control, which is not provided by the original project.

### External SPI Flash

Add external SPI Flash to store some importance configuration of [MOOZ][].

### File System Support

Though the original project supports file system over SD card, it still needs some work. 

- Change the [FatFs][] porting code to support multiple storage instance
- Integrate `STM32_USB_HOST_Library` to support USB disk driver. Also we can do a lot besides this by implementing different class drivers
- Configure the [FatFs][] to add `Long File Name Support` feature

### HMI Support

The operation panel of [MOOZ][] is an UART HMI. In addition to add software driver for it, we designed a full set of UI on it. The HMI acts as a host on this platform.

# Why we share the source code

[GitHub][] is a community that admires highly of sharing, and we would like to open the source code of the [MOOZ][] 3D printer firmware, to follow this virtue of sharing, and meanwhile to collect excellent feedbacks and opinions from you, to encourage the [MOOZ][] team to make a better product in the next generation. Let's move on! Join this forum to get more support: [MOOZ Forum](http://forum.dobot.cc/c/customer-support-dobot-mooz).

[MOOZ]: https://www.dobot.cc/products/mooz-overview.html
[Product details]: https://www.dobot.cc/products/mooz-overview.html
[STM32F407ZG]: http://www.st.com/en/microcontrollers/stm32f407zg.html
[ARM Cortex-M4F]: https://en.wikipedia.org/wiki/ARM_Cortex-M#Cortex-M4
[Marlin4ST 1.0.X]: https://github.com/St3dPrinter/Marlin4ST/tree/1.0.X
[STEVAL-3DP001V1]: http://www.st.com/en/evaluation-tools/steval-3dp001v1.html
[OpenSTM32]: http://www.openstm32.org/HomePage
[MDK-ARM Microcontroller Development Kit]: http://www.keil.com/products/arm/mdk.asp
[FatFs]: http://elm-chan.org/fsw/ff/00index_e.html
[Github]: https://github.com/
