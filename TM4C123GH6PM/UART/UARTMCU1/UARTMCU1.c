// UARTTestMain.c
// Runs on LM4F120/TM4C123
// Used to test the UART.c driver
// Daniel Valvano
// September 12, 2013

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2013

 Copyright 2013 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// U0Rx (VCP receive) connected to PA0
// U0Tx (VCP transmit) connected to PA1

#include "PLL.h"
#include "UART.h"
#include "tm4c123gh6pm.h"
#include <string.h>
#include <stdio.h>

#define	LEDS       (*((volatile unsigned long *)0x40025038))
#define R          0x02
#define G          0x08
#define B          0x04
#define P          0x06
#define W          0x0E
#define D          0x00
#define NVIC_EN0_PORTF 0x40000000

char MENU_CHOICE;
char LED_CHOICE;
unsigned char RECIEVED;
char test;
unsigned int i;
unsigned int INTERRUPT;
char string[255];
char string2[255] = "";
char ack[255] = "I received: ";

void PortF_Init();
void PortE_Init();
extern void DisableInterrupts(void);
extern void EnableInterrupts(void);  

int main(void){
	unsigned char COLOR;
	DisableInterrupts();
	PortF_Init();             // Initialize PortF
	PortE_Init();
  PLL_Init();               // Initialize PLL
  UART_Init();              // Initialize UART
	UART1_Init();
	EnableInterrupts();
  while(1){
		
		// Welcome Prompt
		UART_OutString("Welcome to CECS 447 Project 2 - UART"); Enter();
		UART_OutString("		1. PC   <-> MCU1"); Enter();
		UART_OutString("		2. MCU1 <-> MCU2 LED Control"); Enter();
		UART_OutString("		3. PC   <-> MCU1 <-> MCU2 Messenger"); Enter();
		UART_OutString("Please choose a communication mode(type 1 or 2 or 3):"); Enter(); Enter();
		
		// Menu choice is the user input
		MENU_CHOICE = UART_InChar();
		
		// Output Menu choice
		UART_OutString("You chose Mode ");
		UART_OutChar(MENU_CHOICE); Enter(); Enter();
		
		// If the user chose Mode 1
		if (MENU_CHOICE == '1'){
			
			// Output the LED Menu
			UART_OutString(" r for Red"); Enter();
			UART_OutString(" b for Blue"); Enter();
			UART_OutString(" g for Green"); Enter();
			UART_OutString(" p for Purple"); Enter();
			UART_OutString(" w for White"); Enter();
			UART_OutString(" d for Dark"); Enter(); Enter();
			
			// LED choice is the user input
			LED_CHOICE = UART_InChar();
			
			// Output the user input
			if (LED_CHOICE == 'r' || LED_CHOICE == 'b' || LED_CHOICE == 'g' || LED_CHOICE == 'p' || LED_CHOICE == 'w' || LED_CHOICE == 'd'){
				UART_OutString("You chose: ");
				UART_OutChar(LED_CHOICE); Enter(); Enter();
			} else {
				UART_OutString("Your choice: '");
				UART_OutChar(LED_CHOICE);
				UART_OutString("' is a wrong choice please try again"); Enter(); Enter();
			}
			
			
			// Check the LED color choice
			
			if(LED_CHOICE == 'r') {
				LEDS = R;
				UART_OutString("Red LED is on "); Enter();
				Enter();
			}
			else if(LED_CHOICE == 'b') {
				LEDS = B;
				UART_OutString("Blue LED is on "); Enter();
				Enter();
			}
			else if(LED_CHOICE == 'g') {
				LEDS = G;
				UART_OutString("Green LED is on "); Enter();
				Enter();
			}
			else if(LED_CHOICE == 'p') {
				LEDS = P;
				UART_OutString("Purple LED is on "); Enter();
				Enter();
			}
			else if(LED_CHOICE == 'w') {
				LEDS = W;
				UART_OutString("White LED is on "); Enter();
				Enter();
			}
			else if(LED_CHOICE == 'd') {
				LEDS = D;
				UART_OutString("LED is off "); Enter();
				Enter();
			}
		}
		
		// If mode 2 is chosen
		if(MENU_CHOICE == '2') {
			test = ' ';
			// Trigger the mode 2 interrupt flag
			INTERRUPT = 2;
			UART_OutString("Interrupt set to 2 "); Enter();
			// set LED color to Dark as the first color
			LEDS = D;
			UART_OutString("LEDS set to dark "); Enter();
			RECIEVED = UART1_InChar();
			while(RECIEVED != '2'){
				RECIEVED = UART1_InChar();
			}	
			UART_OutString("I recieved: '");
			UART_OutChar(RECIEVED);
			UART_OutString("' "); Enter();
			
			while(test != 'Q'){		
				if(RECIEVED == '2'){
					UART_OutString("BEFORE UART1 Recieved the color");
					COLOR = UART1_InChar();
					UART_OutString("UART1 Recieved the color");
					LEDS = COLOR;
					UART_OutString("MCU 2 SENT BACK A COLOR ");  
					test = 'Q';
				}			
			}
}
		
		// If mode 3 is chosen
		if(MENU_CHOICE == '3'){
			LEDS = G; // starts with Green LED when in mode 3
			UART_OutString("Enter a message: "); // prompts user to enter string from PC
			Enter();
			UART_InString(string, 254); // receives message from PC and stores it in string in MCU1
			Enter();
			UART1_OutChar('3');
			UART1_OutString(string); //sends the string to MCU2
			Enter1();
			UART_OutString("Sent string to MCU2"); Enter();
			UART1_InString(string,254); //gets "I RECIEVED" string from MCU2
			UART_OutString(string); Enter(); Enter(); //outputs RECIEVE nessage to PC
			LEDS = D;											//sets LED to dark
			
		}
	}
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

void PortE_Init(){
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x10;           // 1) E clock
	delay = SYSCTL_RCGC2_R;           // delay
	GPIO_PORTE_AMSEL_R &= ~0x0F;        // 3) disable analog function
	GPIO_PORTE_PCTL_R &= ~0x0000FFFF;   // 4) GPIO clear bit PCTL
	GPIO_PORTE_DIR_R &= ~0x0F;          // 5) PE3-0  input
	GPIO_PORTE_AFSEL_R &= ~0x0F;        // 6) no alternate function
	GPIO_PORTE_DEN_R |= 0x0F;          // 7) enable digital pins PE3-0
	GPIO_PORTE_IS_R &= ~0x0F;
	GPIO_PORTE_PUR_R = 0x11;          // enable pull-up on PF0 and PF4
	GPIO_PORTE_IBE_R &= ~0x0F;
	GPIO_PORTE_IEV_R &= ~0x0F;
	GPIO_PORTE_ICR_R = 0x0F;
	GPIO_PORTE_IM_R |= 0x0F;
	//NVIC_PRI7_R = (NVIC_PRI7_R&0xFFFF1FFF)|0x00040000; // (g) 
	//NVIC_EN0_R = 0x40000000;      // (h) enable interrupt 30 in NVIC
    }


// PORTF HANDLER
void GPIOPortF_Handler(void)
{	
	for(i = 0; i <= 200000; i++){}
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
			
			
			// Send flag value to MCU2
			 UART1_OutChar('2'); 
			 UART_OutString("Sent 2 to mcu 2 "); Enter();
			
			
			// Send LED value to MCU2
			 UART1_OutChar(LEDS);	
			 UART_OutString("Sent led to mcu 2 "); Enter();
			// Give UART1 CR command
			 UART1_OutChar(CR);
			
			// Reset Mode 2 Interrupt flag
			 INTERRUPT = 0;
		}
	}
	// Clear Switch 2 Interrupt status
	GPIO_PORTF_ICR_R = 0x01;
}


//PORTE HANDLER
void GPIOPortE_Handler(void){
	if(GPIO_PORTE_RIS_R & 0x01) {
		GPIO_PORTE_ICR_R = 0x01;
    UART_OutChar('m');
	}
}
