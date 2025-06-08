# Tetris na STM32 z Wyświetlaczem SPI: Adaptacja i Rozwój 🇵🇱
Ten projekt to implementacja klasycznej gry Tetris na mikrokontrolerze STM32F429I-DISC1, wykorzystująca wyświetlacz LCD z kontrolerem ILI9341 podłączony przez interfejs SPI. 
Sterowanie odbywa się za pomocą komend UART oraz fizycznych przycisków.
## Fundamenty Projektu  
Realizacja tego projektu była możliwa dzięki inspiracji i wykorzystaniu fundamentalnych elementów z istniejących rozwiązań open-source. Rdzeń logiki gry Tetris został zaadaptowany z projektów:

[offpic/TETRIS-STM32 ](https://github.com/offpic/TETRIS-STM32-TFT-ILI9341-SPI-STM32F401)

[KenKenMkIISR/picotetris](https://github.com/KenKenMkIISR/picotetris)

Te projekty dostarczyły solidnej podstawy dla mechaniki spadania klocków, wykrywania kolizji oraz logiki usuwania linii.

Za obsługę grafiki i komunikację z wyświetlaczem LCD ILI9341 odpowiada biblioteka:

[afiskon/stm32-ili9341](https://github.com/afiskon/stm32-ili9341)

Użycie tej biblioteki było kluczowe dla renderowania grafiki na podłączonym module SPI.

### [Bardziej szczegółowe Readme w języku polskim 🇵🇱](https://github.com/Bohenek/STM32F429I-DISK1-TETRIS/blob/main/README_PL.md)

# Tetris on STM32 with SPI Display: Adaptation and Development 🇬🇧
This project is an implementation of the classic Tetris game on the STM32F429I-DISC1 microcontroller, using an LCD display with an ILI9341 controller connected via the SPI interface. 
Control is performed using UART commands and physical buttons.
## Project Fundamentals
The implementation of this project was made possible thanks to inspiration and the use of fundamental elements from existing open-source solutions. The core logic of the Tetris game was adapted from the following projects:

[offpic/TETRIS-STM32 ](https://github.com/offpic/TETRIS-STM32-TFT-ILI9341-SPI-STM32F401)

[KenKenMkIISR/picotetris](https://github.com/KenKenMkIISR/picotetris)

These projects provided a solid foundation for the mechanics of falling blocks, collision detection, and line removal logic.

The library is responsible for graphics handling and communication with the ILI9341 LCD display:

[afiskon/stm32-ili9341](https://github.com/afiskon/stm32-ili9341)

The use of this library was crucial for rendering graphics on the connected SPI module.

### [More detailed Readme in english 🇬🇧](https://github.com/Bohenek/STM32F429I-DISK1-TETRIS/blob/main/README_EN.md)
