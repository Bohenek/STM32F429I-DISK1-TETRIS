# 🧱 Tetris on STM32 with SPI Display
Tetris game project developed as part of microcontroller programming studies.  
Author: Oskar Szyszka  
Index number: 21308  
Start date: 01.04.2025  
The repository contains source code and project materials.  
## 📌 Project description
The aim of the project is to implement the classic Tetris game on the STM32F429I-DISC1 microcontroller. The game is displayed on an external LCD display with an ILI9341 controller connected via the SPI interface. Control is performed using physical buttons on the controller connected via the RJ12 interface to the microcontroller housing and then to the microcontroller's GPIO pins or commands sent via the UART serial port (e.g., from a PC terminal).
## 🛠️ Technologies and tools used
Microcontroller: STM32F429ZIT6 (on the STM32F429I-DISC1 board)  
LCD display: External module with ILI9341 controller (e.g., 2.4" 240x320), connected via SPI5.  
IDE: STM32CubeIDE  
Programming: C (using the HAL library)  
Graphics library: afiskon/stm32-ili9341 for SPI display support  
Communication/Control:  
UART (USART1 configured as Virtual COM Port by ST-LINK)  
### Physical buttons connected to GPIO pins (supported by EXTI interrupts).  
Power supply: 4xAA  
Connections to pins on the board are:  
controller buttons: PG5, PG6, PG7, PG8, PC9  
LEDs: PG2, PG3, PD14, PD15  
ADC pin: PF3  
5V power in: 5V  
power out to RC filter: VDD  
## ⚙️ Features  
✅ Full Tetris game logic: falling, moving, and rotating blocks.  
✅ Collision detection with walls and other blocks.  
✅ Removing complete lines and scoring points.  
✅ Increasing game speed with each level.  
✅ Level-dependent point multiplier.  
✅ Display of current score, high score, number of lines cleared (per level and total), and current level.  
✅ Preview of the next block.  
✅ Control using text commands via UART (e.g., ‘w’, ‘a’, ‘s’, ‘d’, ‘e’).  
✅ Control using physical buttons (Up/Rotate, Down, Left, Right, Start).  
✅ Display graphics on an external SPI ILI9341 display.  
✅ Grid background.  
✅ Line clearing animation and bonus point display.  
✅ Easter egg in the form of images on levels 4, 6, 8.  
✅ AA battery power supply after voltage reduction with filter diodes.  
✅ RC filter removing interference on GPIO lines.  
✅ Voltage divider for ADC to measure battery status.  
✅ LEDs showing current battery status.  
## 📁 Repository structure
/Core/Inc: Project header files, including main.h, ili9341.h, fonts.h.  
/Core/Src: Project source files, including main.c, ili9341.c, fonts.c, stm32f4xx_it.c.  
TETRIS.ioc: STM32CubeMX configuration file.  
/Media/: Project photos and screenshots.  
README_EN.md: This file.  
## 🔌 UART Commands / Button Assignments
Control can be via UART (e.g., PuTTY, 115200 baud, 8N1) or physical buttons. Control flags are shared.  
Action    UART Command    Physical Button  
Rotate    w or W    BTN_UP (PC9)  
Left    a or A    BTN_LEFT (PG5)  
Right    d or D    BTN_RIGHT (PG6)  
Down    s or S    BTN_DOWN (PG7)  
Start    e or E    BTN_START (PG8)  
The START button also functions as Rotate (UP) during active gameplay.  
## 📄 Additional Documentation  
[Report](TETRIS_REPORT_21308.pdf)  
[Tinkercad Schematic](SCHEMATIC.pdf)  
![SCHEMATIC](/Media/SCHEMATIC.png)  
## 📅 Work schedule
Weeks 1-4: Project configuration, SPI display initialization. All game software. Control via UART.  
Weeks 5-7: First implementation of physical button control via EXTI.  
Week 8:   AA battery power supply, second and final implementation of a physical controller connected to the microcontroller via RJ12, battery status indicator LEDs.  
Week 9:   Testing, additional functionalities in the form of simplified control.  
## 🧠 Conclusions and Possible Extensions
STM boards are an incredibly powerful and easy-to-use tool for creating all kinds of projects.  
### Possible extensions:  
Adding a Bluetooth module to enable wireless transmission of UART commands.  
Saving the highest score in non-volatile memory (e.g., Flash, EEPROM).  
## 📬 Contact
Email: bohen@debile.co / 21308@student.ans-elblag.pl  
GitHub: [Bohenek](https://github.com/Bohenek)  
### License: MIT
