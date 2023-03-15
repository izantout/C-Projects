// CECS447Project2MCU2.c
// Starter file for CECS447 Project 2 MCU 2
// Min He
#include "tm4c123gh6pm.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "PLL.h"
#include "UART.h"
//#include "Nokia5110.h"
// color definitions 
#define R 0x02
#define B 0x04
#define G 0x08
#define P 0x06
#define W 0x0E
#define D 0x00
#define NVIC_EN0_PORTF 0x40000000

const uint8_t color_wheel[] = {G,R,B,P,W,D};
unsigned char COLOR;
unsigned char MENU_CHOICE;
unsigned int i;
unsigned int INTERRUPT;
// Defines what Bits control LED lights 
#define	LEDS       (*((volatile unsigned long *)0x40025038))
	
extern void EnableInterrupts(void);
extern void WaitForInterrupt(void);
extern void DisableInterrupts(void);
void PortF_Init();
void Mode2();
void Mode3();


int main(void){
    DisableInterrupts();
  PLL_Init();
    PortF_Init();
	UART_Init();
  UART1_Init();
    //Nokia5110_Init(); 
    EnableInterrupts();
    //Nokia5110_Clear();
  while(1){
		MENU_CHOICE = UART1_InChar();
		if(MENU_CHOICE == '2' || MENU_CHOICE == '3'){
			UART_OutString("MCU2 Recieved ");
			UART_OutChar(MENU_CHOICE); Enter();
		}
		switch(MENU_CHOICE){
				case '2': 
						Mode2();
						break;
				case '3': 
						Mode3();
						break;
				default: 
						break;
		}
  }
}


	void Mode2(void){
			UART_OutString("IN MODE 2 MCU2"); Enter();
			INTERRUPT = 2;
			COLOR = UART1_InChar();
			LEDS = COLOR;

	}

    void Mode3(){
        int r = 0;

    }


// PORTF INIT
void PortF_Init(void)
{
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;              // (a) Activate clock for port F
	while ((SYSCTL_RCGC2_R &= SYSCTL_RCGC2_GPIOF)==0){};
		
  GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;                 // 2) Unlock PortF PF0  
  GPIO_PORTF_CR_R |= 0x1F;                           // Allow changes to PF4-0       
  GPIO_PORTF_DIR_R |= 0x0E;                          // (c) Make LEDs outputs
  GPIO_PORTF_DIR_R &= ~0x11;                         // Make Buttons inputs
  GPIO_PORTF_AFSEL_R &= ~0x1F;                       // Disable alt funct 
  GPIO_PORTF_DEN_R |= 0x1F;                          // Enable digital   
  GPIO_PORTF_PCTL_R &= ~0x000FFFFF;                  // Configure as GPIO
  GPIO_PORTF_AMSEL_R &= ~0x1F;                       // Disable analog functionality on PF
  GPIO_PORTF_PUR_R |= 0x11;                          // Enable weak pull-up on PF0,4
		
  GPIO_PORTF_IS_R &= ~0x11;                          // (d) PF0,4 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x11;                         // PF0,4 is not both edges
  GPIO_PORTF_IEV_R |= 0x11;                          // PF4 rising edge event
  GPIO_PORTF_ICR_R = 0x11;                           // (e) Clear flag4
  GPIO_PORTF_IM_R |= 0x11;                           // (f) Arm interrupt on PF4
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF0FFFFF)|0x00C00000; // (g) Priority 6
  NVIC_EN0_R |= NVIC_EN0_PORTF;                      // (h) Enable interrupt 30 in NVIC
}


// PORTF HANDLER
void GPIOPortF_Handler(void){	
	// If Switch 1 is pressed
	if (GPIO_PORTF_RIS_R & 0x10){
		UART_OutString("SW1 Pressed"); Enter();
		// If the interrupt flag for mode 2 is triggered
		if(INTERRUPT == 2){
			switch (LEDS) {
				case D:
					LEDS = R;
					break;
				case R:
					LEDS = G;
					break;
				case B:
					LEDS = P;
					break;
				case G:
					LEDS = B;
					break;
				case P:
					LEDS = W;
					break;
				case W:
					LEDS = D;
					break;
				default:
					break;
			}
		// Clear Switch 1 Interrupt status
		GPIO_PORTF_ICR_R = 0x10;
		}
	}
	
	// If switch 2 is pressed
	if(GPIO_PORTF_RIS_R & 0x01){
		UART_OutString("SW2 Pressed "); Enter();
		
		// If the interrupt flag for mode 2 is triggered
		if(INTERRUPT == 2) {
			UART_OutString("Interrupt is 2"); Enter();
			// Send '2' to make sure you dont send nothing
			UART1_OutChar('2');
			UART_OutString("2 is sent"); Enter();
			// Send LED value to MCU1
			UART1_OutChar(LEDS);
			UART_OutString("LED is sent"); Enter();
			UART1_OutChar(CR);
			// Reset Mode 2 Interrupt flag
			 INTERRUPT = 0;
		}
	}
	// Clear Switch 2 Interrupt status
	GPIO_PORTF_ICR_R = 0x01;
}
