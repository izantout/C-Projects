// ----- 1. Include Section -----
#include "tm4c123gh6pm.h"
#include <stdint.h>
#include "ButtonLed.h"
#include "Sound.h"

// ----- 2. Constant Decleration
#define SW1 0x10  // Left Switch 
#define SW2 0x01  // Right Switch
#define NVIC_EN0_PORTF 0x40000000  // PORTF Interrupt
#define NVIC_EN0_PORTD 0x00000008  // PORTD Interrupt

// ----- 3. Variable Decleration -----
volatile uint8_t curr_mode=PIANO;
uint8_t noteC = 0, noteD = 0, noteE = 0, noteF = 0;

// ************** ButtonLed_Init *********************
void ButtonLed_Init(void){ volatile unsigned long  delay;
  SYSCTL_RCGC2_R |= 0x00000020;                       // 1) activate clock for Port F
  delay = SYSCTL_RCGC2_R;                             // allow time for clock to start
  GPIO_PORTF_LOCK_R = 0x4C4F434B;                     // 2) unlock GPIO Port F
  GPIO_PORTF_CR_R = 0x1F;                             // allow changes to PF4-0
                                                      // only PF0 needs to be unlocked, other bits can't be locked
  GPIO_PORTF_AMSEL_R &= ~0x1F;                        // 3) disable analog on PF
  GPIO_PORTF_PCTL_R &= ~0x000FFFFF;                   // 4) PCTL GPIO on PF4-0
  GPIO_PORTF_DIR_R |= 0x0E;                           // 5) PF4,PF0 in, PF3-1 out
  GPIO_PORTF_DIR_R &= ~0x11;                          // 5) PF4,PF0 in, PF3-1 out
  GPIO_PORTF_AFSEL_R &= ~0x1F;                        // 6) disable alt funct on PF7-0
  GPIO_PORTF_PUR_R |= 0x11;                           // enable pull-up on PF0 and PF4
  GPIO_PORTF_DEN_R |= 0x1F;                           // 7) enable digital I/O on PF4-0
	GPIO_PORTF_IS_R &= ~0x11;     		                  //  PF4,PF0 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x11;    		                  //  PF4,PF0 is not both edges
  GPIO_PORTF_IEV_R &= ~0x11;    		                  //  PF4,PF0 falling edge event
  GPIO_PORTF_ICR_R = 0x11;      		                  //  Clear flags 4,0
  GPIO_PORTF_IM_R |= 0x11;      		                  //  Arm interrupt on PF4,PF0
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF1FFFFF)|0x00A00000;	// priority 5
  NVIC_EN0_R |= NVIC_EN0_PORTF;      									//  Enable interrupt 30 in NVIC
}

// ************** PianoKeys_Init *********************
void PianoKeys_Init(void){ volatile unsigned long  delay;
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOD;               // 1) activate clock for Port D
  delay = SYSCTL_RCGC2_R;                             // allow time for clock to start
  GPIO_PORTD_CR_R = 0x0F;                             // allow changes to PD0-3
  GPIO_PORTD_AMSEL_R &= ~0x0F;                        // 3) disable analog on PD
  GPIO_PORTD_PCTL_R &= ~0x0000FFFF;                   // 4) PCTL GPIO on PF4-0
  GPIO_PORTD_DIR_R &= ~0x0F;                          // 5) PD in
  GPIO_PORTD_AFSEL_R &= ~0x0F;                        // 6) disable alt funct on PD
  GPIO_PORTD_PUR_R |= 0x0F;                           // enable pull-up on PD0-3
  GPIO_PORTD_DEN_R |= 0x0F;                           // 7) enable digital I/O on PD
	
	GPIO_PORTD_IS_R &= ~0x0F;     				              //  PF4,PF0 is edge-sensitive
  GPIO_PORTD_IBE_R |= 0x0F;    				                //  PF4,PF0 is not both edges
  GPIO_PORTD_ICR_R = 0x0F;      				              //  Clear flags 4,0
  GPIO_PORTD_IM_R |= 0x0F;      				              //  Arm interrupt on PF4,PF0
	
  NVIC_PRI0_R = (NVIC_PRI0_R&0x1FFFFFFF)|0xA0000000;	
  NVIC_EN0_R |= NVIC_EN0_PORTD;      									//  Enable interrupt 30 in NVIC
}

// ************** get_current_mode *********************
uint8_t get_current_mode(void){ // Gets the current mode we are in 
	return curr_mode;
}

// ************** GPIOPortF_Handler *********************
void GPIOPortF_Handler(void){
	Delay();
  if(GPIO_PORTF_RIS_R&0x01){  // If SW2 is pressed
		if(in_piano_mode()){      // If we are in piano mode
			next_octave();          // Go to next octave
		}
		else{                     // Else
			next_song();            // Go to next song
		}
    GPIO_PORTF_ICR_R = 0x01;  // Acknowledge flag 1
  }
  if(GPIO_PORTF_RIS_R&0x10){  // If SW1 is pressed
		piano_mode_toggle();      // Change Piano mode 
    GPIO_PORTF_ICR_R = 0x10;  // Acknowledge flag 4
  }
}

// ************** noteC_playing *********************
uint8_t noteC_playing(void){ // Checks if note C is playing
	return noteC;
}

// ************** noteD_playing *********************
uint8_t noteD_playing(void){ // Checks if note D is playing
	return noteD;
}

// ************** noteE_playing *********************
uint8_t noteE_playing(void){ // Checks if note E is playing
	return noteE;
}

// ************** noteF_playing *********************
uint8_t noteF_playing(void){ // Checks if note F is playing
	return noteF;
}

// ************** GPIOPortD_Handler *********************
void GPIOPortD_Handler(void){
	Delay();
	if(in_piano_mode()){
		if(GPIO_PORTD_RIS_R&0x01){	// If PD0 / Note C is pressed
			noteC ^= 0x01;
			GPIO_PORTD_ICR_R = 0x01;	// Acknowledge flag 0
		}
		if(GPIO_PORTD_RIS_R&0x02){	// PD1 / Note D is pressed
			noteD ^= 0x01;
			GPIO_PORTD_ICR_R = 0x02;	// Acknowledge flag 1
		}
		if(GPIO_PORTD_RIS_R&0x04){	// PD2 / Note E is pressed
			noteE ^= 0x01;
			GPIO_PORTD_ICR_R = 0x04;	// Acknowledge flag 2
		}
		if(GPIO_PORTD_RIS_R&0x08){	// PD3 / Note F is pressed
			noteF ^= 0x01;
			GPIO_PORTD_ICR_R = 0x08;	// Acknowledge flag 3
		}
	}
	else{
		GPIO_PORTD_ICR_R = 0x0F;	// Acknowledge all flags
	}
}
