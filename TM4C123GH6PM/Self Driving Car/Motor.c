// ***** 1. Include Files  *****
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "SysTick.h"

// ***** 2. Global Variable Declerations *****
#define BUTTON1    (*((volatile unsigned long *)0x40025004))// Accesses  PF0(Button1)SWITCH2
#define BUTTON2    (*((volatile unsigned long *)0x40025040))// Accesses  PF4(Button2)SWITCH1
volatile uint32_t RisingEdges = 0;
volatile uint32_t count = 0;
unsigned long CS;
unsigned long STEPPER;

// ***** 3. Function Prototypes *****
void Stepper_Forward(unsigned long delay);
void Stepper_Back(unsigned long delay);
void MotorMover(void);
void portInit(void);
void DisableInterrupts(void);
void EnableInterrupts(void);
void WaitForInterrupt(void);
void EdgeCounter_Init(void);
void SysTick_Init(void);
void SysTick_Wait1ms(unsigned long delay);
void SysTick_Wait(unsigned long delay);
void GPIOPortF_Handler(void);
void SysTick_Handler(void);

struct State
{
  unsigned long Out;
  unsigned long Next[8];
};

typedef const struct State STyp;

#define initial 0        
#define pink 1
#define yellow 2
#define orange 3
#define blue 4

STyp fsm[5] =
{
  {0x08,{blue,blue,blue,blue,blue,orange,pink,blue}}, // initial
  {0x04,{pink,pink,pink,pink,pink,blue,yellow,pink}}, // pink
  {0x02,{yellow,yellow,yellow,yellow,yellow,pink,orange,yellow}}, // yellow
  {0x01,{orange,orange,orange,orange,orange,yellow,blue,orange}}, // orange
  {0x08,{blue,blue,blue,blue,blue,orange,pink,blue}}  // blue
};

// ***** 4. Function Definition  *****
void Stepper_Forward(unsigned long delay)
{
  CS = fsm[CS].Next[CS]; 
  STEPPER = fsm[CS].Out;
  SysTick_Wait1ms(delay);
}

void Stepper_Back(unsigned long delay)
{
  CS = fsm[CS].Next[CS];
  STEPPER = fsm[CS].Out;
  SysTick_Wait1ms(delay);
}

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

void SysTick_Handler(void) {
	GPIO_PORTF_DATA_R ^= 0x06;
	count = count + 1;
}
