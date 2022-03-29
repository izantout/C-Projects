// ***** 0. Documentation Section *****
// DragRace.c for Project 2
// Runs on TM4C123GH6PM
// Index implementation of a Moore finite state machine to operate a drag race Christmas tree.  
// Issam Zantout
// January 25 2022

// ***** 1. Pre-processor Directives Section *****
#include "SysTick.h"
#include "tm4c123gh6pm.h"
	
void portInit(void)
	{
	volatile unsigned long delay;
		
	SYSCTL_RCGC2_R |= 0x0000011;      // PA, PE clock
	delay = SYSCTL_RCGC2_R;           // delay       
		
// PORT A Initialization
	GPIO_PORTA_DIR_R = 0x00;          // 5) PA4, PA3, PA2 are inputs
	GPIO_PORTA_DEN_R = 0x1C;          // 8) enable digital pins PA4-PA2
		
// PORT E Initialization
	GPIO_PORTE_DIR_R = 0xFF;          // 5) PE7, PE6, PE5, PE4, PE3, PE2, PE1, PE0 are outputs
	GPIO_PORTE_DEN_R = 0xFF;          // 8) enable digital pins PE7-PE0
		
		// I dont think we need port F for this one, we take inputs from port A and give outputs to port E
		// And since he doesnt say he wants on the board display we can remove port F i guess
// PORT F	Initialization
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

    
// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined
void delay10ms(int);

// ***** 3. Subroutines Section *****

struct State {
unsigned long Lights_Output;
unsigned long Time;               // 10 ms units
unsigned long Next[8];            // list of next states
};
typedef const struct State STyp;
#define RedB      0   			        //  Output is Red for Both 
#define Yellow1B  1   			        //  Output is Yellow 1 for Both 
#define Yellow2B  2   			        //  Output is Yellow 2 for Both 
#define GreenB    3   			        //  Output is Green for Both 
#define Yellow1L  4   			        //  Output is Yellow 1 for Left
#define Yellow1R  5   			        //  Output is Yellow 1 for Right
#define Yellow2bF 6                 //  Output is Yellow 2 for Both but the False Start Case
#define WinRight  7   			        //  Output is Green for Right
#define WinLeft   8   			        //  Output is Green for Left
#define RedRight  9   			        //  Output is Green for Right
#define RedLeft   10  			        //  Output is Green for Right


STyp FSM[11] = {
/*
Time should be:
check every 0.5 seconds if both buttons are pushed if they are go to Yellow 1                                      //Red Both
check after 0.5 seconds if any of the buttons are released, if none go to Yellow 2, if any is released go to False // Yellow 1 Both
check after 0.5 seconds if any of the buttons are released, if none go to Green, if any is released go to False    // Yellow 2 Both
check every 0.05 seconds to see which button is released first, if left go to Win Left, if right go to Win Right   // Green Both
0.25 in Yellow 1 Left, 0.25 in Yellow 1 Right, and 0.25 in Yellow Both false                                       // 3 Stages of False Start
1 Second for Win Right, 1 second for Win Left                                                                      // Both Win
0.25 seconds for Red Right and Red Left                                                                            // Both Lose 
*/
{0x88, Time,{}}, // Red Both
{0x44, Time,{}}, // Yellow 1 Both
{0x22, Time,{}}, // Yellow 2 Both
{0x11, Time,{}}, // Green Both
{0x40, Time,{}}, // Yellow 1 Left
{0x04, Time,{}}, // Yellow 1 Right
{0x22, Time,{}}, // Yellow 2 Both False Start
{0x01, Time,{}}, // Win Right
{0x10, Time,{}}, // Win Left
{0x08, Time,{}}, // Red Right
{0x80, Time,{}}  // Red Left
};

// Variable Decleration
#define BUTTONS (*((volatile unsigned long *)0x40004070))//accesses PA4(Left)-PA3(Right)-PA2(Reset)
#define LIGHTS  (*((volatile unsigned long *)0x400243FC))//accesses PE7(Red Left)-PE6(Yellow1 Left)-PE5(Yellow2 Left)-PE4(Green Left)
																												 //       PE3(Red Right)-PE2(Yellow1 Right)-PE1(Yellow2 Right)-PE0(Green Right)
unsigned long CS;
unsigned long Input;
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
