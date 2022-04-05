// ***** 0. Documentation Section *****
// DragRace.c for Project 2
// Runs on TM4C123GH6PM
// Index implementation of a Moore finite state machine to operate a drag race Christmas tree.  
// Issam Zantout

// ***** 1. Pre-processor Directives Section *****

#include <stdint.h>                       // C99 data types
#include "SysTick.h"
#include "tm4c123gh6pm.h"

// ***** 1.5 Function Prototypes *****

void portInit(void);
void DisableInterrupts(void);             // Disable interrupts
void EnableInterrupts(void);              // Enable interrupts
void WaitForInterrupt(void);              // Go to low power mode while waiting for the next interrupt

void EdgeCounter_Init(void);              // Initialize edge trigger interrupt for PF0 (SW2) rising edge
void GPIOPortE_Handler(void);             // Handle GPIO Port F interrupts
void SysTick_Handler(void);               // Handle SysTick generated interrupts
    
// ***** 2. Global Declarations Section *****

#define BUTTONS (*((volatile unsigned long *)0x40004070))  //accesses PA4(Left)-PA3(Right)-PA2(Reset)
#define LIGHTS  (*((volatile unsigned long *)0x400243FC))  //accesses PE7(Red Left) - PE6(Yellow1 Left) - PE5(Yellow2 Left) - PE4(Green Left)
																											     	       // PE3(Red Right)- PE2(Yellow1 Right)- PE1(Yellow2 Right)- PE0(Green Right)
unsigned long CS;
unsigned long Input;
unsigned long RisingEdges = 0;
unsigned long count = 0;

// ***** 3. Subroutines Section *****

struct State {
unsigned long Lights_Output;
unsigned long Time;                 // 10 ms units
unsigned long Next[8];              // list of next states
};
typedef const struct State STyp;
#define RedB      0   			        //  Output is Red for Both 
#define Yellow1B  1   			        //  Output is Yellow 1 for Both 
#define Yellow2B  2   			        //  Output is Yellow 2 for Both 
#define GreenB    3   			        //  Output is Green for Both 
#define WinRight  4   			        //  Output is Green for Right
#define WinLeft   5   			        //  Output is Green for Left
#define WinB      6                 //  Output is Green for Both
#define RedRight  7   			        //  Output is Red for Right
#define RedLeft   8  			          //  Output is Red for Left


STyp FSM[9] = {  
{0x88, 500 ,{RedB,	RedB,	    RedB,	    Yellow1B}},      // Red Both
{0x44, 500 ,{RedB,	RedLeft,	RedRight,	Yellow2B}},      // Yellow 1 Both
{0x22, 500 ,{RedB,	RedLeft,	RedRight,	GreenB}},        // Yellow 2 Both
{0x11, 50  ,{WinB,	WinLeft,	WinRight,	GreenB}},        // Green Both
{0x01, 1000,{RedB,	RedB,	    RedB,	    RedB}},          // Win Right
{0x10, 1000,{RedB,	RedB,	    RedB,	    RedB}},          // Win Left
{0x11, 500 ,{RedB,	RedB,	    RedB,	    RedB}},          // Win Both
{0x08, 250 ,{RedB,	RedB,     RedB,	    RedB}},          // Red Right
{0x80, 250 ,{RedB,	RedB,	    RedB,	    RedB}}           // Red Left
};

int main(void){ 

	portInit();            // Initializing Ports and Timer
  CS = RedB;             // Initializing State0
  SysTick_Init();				 // Initializing SysTick


  while(1){ 
		// Assigning variables to FSM values
    LIGHTS = ~FSM[CS].Lights_Output;  
		// Delay
		SysTick_Wait1ms(FSM[CS].Time);
		// Taking Input
		Input = BUTTONS >> 2; // read sensors
		// Define Next State
		CS = FSM[CS].Next[Input];
	}
        
}

void portInit(void)
	{
	volatile unsigned long delay;
		
	SYSCTL_RCGC2_R |= 0x0000011; 		        // 1) PORTA & PORTE clock
	delay = SYSCTL_RCGC2_R;                 // delay       
		
// PORT A Initialization
	GPIO_PORTA_DIR_R = 0x00;                // 5) PA3 & PA2 are inputs, give them a 0
	GPIO_PORTA_DEN_R = 0x0C;                // 8) enable digital pins PA3 & PA2
		
// PORT E Initialization
	GPIO_PORTE_DIR_R = 0xFF;                // 5) PE7-PE0 are outputs, give them a 1
	GPIO_PORTE_DEN_R = 0xFF;                // 8) enable digital pins PE7-PE0
	}
