// Walk_Man.c
// Runs on TM4C123
// Uses ST7735.c LCD.
// Jonathan Valvano
// August 5, 2015
// Possible main program to test the lab
// Feel free to edit this to match your specifications

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected 
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO)
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

#include <stdio.h>
#include <stdint.h>
#include "string.h"
#include "ST7735.h"
#include "PLL.h"
#include "walking.h"
#include "tm4c123gh6pm.h"

void DelayWait10ms(uint32_t n);
void DrawBackground();
void DrawText();

int main(void){
	uint32_t x = 64, ht = 0;	
	int32_t l = 35, h = 10;
	
//  PLL_Init(12);
  PLL_Init(Bus80MHz);
	
  ST7735_InitR(INITR_REDTAB);
	DrawBackground();
		
	
	// 128 by 160
	//  x      y
  while(1){
		ST7735_DrawCircle(x, l, h, ST7735_BLACK);
		DelayWait10ms(10);
		ST7735_DrawCircle( x, l, h, ST7735_GREEN);
		// MOVE THE COORDINATES OF THE MAN
		l++;
	 
		//Reset parameters
		if(l>170){
			x = 64, ht = 0,l = 35, h = 10;
			DrawText();
			DelayWait10ms(600);
			DrawBackground();
		}
	}
} 

void DrawText(){
	char GOAL[6] = "GOAL!";
	ST7735_DrawChar(10,60,GOAL[0], ST7735_RED, ST7735_GREEN, 1);
	ST7735_DrawChar(20,60,GOAL[1], ST7735_YELLOW, ST7735_GREEN, 2);
	ST7735_DrawChar(35,60,GOAL[2], ST7735_BLACK, ST7735_GREEN, 3);
	ST7735_DrawChar(55,60,GOAL[3], ST7735_BLUE, ST7735_GREEN, 3);
	ST7735_DrawChar(65,60,GOAL[4], ST7735_RED, ST7735_GREEN, 4);
}

void DrawBackground(){
	// DRAW THE GROUND
	ST7735_FillScreen(ST7735_GREEN);
	  
	// DRAW THE MIDDLE CIRCLE
	ST7735_FillCircle(60, 80, 30, ST7735_CYAN);
	
	// DRAW THE MIDDLE LINE
	ST7735_DrawLine(0, 80, 128, 80, ST7735_WHITE);
	
	// DRAW THE TOP RECTANGLE
	ST7735_DrawLine(40, 20, 80, 20, ST7735_WHITE);
	ST7735_DrawLine(40, 0, 40, 20, ST7735_WHITE);
	ST7735_DrawLine(80, 0, 80, 20, ST7735_WHITE);

	// DRAW THE BOTTOM RECTANGLE
	ST7735_DrawLine(40, 140, 80, 140, ST7735_WHITE);
	ST7735_DrawLine(40, 160, 40, 140, ST7735_WHITE);
	ST7735_DrawLine(80, 160, 80, 140, ST7735_WHITE);
	
	// DRAW THE 2ND TOP RECTANGLE
	ST7735_DrawLine(50, 0, 50, 10, ST7735_WHITE);
	ST7735_DrawLine(50, 10, 70, 10, ST7735_WHITE);
	ST7735_DrawLine(70, 0, 70, 10, ST7735_WHITE);

	// DRAW THE 2ND BOTTOM RECTANGLE
	ST7735_DrawLine(50, 150, 70, 150, ST7735_WHITE);
	ST7735_DrawLine(50, 160, 50, 150, ST7735_WHITE);
	ST7735_DrawLine(70, 160, 70, 150, ST7735_WHITE);
	
	// DRAW A PERSON
	// HEAD
	ST7735_DrawCircle(80, 40, 4, ST7735_BLACK);
	// BODY
	ST7735_DrawLine(80, 45, 80, 55, ST7735_BLACK);
	// ARMS LEFT
	ST7735_DrawLine(80, 47, 77, 50, ST7735_BLACK);
	// ARMS RIGHT
	ST7735_DrawLine(80, 47, 83, 50, ST7735_BLACK);
	// LEG LEFT
	ST7735_DrawLine(80, 55, 77, 60, ST7735_BLACK);
	// LEG RIGHT
	ST7735_DrawLine(80, 55, 83, 60, ST7735_BLACK);

}

// Subroutine to wait 10 msec
// Inputs: None
// Outputs: None
// Notes: ...
void DelayWait10ms(uint32_t n){uint32_t volatile time;
  while(n){
//    time = 727240*2/91;  // 10msec for launchpad
    time = 727240/91;  // for simulation
    while(time){
	  	time--;
    }
    n--;
  }
}
