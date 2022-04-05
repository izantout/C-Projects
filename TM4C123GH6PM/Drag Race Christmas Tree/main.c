// ***** 0. Documentation Section *****
// DragRace.c for Project 2
// Runs on TM4C123GH6PM
// Index implementation of a Moore finite state machine to operate a drag race Christmas tree.  
// Issam Zantout

// ***** 0. Include Files  *****

#include <stdint.h>                       // C99 data types
#include "SysTick.h"
#include "tm4c123gh6pm.h"

// ***** 1. Function Prototypes *****

void portInit(void);
void DisableInterrupts(void);             // Disable interrupts
void EnableInterrupts(void);              // Enable interrupts
    
// ***** 2. Global Variable Declerations *****

#define BUTTONS      (*((volatile unsigned long *)0x40004030))// Accesses  PA3(Left)     - PA2(Right)
#define LIGHTS_Left  (*((volatile unsigned long *)0x40024078))// Accesses  PE4(Red Left) - PE3(Yellow 1 Left)- PE2(Yellow 2 Left)- PE1(Green Left)
#define	LIGHTS_Right (*((volatile unsigned long *)0x400063C0))// Accesses  PC7(Red Right)- PC6(Yellow1 Right)- PC5(Yellow2 Right)- PC4(Green Right)
unsigned long CS;
unsigned long Input;

// ***** 3. Struct  *****

struct State {
unsigned long Lights_Left_Output;
unsigned long Lights_Right_Output;
unsigned long Time;                 // 10 ms units
unsigned long Next[4];              // list of next states
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
{0x80, 0x08, 500 ,{RedB,	RedB,	    RedB,	    Yellow1B}},      // Red Both
{0x40, 0x04, 500 ,{RedB,	RedLeft,	RedRight,	Yellow2B}},      // Yellow 1 Both
{0x20, 0x02, 500 ,{RedB,	RedLeft,	RedRight,	GreenB}},        // Yellow 2 Both
{0x10, 0x01, 50  ,{WinB,	WinLeft,	WinRight,	GreenB}},        // Green Both
{0x00, 0x01, 1000,{RedB,	RedB,	    RedB,	    RedB}},          // Win Right
{0x10, 0x00, 1000,{RedB,	RedB,	    RedB,	    RedB}},          // Win Left
{0x10, 0x01, 500 ,{RedB,	RedB,	    RedB,	    RedB}},          // Win Both
{0x00, 0x08, 250 ,{RedB,	RedB,     RedB,	    RedB}},          // Red Right
{0x80, 0x00, 250 ,{RedB,	RedB,	    RedB,	    RedB}}           // Red Left
};


// ***** 4. main function  *****

int main(void){ 
	
	// Before while loop 
	portInit();            // Initializing Ports and Timer
  CS = RedB;             // Initializing State0
  SysTick_Init();				 // Initializing SysTick


  while(1){ 
		// Assigning variables to FSM values
    LIGHTS_Left = ~FSM[CS].Lights_Left_Output;  
		LIGHTS_Right = ~FSM[CS].Lights_Right_Output; 
		// Delay
		SysTick_Wait1ms(FSM[CS].Time);
		// Taking Input
		Input = BUTTONS >> 2; // read sensors
		// Define Next State
		CS = FSM[CS].Next[Input];
	}      
}
// ***** 5. Function Definition  *****

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
