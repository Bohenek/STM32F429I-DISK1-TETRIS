🧱 Tetris na STM32 z Wyświetlaczem SPI
Projekt gry Tetris zrealizowany w ramach nauki programowania mikrokontrolerów.
Autor: Oskar Szyszka
Numer indeksu:21308
Data rozpoczęcia: 01.04.2025
Repozytorium zawiera kod źródłowy oraz materiały projektowe.
📌 Opis projektu
Celem projektu jest implementacja klasycznej gry Tetris na mikrokontrolerze STM32F429I-DISC1. Gra jest wyświetlana na zewnętrznym wyświetlaczu LCD z kontrolerem ILI9341, podłączonym przez interfejs SPI. Sterowanie odbywa się za pomocą fizycznych przycisków na kontrolerze podłączonego przez interfejs RJ12 do obudowy mikrokontrolera a następnie do pinów GPIO mikrokontrolera lub komend wysyłanych przez port szeregowy UART (np. z terminala na PC)
🛠️ Zastosowane technologie i narzędzia
Mikrokontroler: STM32F429ZIT6 (na płytce STM32F429I-DISC1)
Wyświetlacz LCD: Zewnętrzny moduł z kontrolerem ILI9341 (np. 2.4" 240x320), podłączony przez SPI5.
IDE: STM32CubeIDE
Programowanie: C (z wykorzystaniem biblioteki HAL)
Biblioteka Graficzna: afiskon/stm32-ili9341 dla obsługi wyświetlacza SPI
Komunikacja/Sterowanie:
UART (USART1 skonfigurowany jako Virtual COM Port przez ST-LINK)
Fizyczne przyciski podłączone do pinów GPIO (obsługa przez przerwania EXTI).
Zasilanie: 4xAA
przyciski kontrolera: PG5, PG6, PG7, PG8, PC9
diody LED: PG2, PG3, PD14, PD15
pin ADC: PF3
zasilanie 5 V: 5 V
wyjście zasilania do filtra RC: VDD
⚙️ Funkcjonalności
✅ Pełna logika gry Tetris: spadanie, przesuwanie i obracanie klocków.
✅ Wykrywanie kolizji ze ścianami i innymi klockami.
✅ Usuwanie pełnych linii i naliczanie punktów.
✅ Zwiększanie prędkości gry wraz z poziomami.
✅ Mnożnik punktów zależny od poziomu.
✅ Wyświetlanie aktualnego wyniku, najwyższego wyniku, liczby usuniętych linii (na poziomie i ogółem) oraz aktualnego poziomu.
✅ Podgląd następnego klocka.
✅ Sterowanie za pomocą komend tekstowych przez UART (np. 'w', 'a', 's', 'd', 'e').
✅ Sterowanie za pomocą fizycznych przycisków (Góra/Obrót, Dół, Lewo, Prawo, Start).
✅ Wyświetlanie grafiki na zewnętrznym wyświetlaczu SPI ILI9341.
✅ Tło planszy z siatką.
✅ Animacja czyszczenia linii i wyświetlanie punktów bonusowych.
✅ Easter egg w formie obrazów na poziomach 4, 6, 8
✅ Zasilanie z baterii AA po zmniejszeniu napięcia diodami filtrującymi
✅ Filtr RC usuwający zakłócenia na liniach GPIO
✅ Dzielnik napięcia dla ADC do mierzenia stanu baterii
✅ Diody LED pokazujace aktualny stan baterii
📁 Struktura repozytorium
/Core/Inc: Pliki nagłówkowe projektu, w tym main.h, ili9341.h, fonts.h.
/Core/Src: Pliki źródłowe projektu, w tym main.c, ili9341.c, fonts.c, stm32f4xx_it.c.
TETRIS.ioc: Plik konfiguracyjny STM32CubeMX.
/Media/: Zdjęcia, screenshoty projektu.
README_PL.md: Ten plik.
🔌 Komendy UART / Przypisanie Przycisków
Sterowanie może odbywać się przez UART (np. PuTTY, 115200 baud, 8N1) lub fizyczne przyciski. Flagi sterujące są współdzielone.
Akcja	Komenda UART	Przycisk Fizyczny
Obrót	w lub W	BTN_UP (PC9)
Lewo	a lub A	BTN_LEFT (PG5)
Prawo	d lub D	BTN_RIGHT (PG6)
Dół	s lub S	BTN_DOWN (PG7)
Start	e lub E	BTN_START (PG8)
Przycisk START działa również jako Obrót (UP) w trakcie aktywnej gry.
📄 Dokumentacja Dodatkowa
TETRIS_RAPORT_21308.pdf
📅 Harmonogram pracy
Tydzień 1-4: Konfiguracja projektu, inicjalizacja wyświetlacza SPI. Całe oprogramowanie gry. Sterowanie przez UART.
Tydzień 5-7: Pierwsza implementacja sterowania fizycznymi przyciskami przez EXTI.
Tydzień 8:   Zasilanie z baterii AA, Druga i finalna implementacja fizycznego kontrolera łączonego z mikrokontrolerem przez RJ12, Diody sygnalizujące stan baterii.
Tydzień 9:   Testy, dodatkowe funkcjonalności w formie uproszczenia sterowania.
🧠 Wnioski i Możliwe Rozszerzenia
Płytki STM to niesamowicie potężne i proste w użyciu narzędzie do tworzenia wszelakiej maści projektów.
Możliwe rozszerzenia:
Dodanie modułu Bluetooth umożliwiającego przesyłanie komend UART bezprzewodowo.
Zapisywanie najwyższego wyniku w pamięci nieulotnej (np. Flash, EEPROM).
📬 Kontakt
Email: bohen@debile.co / 21308@student.ans-elblag.pl
GitHub: Bohenek
Licencja: MIT 
