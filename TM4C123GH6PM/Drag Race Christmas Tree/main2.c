// ***** 0. Documentation Section *****
// DragRace.c for Project 2
// Runs on TM4C123GH6PM
// Index implementation of a Moore finite state machine to operate a drag race Christmas tree.  
// Nic Moy

// ***** 0. Include Files  *****

#include <stdint.h>                       // C99 data types
#include "SysTick.h"
#include "tm4c123gh6pm.h"

// ***** 1. Function Prototypes *****

void portInit(void);
void DisableInterrupts(void);             // Disable interrupts
void EnableInterrupts(void);              // Enable interrupts
    
// ***** 2. Global Variable Declerations *****



// ***** 3. Struct  *****

struct State {
unsigned long Lights_Left_Output;
unsigned long Lights_Right_Output;
unsigned long Time;                 // 10 ms units
unsigned long Next[4];              // list of next states
};
typedef const struct State STyp;
#define Initialize         0								//  Output is all LEDs on
#define Wait_Staging       1                //  Output is all LEDs off
#define Yellow1Both     	 2	              //  Output is Yellow 1 for Both countdown Y1
#define Yellow2Both    		 3	              //  Output is Yellow 2 for Both countdown Y2
#define Go       			     4       					//  Output is Green for Both   Go state
#define WinRight   			   5     						//  Output is Green for Right  win right
#define WinLeft       		 6	   			      //  Output is Green for Left  win left
#define WinBoth            7           		  //  Output is Green for Both  win both
#define FalseStartRight    8  			        //  Output is Red for Right   false start left
#define FasleStartLeft     9		            //  Output is Red for Left    false start right
#define FasleStartBoth     10			          //  Output is Red for Both  false start both



STyp FSM[11] = {
{0xFF, 0xFF, 2000, {Wait_Staging, Wait_Staging, Wait_Staging, Wait_Staging}}, 				 //Initialize
{0x00, 0x00, 1000, {Wait_Staging, Wait_Staging, Wait_Staging, Yellow1Both}}, 					 //Wait for staging
{0x08, 0x08, 1000, {FasleStartBoth,	FasleStartLeft,	FalseStartRight, Yellow2Both}},    // Countdown Y1
{0x04, 0x04, 1000, {FasleStartBoth,	FasleStartLeft,	FalseStartRight, Go}},     				 // Countdown Y2
{0x02, 0x02, 50,   {WinBoth,	WinLeft,	WinRight,	Go}},        												 // Go state
{0x00, 0x02, 2000, {Wait_Staging,	Wait_Staging, Wait_Staging, Wait_Staging}},          // Win right
{0x02, 0x00, 2000, {Wait_Staging,	Wait_Staging, Wait_Staging, Wait_Staging}},        	 // Win left
{0x02, 0x02, 2000, {Wait_Staging,	Wait_Staging, Wait_Staging,	Wait_Staging}},        	 // Win Both
{0x00, 0x10, 2000, {Wait_Staging,	Wait_Staging, Wait_Staging, Wait_Staging}},          // False start right
{0x10, 0x00, 2000, {Wait_Staging,	Wait_Staging,	Wait_Staging,	Wait_Staging}},          // False start left
{0x10, 0x10, 2000, {Wait_Staging,	Wait_Staging,	Wait_Staging,	Wait_Staging}}           // False start both
};


// ***** 4. main function  *****
#define BUTTONS      (*((volatile unsigned long *)0x40004030))// Accesses  PA3(Left)     - PA2(Right)
#define LIGHTS_Left  (*((volatile unsigned long *)0x40024078))// Accesses  PE4(Red Left) - PE3(Yellow 1 Left)- PE2(Yellow 2 Left)- PE1(Green Left)
#define	LIGHTS_Right (*((volatile unsigned long *)0x40025078))// Accesses  PF4(Red Right)- PF3(Yellow1 Right)- PF2(Yellow2 Right)- PF1(Green Right)
unsigned long CS;
unsigned long Input;
int main(void){ 
	
	// Before while loop 
	portInit();            // Initializing Ports and Timer
  CS = Initialize;       // Initializing State0
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
	GPIO_PORTE_DIR_R = 0x1E;                // 5) PE7-PE0 are outputs, give them a 1
	GPIO_PORTE_DEN_R = 0x1E;                // 8) enable digital pins PE7-PE0
		
// PORT F Initialization		
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) F clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0       
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R = 0x1E;          // 5) PF4,PF0 input, PF3,PF2,PF1 output   
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTF_PUR_R = 0x1E;          // enable pullup resistors on PF4-PF0       
  GPIO_PORTF_DEN_R = 0x1E;          // 7) enable digital pins PF4-PF0 
	}
