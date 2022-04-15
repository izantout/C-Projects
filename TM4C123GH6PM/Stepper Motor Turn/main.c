// ***** 0. Documentation Section *****
// StepperMotor.c for Lab 6
// Runs on TM4C123GH6PM
// Making a Stepper Motor turn with user inputs  
// Issam Zantout

// ***** 1. Include Files  *****

#include <stdint.h>                       // C99 data types
#include "tm4c123gh6pm.h"
#include "SysTick.h"

// ***** 2. Function Prototypes *****
void MotorMover(void);
void portInit(void);
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // Go to low power mode while waiting for the next interrupt
void EdgeCounter_Init(void);  // Initialize edge trigger interrupt for PF0 (SW2) rising edge
void SysTick_Init(void);      // Initialize SysTick timer for 0.1s delay with interrupt enabled
void SysTick_Wait1ms(unsigned long delay);
void SysTick_Wait(unsigned long delay);
void GPIOPortF_Handler(void); // Handle GPIO Port F interrupts
void SysTick_Handler(void);   // Handle SysTick generated interrupts

// ***** 3. Global Variable Declerations *****
#define MotorMove  (*((volatile unsigned long *)0x4002403C))// Accesses  PE3(IN1 which is Blue) - PE2(IN2 which is Pink)- PE1(IN3 which is Yellow)- PE0(IN4 which is Orange)
#define BUTTON1    (*((volatile unsigned long *)0x40025004))// Accesses  PF0(Button1)SWITCH2
#define BUTTON2    (*((volatile unsigned long *)0x40025040))// Accesses  PF4(Button2)SWITCH1

volatile uint32_t RisingEdges = 0;
volatile uint32_t count = 0;
	
// ***** 4. main function  *****
int main()
	{
		portInit();
		EdgeCounter_Init();
		SysTick_Init();
		while(1)
			{
				WaitForInterrupt();
			}
	}
	
// ***** 5. Function Definition  *****
void portInit(void)
	{
		volatile unsigned long delay;
		
		// PORT E Initialization
		SYSCTL_RCGC2_R |= 0x0000010; 		        // 1) PORTE clock
		delay = SYSCTL_RCGC2_R;                 // delay    
		GPIO_PORTE_DIR_R = 0x0F;                // 5) PE3-PE0 are outputs, give them a 1
		GPIO_PORTE_DEN_R = 0x0F;                // 8) enable digital pins PE3-PE0
		
		// PORT F Initialization		
		SYSCTL_RCGC2_R |= 0x00000020;           // 1) PORTF clock
		delay = SYSCTL_RCGC2_R;                 // delay   
		GPIO_PORTF_LOCK_R = 0x4C4F434B;         // 2) unlock PortF PF0  
		GPIO_PORTF_CR_R = 0x1F;                 // allow changes to PF4-0       
		GPIO_PORTF_AMSEL_R = 0x00;              // 3) disable analog function
		GPIO_PORTF_PCTL_R = 0x00000000;         // 4) GPIO clear bit PCTL  
		GPIO_PORTF_DIR_R = 0x00;                // 5) PF4 & PF0 Inputs   
		GPIO_PORTF_AFSEL_R = 0x00;              // 6) no alternate function
		GPIO_PORTF_PUR_R = 0x1F;                // enable pullup resistors on PF4-PF0       
		GPIO_PORTF_DEN_R = 0x1F;                // 7) enable digital pins PF4-PF0 
	}
	
void MotorMover(void)
	{
		/*
			Blue   is IN1 which is E3
			Pink   is IN2 which is E2
			Yellow is IN3 which is E1
			Orange is IN4 which is E0
		*/
	}

// Initialize edge trigger interrupt for PF0 (SW2) rising edge
void EdgeCounter_Init(void) {
	RisingEdges = 0;
	GPIO_PORTF_IS_R &= ~0x11;  // PF0 is edge sensitive
	GPIO_PORTF_IBE_R &= ~0x11; // PF0 is not both edges
	GPIO_PORTF_IEV_R &= ~0x11;  // PF0 is on rising edge
	GPIO_PORTF_ICR_R = 0x11;   // Clearing flag0
	GPIO_PORTF_IM_R |= 0x11;   // Arming Interrupt on PF0
	NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF) | 0x00A00000;
	NVIC_EN0_R = 0x40000000;   // Enables interrupt 30 in NVIC
	EnableInterrupts();        // Clears the I bit

}

// Handle GPIO Port F interrupts. When Port F interrupt triggers, do what's necessary then increment global variable RisingEdges
void GPIOPortF_Handler(void) {
	int i=0;
	GPIO_PORTF_ICR_R = 0x11;    // Acknowledge flag0
		// Blue
		// 1000
		MotorMove = 0x08;
		
		// If button PF0 is pressed: if PF0 & 0xFF == 1
		if(BUTTON1==0)
			{
				for(i=0; i<128; i++)
					{
						// Pink
						// 0100
						MotorMove = 0x04;
						SysTick_Wait1ms(20);
						// Yellow
						// 0010
						MotorMove = 0x02;
						SysTick_Wait1ms(20);
						// Orange
						// 0001
						MotorMove = 0x01;
						SysTick_Wait1ms(20);
						// Blue
						// 1000
						MotorMove = 0x08;
						SysTick_Wait1ms(20);
					}
			}
					
		// If button PF4 is pressed
		else if(BUTTON2==0)
			{
				for(i=0; i<256; i++)
					{
						// Orange
						// 0001
						MotorMove = 0x01;
						SysTick_Wait1ms(20);
						// Yellow
						// 0010
						MotorMove = 0x02;
						SysTick_Wait1ms(20);
						// Pink
						// 0100
						MotorMove = 0x04;
						SysTick_Wait1ms(20);
						// Blue
						// 1000
						MotorMove = 0x08;
						SysTick_Wait1ms(20);
					}
			}

}

// Handle SysTick generated interrupts. When timer interrupt triggers, do what's necessary then toggle red and blue LEDs at the same time
void SysTick_Handler(void) {
	GPIO_PORTF_DATA_R ^= 0x06; // Toggles red and blue LEDs
	count = count + 1;
}
