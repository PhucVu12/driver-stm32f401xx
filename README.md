# 🔧 STM32F401xx Bare-Metal Driver Development

This repository contains bare-metal driver implementations for the **STM32F401xx** microcontroller.  
All drivers are written from scratch, without using STM32 HAL or CMSIS-Driver libraries, to provide a deeper understanding of the low-level hardware operations.

---

## 📚 What This Project Covers

Writing low-level drivers for the following peripherals:

- ✅ **GPIO (General Purpose Input/Output)**
- ✅ **I2C (Inter-Integrated Circuit)**
- ✅ **SPI (Serial Peripheral Interface)**
- ✅ **USART (Universal Synchronous/Asynchronous Receiver Transmitter)**

Each driver is built step-by-step with direct register-level access, fully compliant with the **STM32F401 reference manual** and **datasheet**.

---

## 📂 Folder 

- 📁 `stm32f4xx_drivers/driver`: Contains all driver source files and headers for GPIO, SPI, I2C, and USART.
- 📁 `stm32f4xx_drivers/Src`: Contains test application code to demonstrate and validate driver functionalities.

---

## 🎯 Objectives

- Learn how microcontroller peripherals work at the register level.
- Understand bitwise operations and memory-mapped I/O.
- Gain full control over the MCU without abstraction layers.
- Build a solid foundation for professional embedded development.

---

## 🎓 Course Reference

Project comes with guidance from FastBit Embedded Brain Academy's Mastering Microcontroller and Embedded Driver Development course on Udemy.

---

## 🛠️ Tools & Requirements

- **MCU**: STM32F401CCU6 
- **Toolchain**: ARM GCC Toolchain / STM32CubeIDE 
- **Debugger**: ST-Link V2 
- **Editor**: VS Code / STM32CubeIDE / your favorite editor
- **Documents**: STM32F401xx Reference Manual & Datasheet
