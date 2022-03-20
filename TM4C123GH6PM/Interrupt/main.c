// EdgeInterrupt.c
// Based on TExaSware\C12_EdgeInterrupt

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers"
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2013
   Volume 1, Program 9.4
   
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2013
   Volume 2, Program 5.6, Section 5.5

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

#include <stdint.h> // C99 data types
#include "tm4c123gh6pm.h"

// Function Prototypes (from startup.s)
void DisableInterrupts(); // Disable interrupts
void EnableInterrupts();  // Enable interrupts
void WaitForInterrupt();  // Go to low power mode while waiting for the next interrupt

// Function Prototypes
void EdgeCounter_Init();  // Initialize edge trigger interrupt for PF0 (SW2) rising edge
void PortF_LEDInit();     // Initialize Port F LEDs
void SysTick_Init();      // Initialize SysTick timer for 0.1s delay with interrupt enabled

void GPIOPortF_Handler(); // Handle GPIO Port F interrupts
void SysTick_Handler();   // Handle SysTick generated interrupts

// global variable visible in Watch and Memory window of debugger
// increments at least once per button release
volatile uint32_t RisingEdges = 0;

int main(void){
  PortF_LEDInit();
	EdgeCounter_Init();           // initialize GPIO Port F interrupt
	SysTick_Init();
	
	// initialize LaunchPad LEDs to red
	GPIO_PORTF_DATA_R = (GPIO_PORTF_DATA_R & ~0x0E) + 0x02;
	
  while(1){
		WaitForInterrupt();
  }
}

// Color    LED(s) PortF
// red      R--    0x02
// blue     --B    0x04
// green    -G-    0x08

// Initialize Port F LEDs
void PortF_LEDInit() {
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) F clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0       
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R = 0x0A;          // 5) PF4,PF0 input, PF3,PF2,PF1 output   
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTF_PUR_R = 0x11;          // enable pullup resistors on PF4,PF0       
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital pins PF4-PF0 
}

// Initialize SysTick timer for 0.1s delay with interrupt enabled
void SysTick_Init() {
  NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_ST_RELOAD_R = NVIC_ST_RELOAD_M;  // maximum reload value
  NVIC_ST_CURRENT_R = 0;                // any write to current clears it
                                        // enable SysTick with core clock
  NVIC_ST_CTRL_R = NVIC_ST_CTRL_ENABLE+NVIC_ST_CTRL_CLK_SRC;
}

// Initialize edge trigger interrupt for PF0 (SW2) rising edge
void EdgeCounter_Init() {                          

}

// Handle GPIO Port F interrupts. When Port F interrupt triggers, do what's necessary then increment global variable RisingEdges
void GPIOPortF_Handler() {

}

// Handle SysTick generated interrupts. When timer interrupt triggers, do what's necessary then toggle red and blue LEDs at the same time
void SysTick_Handler() {

}
