// ***** 0. Documentation Section *****
// StepperMotor.c for Lab 6
// Runs on TM4C123GH6PM
// Making a Stepper Motor turn with user inputs  
// Issam Zantout

// ***** 1. Include Files  *****

#include <stdint.h>                       // C99 data types
#include "tm4c123gh6pm.h"

// ***** 2. Function Prototypes *****
void MotorMover(void);
void portInit(void);
void DisableInterrupts(void);             // Disable interrupts
void EnableInterrupts(void);              // Enable interrupts

// ***** 3. Global Variable Declerations *****
#define MotorMove  (*((volatile unsigned long *)0x4002403C))// Accesses  PE3(IN1 which is Blue) - PE2(IN2 which is Pink)- PE1(IN3 which is Yellow)- PE0(IN4 which is Orange)
#define BUTTON1    (*((volatile unsigned long *)0x40025004))// Accesses  PF0(Button1)
#define BUTTON2    (*((volatile unsigned long *)0x40025040))// Accesses  PF4(Button2)
int i;
	
// ***** 4. main function  *****
int main()
	{
		portInit();
	}
	
// ***** 5. Function Definition  *****
void portInit(void)
	{
		volatile unsigned long delay;
		
		// PORT E Initialization
		SYSCTL_RCGC2_R |= 0x0000010; 		        // 1) PORTE clock
		delay = SYSCTL_RCGC2_R;                 // delay    
		GPIO_PORTE_DIR_R = 0x0E;                // 5) PE3-PE0 are outputs, give them a 1
		GPIO_PORTE_DEN_R = 0x0E;                // 8) enable digital pins PE3-PE0
		
		// PORT F Initialization		
		SYSCTL_RCGC2_R |= 0x00000020;           // 1) PORTF clock
		delay = SYSCTL_RCGC2_R;                 // delay   
		GPIO_PORTF_LOCK_R = 0x4C4F434B;         // 2) unlock PortF PF0  
		GPIO_PORTF_CR_R = 0x1F;                 // allow changes to PF4-0       
		GPIO_PORTF_AMSEL_R = 0x00;              // 3) disable analog function
		GPIO_PORTF_PCTL_R = 0x00000000;         // 4) GPIO clear bit PCTL  
		GPIO_PORTF_DIR_R = 0x00;                // 5) PF4 & PF0 Inputs   
		GPIO_PORTF_AFSEL_R = 0x00;              // 6) no alternate function
		GPIO_PORTF_PUR_R = 0x1E;                // enable pullup resistors on PF4-PF0       
		GPIO_PORTF_DEN_R = 0x1E;                // 7) enable digital pins PF4-PF0 
	}
	
void MotorMover(void)
	{
		/*
			Blue   is IN1 which is E3
			Pink   is IN2 which is E2
			Yellow is IN3 which is E1
			Orange is IN4 which is E0
		*/
		
		// Blue
		// 1000
		MotorMove = 0x08;
		
		// If button PF0 is pressed: if PF0 & 0xFF == 1
		if(BUTTON1&&0xFF==1)
			{
				for(i=0; i<2; i++) // 11.25 * 8 = 90 this is why its a for loop 2 iterations
					{
						// Pink
						// 0100
						MotorMove = 0x04;
						
						// Yellow
						// 0010
						MotorMove = 0x02;
						
						// Orange
						// 0001
						MotorMove = 0x01;
						
						// Blue
						// 1000
						MotorMove = 0x08;
						
					}
			}
					
		// If button PF4 is pressed
		if(BUTTON2&&0xFF==1)
			{
				for(i=0; i<4; i++) // 11.25 * 16 = 180 this is why its a for loop 4 iterations
					{
						// Orange
						// 0001
						MotorMove = 0x01;
						
						// Yellow
						// 0010
						MotorMove = 0x02;
						
						// Pink
						// 0100
						MotorMove = 0x04;
						
						// Blue
						// 1000
						MotorMove = 0x08;
						
					}
			}
	}

