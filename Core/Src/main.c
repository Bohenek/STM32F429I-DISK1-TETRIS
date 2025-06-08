/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ili9341.h"
#include "fonts.h"
#include "level_imgs.h"
#include "gameover_img.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct {
	int8_t x1, y1;
	int8_t x2, y2;
	int8_t x3, y3;
	uint16_t color;
	int8_t rot;
} _Block;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define KEY_IS_DOWN_PRESSED (HAL_GPIO_ReadPin(BTN_DOWN_GPIO_Port, BTN_DOWN_Pin) == GPIO_PIN_RESET)
#define COLOR_BG ILI9341_BLACK
#define COLOR_WALL ILI9341_BLUE
#define COLOR_SCORE_TEXT ILI9341_WHITE
#define COLOR_INFO_TEXT ILI9341_GREEN
#define COLOR_GAMEOVER ILI9341_RED
#define COLOR_FRAME 0xC638
#define O_BLOCK ILI9341_YELLOW
#define I_BLOCK ILI9341_CYAN
#define S_BLOCK ILI9341_GREEN
#define Z_BLOCK ILI9341_RED
#define L_BLOCK ILI9341_COLOR565(255, 165, 0)
#define J_BLOCK ILI9341_BLUE
#define T_BLOCK ILI9341_MAGENTA
#define COLOR_SPACE 0
#define COLOR_CLEARBLOCK ILI9341_WHITE
#define COLOR_GRID ILI9341_COLOR565(64, 64, 64)
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 23
#define BOARD_ARRAY_WIDTH 12
#define BOARD_ARRAY_HEIGHT 25
#define BLOCK_SIZE 10
#define BOARD_X_OFFSET (ILI9341_WIDTH / 2 - (10*BLOCK_SIZE / 2))
#define BOARD_Y_OFFSET 80
#define SCORE_X_OFFSET 10
#define SCORE_Y_OFFSET 10
#define NEXT_X_OFFSET (ILI9341_WIDTH - (5 * BLOCK_SIZE) - 10 - 2)
#define NEXT_Y_OFFSET 22
#define NEXT_AREA_W (5 * BLOCK_SIZE)
#define NEXT_AREA_H (5 * BLOCK_SIZE)
#define NEXT_LABEL_H Font_7x10.height
#define NEXT_PADDING 2
#define SCENECLEARLINE 2
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc3;

SPI_HandleTypeDef hspi5;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
		// ---------- UART INPUT ----------
volatile uint8_t input_buffer[1];
volatile uint8_t uart_up=0;
volatile uint8_t uart_left=0;
volatile uint8_t uart_right=0;
volatile uint8_t uart_down=0;
volatile uint8_t uart_start=0;
volatile uint8_t physical_down_key_held = 0;
		// ---------- GAME STATE VARS ----------
uint16_t  board[BOARD_ARRAY_HEIGHT][BOARD_ARRAY_WIDTH];
uint16_t  boardchange[BOARD_ARRAY_HEIGHT][BOARD_ARRAY_WIDTH];
uint32_t score, highscore;
uint32_t total_lines = 0;
uint8_t  next;
uint8_t  fallspeed, fallcount;
uint8_t  level;
uint8_t  gamestatus;
uint8_t  lines;
const uint16_t scorearray[] = {40, 100, 300, 1200};
		// ---------- BLOCK DEFS ----------
const _Block block[7] = {
    {1, 0, -1, 0, -2, 0, I_BLOCK, 1}, // I 
    {1, 0, -1, 0, -1,-1, J_BLOCK, 3}, // J 
    {1, 0,  1,-1, -1, 0, L_BLOCK, 3}, // L 
    {1, 0,  0,-1, -1,-1, Z_BLOCK, 1}, // Z 
    {0,-1,  1,-1, -1, 0, S_BLOCK, 1}, // S 
    {0,-1, -1, 0, -1,-1, O_BLOCK, 0}, // O 
    {1, 0,  0,-1, -1, 0, T_BLOCK, 3}  // T 
};
_Block falling;
int8_t  blockx, blocky;
uint8_t blockangle;
uint8_t blockno;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI5_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_ADC3_Init(void);
/* USER CODE BEGIN PFP */
void updateLEDs(void);
void gameinit(void);
void gameinit2(void);
void gameinit3(void);
void title(void);
void game(void);
void clearscreen(void);
void DrawGridCell(uint16_t cell_px, uint16_t cell_py);
void show(void);
void displayscore(void);
void printnext(void);
int  check(_Block *bp, int8_t x, int8_t y);
void putblock(void);
void eraseblock(void);
int  newblock(void);
void moveblock(void);
void linecheck(void);
void displaylevel(void);
void redraw(void);
void gameover(void);
unsigned char startkeycheck(unsigned short n);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void updateLEDs(void) {
  uint32_t sum = 0;
  for (int i = 0; i < 10; i++) {
    HAL_ADC_Start(&hadc3);
    HAL_ADC_PollForConversion(&hadc3, HAL_MAX_DELAY);
    sum += HAL_ADC_GetValue(&hadc3);
    HAL_Delay(1);
  }
  uint32_t adcValue = sum / 10;

  float battV = adcValue * 0.00161; 		// V_in = ADC * (3.3/4095) * (R1+R2)/R2

  if (battV > 5.4) { 						// 100% // >3350 // PG3 PG2 PG15 PG14
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 1);
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 1);
    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 1);
    HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, 1);
  } else if (battV > 4.9) { 				// 75% // 3043–3350 // PG2 PG15 PG14
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 1);
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 1);
    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 1);
    HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, 0);
  } else if (battV > 4.4) { 				// 50% // 2730–3043 // PG15 PG14
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 1);
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 1);
    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 0);
    HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, 0);
  } else if (battV > 3.9) { 				// 25% // 2418–2730 // PG14
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 1);
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 0);
    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 0);
    HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, 0);
  } else {
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 0);
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 0);
    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 0);
    HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, 0);
  }

  HAL_Delay(4000);

  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 0);
  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 0);
  HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 0);
  HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, 0);
}
void clearscreen(void)
{
	ILI9341_FillScreen(COLOR_BG);
}
void gameinit(void)
{
	highscore = 0;
	score = 0;
	total_lines = 0;
}
void gameinit2(void)
{
	score = 0;
	level = 0;
	lines = 0;
	total_lines = 0;
	fallspeed = 25;
	gamestatus = 1;
	clearscreen();
		//GAME AREA
	uint16_t board_frame_x = BOARD_X_OFFSET - 1;
	uint16_t board_frame_y = BOARD_Y_OFFSET - 1;
	uint16_t board_frame_w = BOARD_WIDTH * BLOCK_SIZE + 2; // Width including border pixels
	uint16_t board_frame_h = BOARD_HEIGHT * BLOCK_SIZE + 2; // Height including border pixels
		// Top line
	ILI9341_FillRectangle(board_frame_x, board_frame_y, board_frame_w, 1, COLOR_FRAME);
		// Bottom line
	ILI9341_FillRectangle(board_frame_x, board_frame_y + board_frame_h - 1, board_frame_w, 1, COLOR_FRAME);
		// Left line
	ILI9341_FillRectangle(board_frame_x, board_frame_y, 1, board_frame_h, COLOR_FRAME);
		// Right line
	ILI9341_FillRectangle(board_frame_x + board_frame_w - 1, board_frame_y, 1, board_frame_h, COLOR_FRAME);


		//SCORE AREA
	uint16_t score_frame_x = SCORE_X_OFFSET - 1;
	uint16_t score_frame_y = SCORE_Y_OFFSET - 1;
	uint16_t score_frame_w = 80;
	uint16_t score_frame_h = 5 * (Font_7x10.height + 2) + 4;
	    // Top
	ILI9341_FillRectangle(score_frame_x, score_frame_y, score_frame_w, 1, COLOR_FRAME);
	    // Bottom
	ILI9341_FillRectangle(score_frame_x, score_frame_y + score_frame_h - 1, score_frame_w, 1, COLOR_FRAME);
	    // Left
	ILI9341_FillRectangle(score_frame_x, score_frame_y, 1, score_frame_h, COLOR_FRAME);
	    // Right
	ILI9341_FillRectangle(score_frame_x + score_frame_w - 1, score_frame_y, 1, score_frame_h, COLOR_FRAME);
		//NEXT AREA
	 uint16_t next_frame_x = NEXT_X_OFFSET - NEXT_PADDING;
	 uint16_t next_frame_y = NEXT_Y_OFFSET - NEXT_LABEL_H - NEXT_PADDING*2;
	 uint16_t next_frame_w = NEXT_AREA_W + NEXT_PADDING*2;
	 uint16_t next_frame_h = NEXT_LABEL_H + NEXT_AREA_H + NEXT_PADDING*2;
	    // Top
	ILI9341_FillRectangle(next_frame_x, next_frame_y, next_frame_w, 1, COLOR_FRAME);
	    // Bottom
	ILI9341_FillRectangle(next_frame_x, next_frame_y + next_frame_h - 1, next_frame_w, 1, COLOR_FRAME);
	    // Left
	ILI9341_FillRectangle(next_frame_x, next_frame_y, 1, next_frame_h, COLOR_FRAME);
	    // Right
	ILI9341_FillRectangle(next_frame_x + next_frame_w - 1, next_frame_y, 1, next_frame_h, COLOR_FRAME);
		// GAME AREA LOGIC
	for (int y = 0; y < BOARD_ARRAY_HEIGHT; y++)
	{
		for (int x = 0; x < BOARD_ARRAY_WIDTH; x++)
		{
			if (x == 0 || x == BOARD_ARRAY_WIDTH - 1 || y == BOARD_ARRAY_HEIGHT - 1)
			{
				board[y][x] = COLOR_WALL;
			}
			else {
				board[y][x] = COLOR_SPACE;
			}
		}
	}
		// DRAW PLAYFIELD BACKGROUND
	ILI9341_FillRectangle(BOARD_X_OFFSET, BOARD_Y_OFFSET, BOARD_WIDTH * BLOCK_SIZE, BOARD_HEIGHT * BLOCK_SIZE, COLOR_BG);
		// DRAW GRID
	for(int x = 1; x < BOARD_WIDTH; x++)
	{
		ILI9341_FillRectangle(BOARD_X_OFFSET + x * BLOCK_SIZE -1, BOARD_Y_OFFSET, 1, BOARD_HEIGHT * BLOCK_SIZE, COLOR_GRID);
	}
	for(int y = 1; y < BOARD_HEIGHT; y++) {ILI9341_FillRectangle(BOARD_X_OFFSET, BOARD_Y_OFFSET + y * BLOCK_SIZE -1, BOARD_WIDTH * BLOCK_SIZE, 1, COLOR_GRID);
	}
	for (int y = 0; y < BOARD_ARRAY_HEIGHT; y++)
	{
		for (int x = 0; x < BOARD_ARRAY_WIDTH; x++)
		{
			boardchange[y][x] = 1;
		}
	}
		// DISPLAYS
	displayscore();
	next = rand() % 7;
	printnext();
}
void gameinit3(void)
{
	level++;

	if (fallspeed > 5)
	{
		fallspeed -=3;
	}
	else if (fallspeed > 1) {
		   fallspeed--;
	}
	srand(HAL_GetTick()+level);
}
void DrawGridCell(uint16_t cell_px, uint16_t cell_py)
{
    ILI9341_FillRectangle(cell_px, cell_py + BLOCK_SIZE - 1, BLOCK_SIZE, 1, COLOR_GRID);
    ILI9341_FillRectangle(cell_px + BLOCK_SIZE - 1, cell_py, 1, BLOCK_SIZE, COLOR_GRID);
}
void show(void)
{
	uint16_t color_to_draw;
	for (int y = 1; y <= BOARD_HEIGHT; y++) {
		for (int x = 1; x <= BOARD_WIDTH; x++) {
			if (boardchange[y][x]) {
                uint16_t px = BOARD_X_OFFSET + (x - 1) * BLOCK_SIZE;
                uint16_t py = BOARD_Y_OFFSET + (y - 1) * BLOCK_SIZE;

				if (board[y][x] == COLOR_SPACE)
	            {
					color_to_draw = COLOR_BG;
                    ILI9341_FillRectangle(px, py, BLOCK_SIZE, BLOCK_SIZE, color_to_draw);
                    DrawGridCell(px, py);
	            } else {
	            	color_to_draw = board[y][x];
                    ILI9341_FillRectangle(px, py, BLOCK_SIZE, BLOCK_SIZE, color_to_draw);
	            }
				boardchange[y][x] = 0;
			}
	    }
	}
}
void displayscore(void)
{
	char buffer[20];
	uint8_t num_info_lines = 5;
	uint16_t line_height = Font_7x10.height + 2;
	uint16_t clear_width = 78;
	uint16_t clear_height = num_info_lines * line_height + 2;
		// CLEAR
	ILI9341_FillRectangle(SCORE_X_OFFSET, SCORE_Y_OFFSET, clear_width, clear_height, COLOR_BG);
		// Display Score
	sprintf(buffer, "SCORE:%lu", score);
	ILI9341_WriteString(SCORE_X_OFFSET, SCORE_Y_OFFSET + 2, buffer, Font_7x10, COLOR_SCORE_TEXT, COLOR_BG);

		// Display High Score
	sprintf(buffer, "HI   :%lu", highscore);
	ILI9341_WriteString(SCORE_X_OFFSET, SCORE_Y_OFFSET + line_height + 2, buffer, Font_7x10, COLOR_SCORE_TEXT, COLOR_BG);

		// Display Per-Level Lines
	sprintf(buffer, "LINES:%d", lines);
	ILI9341_WriteString(SCORE_X_OFFSET, SCORE_Y_OFFSET + 2 * line_height + 2, buffer, Font_7x10, COLOR_SCORE_TEXT, COLOR_BG);

	    // Display TOTAL Lines
	sprintf(buffer, "TOTAL:%lu", total_lines);
	ILI9341_WriteString(SCORE_X_OFFSET, SCORE_Y_OFFSET + 3 * line_height + 2, buffer, Font_7x10, COLOR_SCORE_TEXT, COLOR_BG);

		// Display Level
	sprintf(buffer, "LEVEL:%d", level);
	ILI9341_WriteString(SCORE_X_OFFSET, SCORE_Y_OFFSET + 4 * line_height + 2, buffer, Font_7x10, COLOR_SCORE_TEXT, COLOR_BG);
}
void printnext(void)
{
	const _Block *bp = &block[next];

	uint16_t preview_x = NEXT_X_OFFSET;
	uint16_t preview_y = NEXT_Y_OFFSET;
	uint16_t preview_w = NEXT_AREA_W;
	uint16_t preview_h = NEXT_AREA_H - 2;

	uint16_t label_w = strlen("NEXT") * Font_7x10.width;
	uint16_t label_h = NEXT_LABEL_H;
	uint16_t label_x = NEXT_X_OFFSET + (preview_w - label_w) / 2;
	uint16_t label_y = NEXT_Y_OFFSET - label_h - NEXT_PADDING;

	ILI9341_FillRectangle(label_x, label_y, label_w, label_h, COLOR_BG);
	ILI9341_FillRectangle(preview_x, preview_y, preview_w, preview_h, COLOR_BG);

	ILI9341_WriteString(label_x, label_y, "NEXT", Font_7x10, COLOR_INFO_TEXT, COLOR_BG);

	uint16_t base_center_x = NEXT_X_OFFSET + preview_w / 2;
	uint16_t base_center_y = NEXT_Y_OFFSET + preview_h / 2;

    uint16_t anchor_x = base_center_x - BLOCK_SIZE/2;
    uint16_t anchor_y = base_center_y - BLOCK_SIZE/2;

	switch (next) {
        case 0: // I-Bloc
            anchor_x += BLOCK_SIZE / 2;
            break;

        case 1: // J-Block
             anchor_y -= BLOCK_SIZE / 4;
            break;

        case 2: // L-Block
             anchor_y -= BLOCK_SIZE / 4;
            break;

        case 3: // Z-Block
             anchor_y += BLOCK_SIZE / 4;
            break;

        case 4: // S-Block
             anchor_y += BLOCK_SIZE / 4;
            break;

        case 5: // O-Block
            anchor_x += BLOCK_SIZE / 2;
            break;

         case 6: // T-Block
             anchor_y -= BLOCK_SIZE / 4;
            break;
        default:
            break;
	}
	ILI9341_FillRectangle(anchor_x + 0 * BLOCK_SIZE,         anchor_y + 0 * BLOCK_SIZE,         BLOCK_SIZE, BLOCK_SIZE, bp->color); // Center block ref
	ILI9341_FillRectangle(anchor_x + bp->x1 * BLOCK_SIZE,    anchor_y + bp->y1 * BLOCK_SIZE,    BLOCK_SIZE, BLOCK_SIZE, bp->color); // Block 1
	ILI9341_FillRectangle(anchor_x + bp->x2 * BLOCK_SIZE,    anchor_y + bp->y2 * BLOCK_SIZE,    BLOCK_SIZE, BLOCK_SIZE, bp->color); // Block 2
	ILI9341_FillRectangle(anchor_x + bp->x3 * BLOCK_SIZE,    anchor_y + bp->y3 * BLOCK_SIZE,    BLOCK_SIZE, BLOCK_SIZE, bp->color); // Block 3
}
void redraw(void)
{
	uint16_t board_frame_x = BOARD_X_OFFSET - 1;
	uint16_t board_frame_y = BOARD_Y_OFFSET - 1;
	uint16_t board_frame_w = BOARD_WIDTH * BLOCK_SIZE + 2;
	uint16_t board_frame_h = BOARD_HEIGHT * BLOCK_SIZE + 2;
	ILI9341_FillRectangle(board_frame_x, board_frame_y, board_frame_w, 1, COLOR_FRAME); // Top
	ILI9341_FillRectangle(board_frame_x, board_frame_y + board_frame_h - 1, board_frame_w, 1, COLOR_FRAME); // Bottom
	ILI9341_FillRectangle(board_frame_x, board_frame_y, 1, board_frame_h, COLOR_FRAME); // Left
	ILI9341_FillRectangle(board_frame_x + board_frame_w - 1, board_frame_y, 1, board_frame_h, COLOR_FRAME); // Right
	// Score Area Frame
	uint16_t score_frame_x = SCORE_X_OFFSET - 1;
	uint16_t score_frame_y = SCORE_Y_OFFSET - 1;
	uint16_t score_frame_w = 80;
	uint16_t score_frame_h = 5 * (Font_7x10.height + 2) + 4;
	ILI9341_FillRectangle(score_frame_x, score_frame_y, score_frame_w, 1, COLOR_FRAME); // Top
	ILI9341_FillRectangle(score_frame_x, score_frame_y + score_frame_h - 1, score_frame_w, 1, COLOR_FRAME); // Bottom
	ILI9341_FillRectangle(score_frame_x, score_frame_y, 1, score_frame_h, COLOR_FRAME); // Left
	ILI9341_FillRectangle(score_frame_x + score_frame_w - 1, score_frame_y, 1, score_frame_h, COLOR_FRAME); // Right
	// Next Area Frame
	uint16_t next_frame_x = NEXT_X_OFFSET - NEXT_PADDING;
	uint16_t next_frame_y = NEXT_Y_OFFSET - NEXT_LABEL_H - NEXT_PADDING*2;
	uint16_t next_frame_w = NEXT_AREA_W + NEXT_PADDING*2;
	uint16_t next_frame_h = NEXT_LABEL_H + NEXT_AREA_H + NEXT_PADDING*3;
	ILI9341_FillRectangle(next_frame_x, next_frame_y, next_frame_w, 1, COLOR_FRAME); // Top
	ILI9341_FillRectangle(next_frame_x, next_frame_y + next_frame_h - 1, next_frame_w, 1, COLOR_FRAME); // Bottom
	ILI9341_FillRectangle(next_frame_x, next_frame_y, 1, next_frame_h, COLOR_FRAME); // Left
	ILI9341_FillRectangle(next_frame_x + next_frame_w - 1, next_frame_y, 1, next_frame_h, COLOR_FRAME); // Right

	// Redraw Grid
	// Draw Vertical Lines
	for(int x = 1; x < BOARD_WIDTH; x++)
	{
		ILI9341_FillRectangle(BOARD_X_OFFSET + x * BLOCK_SIZE -1, BOARD_Y_OFFSET,
				1, BOARD_HEIGHT * BLOCK_SIZE, COLOR_GRID);
	}
	// Draw Horizontal Lines
	for(int y = 1; y < BOARD_HEIGHT; y++)
	{
		ILI9341_FillRectangle(BOARD_X_OFFSET, BOARD_Y_OFFSET + y * BLOCK_SIZE -1,
				BOARD_WIDTH * BLOCK_SIZE, 1, COLOR_GRID);
	}

	// Mark boardchange array fully to redraw existing blocks
	for (int y = 1; y <= BOARD_HEIGHT; y++)
	{
		for (int x = 1; x <= BOARD_WIDTH; x++)
		{
			boardchange[y][x] = 1;
		}
	}
	// Update static displays
	displayscore();
	printnext();
	return;
}
void displaylevel(void)
{

	char buffer[20];
	sprintf(buffer, "LEVEL %d START!", level);
	uint16_t msg_w = strlen(buffer) * Font_11x18.width;
	uint16_t msg_h = Font_11x18.height;
	uint16_t msg_x = (ILI9341_WIDTH - msg_w) / 2;
	uint16_t msg_y = (ILI9341_HEIGHT - msg_h) / 2;

		// Game Frame Coordinates
	uint16_t board_frame_x = BOARD_X_OFFSET - 1;
	uint16_t board_frame_y = BOARD_Y_OFFSET - 1;
	uint16_t board_frame_w = BOARD_WIDTH * BLOCK_SIZE + 2;
	uint16_t board_frame_h = BOARD_HEIGHT * BLOCK_SIZE + 2;

		// Display LEVEL text
	ILI9341_WriteString(msg_x, msg_y, buffer, Font_11x18, COLOR_INFO_TEXT, COLOR_BG);
	HAL_Delay(3000);

		// Clear LEVEL text area
	ILI9341_FillRectangle(msg_x, msg_y, msg_w, msg_h, COLOR_BG);

		// Redraw affected FRAME lines
    uint16_t msg_end_x = msg_x + msg_w;
    uint16_t msg_end_y = msg_y + msg_h;

    ILI9341_FillRectangle(board_frame_x, board_frame_y, board_frame_w, 1, COLOR_FRAME); // Top
    ILI9341_FillRectangle(board_frame_x, board_frame_y + board_frame_h - 1, board_frame_w, 1, COLOR_FRAME); // Bottom
    ILI9341_FillRectangle(board_frame_x, board_frame_y, 1, board_frame_h, COLOR_FRAME); // Left
    ILI9341_FillRectangle(board_frame_x + board_frame_w - 1, board_frame_y, 1, board_frame_h, COLOR_FRAME); // Right

	int start_y = (msg_y >= BOARD_Y_OFFSET) ? (msg_y - BOARD_Y_OFFSET) / BLOCK_SIZE + 1 : 1;
	int end_y   = (msg_end_y > BOARD_Y_OFFSET) ? (msg_end_y - BOARD_Y_OFFSET) / BLOCK_SIZE + 1 : 0;
	int start_x = (msg_x >= BOARD_X_OFFSET) ? (msg_x - BOARD_X_OFFSET) / BLOCK_SIZE + 1 : 1;
	int end_x   = (msg_end_x > BOARD_X_OFFSET) ? (msg_end_x - BOARD_X_OFFSET) / BLOCK_SIZE + 1 : 0;

    if (start_y < 1) start_y = 1;
    if (end_y > BOARD_HEIGHT) end_y = BOARD_HEIGHT;
	if (start_x < 1) start_x = 1;
	if (end_x > BOARD_WIDTH) end_x = BOARD_WIDTH;

	if (end_y >= start_y && end_x >= start_x) {
	    for (int y = start_y; y <= end_y; y++) {
	        for (int x = start_x; x <= end_x; x++) {
                if (y < BOARD_ARRAY_HEIGHT && x < BOARD_ARRAY_WIDTH) {
	                boardchange[y][x] = 1;
                }
	        }
	    }
	}
	displayscore();
	if (level == 4) {
		clearscreen();
		ILI9341_DrawImage((ILI9341_WIDTH - 240) / 2, (ILI9341_HEIGHT - 240) / 2, 240, 240, (const uint16_t*)level4_img);
		HAL_Delay(5000);
		clearscreen();
		redraw();
	}
	if (level == 6) {
		clearscreen();
		ILI9341_DrawImage((ILI9341_WIDTH - 240) / 2, (ILI9341_HEIGHT - 240) / 2, 240, 240, (const uint16_t*)level6_img);
		HAL_Delay(5000);
		clearscreen();
		redraw();
	}
	if (level == 8) {
		clearscreen();
		ILI9341_DrawImage((ILI9341_WIDTH - 240) / 2, (ILI9341_HEIGHT - 320) / 2, 240, 320, (const uint16_t*)level8_img);
		HAL_Delay(5000);
		clearscreen();
		redraw();
	}
}

void gameover(void){
	char *msg = "GAME OVER";
	uint16_t msg_w = strlen(msg) * Font_16x26.width;
	uint16_t msg_h = Font_16x26.height;
	uint16_t msg_x = (ILI9341_WIDTH - msg_w) / 2;
	uint16_t msg_y = (ILI9341_HEIGHT - msg_h) / 2;
		// GAME OVER
	ILI9341_WriteString(msg_x, msg_y, msg, Font_16x26, COLOR_GAMEOVER, COLOR_BG);

	HAL_Delay(3000);
	clearscreen();


	ILI9341_DrawImage((ILI9341_WIDTH - 240) / 2, (ILI9341_HEIGHT - 240) / 2, 240, 240, (const uint16_t*)gameover_img);
	HAL_Delay(5000);

	clearscreen();
}
int check(_Block *bp, int8_t x, int8_t y)
{
		// Check center block position
	if (board[y][x] != COLOR_SPACE) return -1;
		// Check relative block 1 position
	if (board[y + bp->y1][x + bp->x1] != COLOR_SPACE) return -1;
		// Check relative block 2 position
	if (board[y + bp->y2][x + bp->x2] != COLOR_SPACE) return -1;
		// Check relative block 3 position
	if (board[y + bp->y3][x + bp->x3] != COLOR_SPACE) return -1;
	return 0;
		// No collision
}
void putblock(void)
{
	_Block *bp = &falling;
	uint16_t color = bp->color;

	board[blocky][blockx] = color;
	board[blocky + bp->y1][blockx + bp->x1] = color;
	board[blocky + bp->y2][blockx + bp->x2] = color;
	board[blocky + bp->y3][blockx + bp->x3] = color;

	boardchange[blocky][blockx] = 1;
	boardchange[blocky + bp->y1][blockx + bp->x1] = 1;
	boardchange[blocky + bp->y2][blockx + bp->x2] = 1;
	boardchange[blocky + bp->y3][blockx + bp->x3] = 1;
}
void eraseblock(void)
{
    _Block *bp = &falling;
    // Set board locations to empty (COLOR_SPACE = 0)
    board[blocky][blockx] = COLOR_SPACE;
    board[blocky + bp->y1][blockx + bp->x1] = COLOR_SPACE;
    board[blocky + bp->y2][blockx + bp->x2] = COLOR_SPACE;
    board[blocky + bp->y3][blockx + bp->x3] = COLOR_SPACE;

    boardchange[blocky][blockx] = 1;
    boardchange[blocky + bp->y1][blockx + bp->x1] = 1;
    boardchange[blocky + bp->y2][blockx + bp->x2] = 1;
    boardchange[blocky + bp->y3][blockx + bp->x3] = 1;
}
int  newblock(void)
{
	const _Block *blockp = &block[next];
	falling.x1 = blockp->x1; falling.y1 = blockp->y1;
	falling.x2 = blockp->x2; falling.y2 = blockp->y2;
	falling.x3 = blockp->x3; falling.y3 = blockp->y3;
	falling.color = blockp->color;
	falling.rot = blockp->rot;

	blockx = BOARD_ARRAY_WIDTH / 2 - 1;
	blocky = 2;
	blockangle = 0;
	blockno = next;

	fallcount = fallspeed;

	next = rand() % 7;
	printnext();

	if (check(&falling, blockx, blocky) != 0)
	{
		return -1;
	}

	putblock();
	return 0;
}
void moveblock(void)
{
	_Block tempblock;
	const _Block *blockp;
		// Player Input
		// Rotate
	if (uart_up)
	{
		uart_up = 0;
		eraseblock();

		uint8_t next_angle = (blockangle + 1) % 4;
		if (falling.rot == 1 && next_angle > 1) next_angle = 0;
		if (falling.rot == 0) next_angle = 0;

		if (blockangle == falling.rot)
		{
			blockp = &block[blockno];
			tempblock = *blockp;
		} else {
			tempblock.x1 = -falling.y1; tempblock.y1 = falling.x1;
			tempblock.x2 = -falling.y2; tempblock.y2 = falling.x2;
			tempblock.x3 = -falling.y3; tempblock.y3 = falling.x3;
			tempblock.color = falling.color;
			tempblock.rot = falling.rot;
		}
			// Check rotate
		if (check(&tempblock, blockx, blocky) == 0)
		{
			falling = tempblock;
			blockangle = (blockangle == falling.rot) ? 0 : blockangle + 1;
		}
		putblock();
	}
		// Move Right
	else if (uart_right) {
		uart_right = 0;
		eraseblock();
		if (check(&falling, blockx + 1, blocky) == 0)
		{
			blockx++;
		}
		putblock();
	}
		// Move Left
	else if (uart_left) {
		uart_left = 0;
		eraseblock();
		if (check(&falling, blockx - 1, blocky) == 0)
		{
			blockx--;
		}
		putblock();
	}
		// Move Down
	bool down_command_active = uart_down || KEY_IS_DOWN_PRESSED;

	if (down_command_active) {
        if (uart_down) {
            uart_down = 0;
        }
		eraseblock();
		if (check(&falling, blockx, blocky + 1) == 0)
		{
			blocky++;
			score++;
			if (score > highscore) highscore = score;
		}
		putblock();
	}
		// Fall Logic
	fallcount--;
	if (fallcount == 0)
	{
		fallcount = fallspeed;
		eraseblock();
		if (check(&falling, blockx, blocky + 1) == 0)
		{
			blocky++;
			putblock();
		} else {
			putblock();
			gamestatus = 4;
			return;
		}
		putblock();
	}
}
void linecheck(void)
{
	uint8_t cleared_line_count = 0;
	int8_t y_check;
	int8_t first_cleared_y = -1;

		// Detect and Visually Mark Cleared Lines
	for (y_check = BOARD_HEIGHT; y_check >= 1; y_check--)
	{
		uint8_t blocks_in_row = 0;
		for (int8_t x = 1; x <= BOARD_WIDTH; x++)
		{
			if (board[y_check][x] != COLOR_SPACE)
			{
				blocks_in_row++;
			}
		}
		if (blocks_in_row == BOARD_WIDTH)
		{
			if (first_cleared_y == -1 || y_check < first_cleared_y)
			{
				first_cleared_y = y_check;
			}
			cleared_line_count++;
			uint16_t px = BOARD_X_OFFSET;
			uint16_t py = BOARD_Y_OFFSET + (y_check - 1) * BLOCK_SIZE;
			ILI9341_FillRectangle(px, py, BOARD_WIDTH * BLOCK_SIZE, BLOCK_SIZE, COLOR_CLEARBLOCK);
		}
	}
	if (cleared_line_count == 0)
	{
		gamestatus = 2;
		return;
	}

	uint16_t base_points = scorearray[cleared_line_count - 1];
	float multiplier = 1.0f + (float)(level - 1) * 0.5f;
	uint32_t points_to_add = (uint32_t)(base_points * multiplier);

		// Display Score Added
	if (first_cleared_y != -1)
	{
		char score_msg[10];
		sprintf(score_msg, "+%lu", points_to_add);
		uint16_t msg_w = strlen(score_msg) * Font_7x10.width;
		uint16_t msg_x = BOARD_X_OFFSET + (BOARD_WIDTH * BLOCK_SIZE - msg_w) / 2;
		uint16_t msg_y = BOARD_Y_OFFSET + (first_cleared_y - 1) * BLOCK_SIZE + (BLOCK_SIZE - Font_7x10.height)/2;

		ILI9341_WriteString(msg_x, msg_y, score_msg, Font_7x10, ILI9341_BLACK, COLOR_CLEARBLOCK);
	}


	HAL_Delay(500);


		// Shift Board Down
	int8_t row_to_write = BOARD_HEIGHT;
	for (y_check = BOARD_HEIGHT; y_check >= 1; y_check--)
	{
		uint8_t blocks_in_row = 0;

		for(int i=0; i < cleared_line_count; ++i)
		{
			for (int8_t x = 1; x <= BOARD_WIDTH; x++)
			{
				if (board[y_check][x] != COLOR_SPACE) { blocks_in_row++; }
			}
			if (blocks_in_row == BOARD_WIDTH)
			{
				break;
			}
		}
		blocks_in_row = 0;
		for (int8_t x = 1; x <= BOARD_WIDTH; x++)
		{
			if (board[y_check][x] != COLOR_SPACE) { blocks_in_row++; }
		}

		if (blocks_in_row < BOARD_WIDTH)
		{
			if (row_to_write != y_check)
			{
				for (int8_t x = 1; x <= BOARD_WIDTH; x++)
				{
					board[row_to_write][x] = board[y_check][x];
					boardchange[row_to_write][x] = 1;
				}
			}
			row_to_write--;
		}
	}
	    // Clear Top Rows
    while (row_to_write >= 1)
    {
        for (int8_t x = 1; x <= BOARD_WIDTH; x++)
        {
            board[row_to_write][x] = COLOR_SPACE;
            boardchange[row_to_write][x] = 1;
        }
        row_to_write--;
    }
		// Update Score, Lines, and Game State
	score += points_to_add;
	if (score > highscore)
	{
		highscore = score;
	}
	lines += cleared_line_count;
	total_lines += cleared_line_count;

	if (lines >= SCENECLEARLINE)
	{
		gamestatus = 1;
		lines = 0;
	} else {
		gamestatus = 2;
	}
	displayscore();
}
unsigned char startkeycheck(unsigned short n)
{
	while (n--)
	{
		HAL_Delay(16);
		if (uart_start) {
			uart_start = 0;
			return 1;
		}
	}
	return 0;
}
void title(void)
{
    clearscreen();
    char buffer[30];
    uint16_t y_pos = 30;
    uint16_t line_spacing_small = Font_7x10.height + 4;
    uint16_t temp_x;
    uint16_t temp_w;
    	// Title Text
    temp_w = strlen("TETRIS") * Font_16x26.width;
    temp_x = (ILI9341_WIDTH > temp_w) ? (ILI9341_WIDTH - temp_w) / 2 : 0;
    ILI9341_WriteString(temp_x, y_pos, "TETRIS", Font_16x26, ILI9341_YELLOW, COLOR_BG);
    y_pos += Font_16x26.height + 10;

    	// Info Text
    temp_w = strlen("STM32F429 / SPI LCD") * Font_7x10.width;
    temp_x = (ILI9341_WIDTH > temp_w) ? (ILI9341_WIDTH - temp_w) / 2 : 0;
    ILI9341_WriteString(temp_x, y_pos, "STM32F429 / SPI LCD", Font_7x10, ILI9341_CYAN, COLOR_BG);
    y_pos += line_spacing_small * 2;

    	// Display Scores
    sprintf(buffer, "LAST SCORE: %lu", score);
    ILI9341_WriteString(10, y_pos, buffer, Font_7x10, COLOR_SCORE_TEXT, COLOR_BG);
    y_pos += line_spacing_small;
    sprintf(buffer, "HIGH SCORE: %lu", highscore);
    ILI9341_WriteString(10, y_pos, buffer, Font_7x10, COLOR_SCORE_TEXT, COLOR_BG);
    y_pos += line_spacing_small * 2;

    	// Instructions
    ILI9341_WriteString(10, y_pos, "CONTROLS :", Font_7x10, COLOR_INFO_TEXT, COLOR_BG);
    y_pos += line_spacing_small;
    ILI9341_WriteString(10, y_pos, "'a': Left  'd': Right", Font_7x10, COLOR_INFO_TEXT, COLOR_BG);
    y_pos += line_spacing_small;
    ILI9341_WriteString(10, y_pos, "'s': Down  'w': Rotate", Font_7x10, COLOR_INFO_TEXT, COLOR_BG);
    y_pos += line_spacing_small;
    ILI9341_WriteString(10, y_pos, "'e': Start", Font_7x10, COLOR_INFO_TEXT, COLOR_BG);
    y_pos += line_spacing_small * 3;

    	// Prompt
    temp_w = strlen("Send 'e' to Start") * Font_7x10.width;
    temp_x = (ILI9341_WIDTH > temp_w) ? (ILI9341_WIDTH - temp_w) / 2 : 0;
    ILI9341_WriteString(temp_x, ILI9341_HEIGHT - line_spacing_small - 10, "Send 'e' to Start", Font_7x10, ILI9341_WHITE, COLOR_BG);

    while (1)
    {
    	if (startkeycheck(60)) return;
    }
}
void game(void)
{
	gameinit2();

	while (gamestatus != 6)
	{
		switch (gamestatus)
		{
		case 1: // Initialize New Level
			gameinit3();
			displaylevel();
			gamestatus = 2;
			break;

		case 2: // Spawn New Block
			if (newblock() != 0)
			{
				gamestatus = 5;
			} else {
				gamestatus = 3;
			}
			break;

		case 3: // Block Falling, Input Check
			// Game tick delay happens here
			HAL_Delay(16); // ~60Hz loop speed

			show();         // Redraw changed parts of the board
			displayscore(); // Update score display
			eraseblock();   // Erase block at current position
			moveblock();    // Check input, apply natural fall, check landing
			putblock();     // Draw block at new position

			break;

		case 4: // Block Landed, Check for Line Clears
			show();
			linecheck(); // Check lines, clear them, update score/lines
			break;
		case 5: // Game Over
			gameover();
			gamestatus = 6;
			break;
		default:
			gamestatus = 6;
			break;
		}
	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI5_Init();
  MX_USART1_UART_Init();
  MX_ADC3_Init();
  /* USER CODE BEGIN 2 */
  srand(HAL_GetTick());
  ILI9341_Init();
  ILI9341_FillScreen(COLOR_BG);
  HAL_UART_Receive_IT(&huart1, input_buffer, 1);

  gameinit();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  updateLEDs();
	  title();
	  game();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 360;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC3_Init(void)
{

  /* USER CODE BEGIN ADC3_Init 0 */

  /* USER CODE END ADC3_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC3_Init 1 */

  /* USER CODE END ADC3_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc3.Instance = ADC3;
  hadc3.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc3.Init.Resolution = ADC_RESOLUTION_12B;
  hadc3.Init.ScanConvMode = DISABLE;
  hadc3.Init.ContinuousConvMode = DISABLE;
  hadc3.Init.DiscontinuousConvMode = DISABLE;
  hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc3.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc3.Init.NbrOfConversion = 1;
  hadc3.Init.DMAContinuousRequests = DISABLE;
  hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_9;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC3_Init 2 */

  /* USER CODE END ADC3_Init 2 */

}

/**
  * @brief SPI5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI5_Init(void)
{

  /* USER CODE BEGIN SPI5_Init 0 */

  /* USER CODE END SPI5_Init 0 */

  /* USER CODE BEGIN SPI5_Init 1 */

  /* USER CODE END SPI5_Init 1 */
  /* SPI5 parameter configuration*/
  hspi5.Instance = SPI5;
  hspi5.Init.Mode = SPI_MODE_MASTER;
  hspi5.Init.Direction = SPI_DIRECTION_2LINES;
  hspi5.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi5.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi5.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi5.Init.NSS = SPI_NSS_SOFT;
  hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi5.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi5.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi5.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi5.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI5_Init 2 */

  /* USER CODE END SPI5_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, 0);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, RES_Pin|DC_Pin|LED4_Pin|LED3_Pin, 0);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, LED2_Pin|LED1_Pin, 0);

  /*Configure GPIO pin : CS_Pin */
  GPIO_InitStruct.Pin = CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BTN_Pin */
  GPIO_InitStruct.Pin = BTN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BTN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : RES_Pin DC_Pin */
  GPIO_InitStruct.Pin = RES_Pin|DC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : LED4_Pin LED3_Pin */
  GPIO_InitStruct.Pin = LED4_Pin|LED3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : LED2_Pin LED1_Pin */
  GPIO_InitStruct.Pin = LED2_Pin|LED1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pins : BTN_LEFT_Pin BTN_RIGHT_Pin BTN_DOWN_Pin BTN_START_Pin */
  GPIO_InitStruct.Pin = BTN_LEFT_Pin|BTN_RIGHT_Pin|BTN_DOWN_Pin|BTN_START_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pin : BTN_UP_Pin */
  GPIO_InitStruct.Pin = BTN_UP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(BTN_UP_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)
	{
		switch(input_buffer[0])
		{
			case 'w' : case 'W' : uart_up = 1; break;
			case 'a' : case 'A' : uart_left = 1; break;
			case 'd' : case 'D' : uart_right = 1; break;
			case 's' : case 'S' : uart_down = 1; break;
			case 'e' : case 'E' : uart_start = 1; break;
		}
		HAL_UART_Receive_IT(&huart1, input_buffer, 1);
	}
}

	void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
	{
		static uint32_t last_interrupt_time_up = 0;
		static uint32_t last_interrupt_time_left = 0;
		static uint32_t last_interrupt_time_right = 0;
		static uint32_t last_interrupt_time_down = 0;
		static uint32_t last_interrupt_time_start = 0;
		uint32_t current_time = HAL_GetTick();
		const uint32_t debounce_delay = 50;

		switch (GPIO_Pin)
		{
			case BTN_UP_Pin: // PC9
				if ((current_time - last_interrupt_time_up) > debounce_delay)
				{
					last_interrupt_time_up = current_time;
					uart_up = 1;
				}
				break;

			case BTN_LEFT_Pin: // PG5
				if ((current_time - last_interrupt_time_left) > debounce_delay)
				{
					last_interrupt_time_left = current_time;
					uart_left = 1;
				}
				break;

			case BTN_RIGHT_Pin: // PG6
				if ((current_time - last_interrupt_time_right) > debounce_delay)
				{
					last_interrupt_time_right = current_time;
					uart_right = 1;
				}
				break;

			case BTN_DOWN_Pin: // PG7
				if ((current_time - last_interrupt_time_down) > debounce_delay)
				{
					last_interrupt_time_down = current_time;
					uart_down = 1;
				}
				break;

			case BTN_START_Pin: // PG8
				if ((current_time - last_interrupt_time_start) > debounce_delay)
				{
					last_interrupt_time_start = current_time;

					if (gamestatus == 3 || gamestatus == 2) {
						uart_up = 1;
					} else {
						uart_start = 1;
					}
				}
				break;

			default:
				break;
		}
	}
	/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
