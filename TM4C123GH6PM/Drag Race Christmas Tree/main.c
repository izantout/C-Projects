// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Issam Zantout
// January 25 2022

// ***** 1. Pre-processor Directives Section *****
#include "SysTick.h"
#include "tm4c123gh6pm.h"
	
void portInit(void)
	{
	volatile unsigned long delay;
		
		SYSCTL_RCGC2_R |= 0x0000011;    // PA, PE clock
	delay = SYSCTL_RCGC2_R;           // delay            
// PORT A Initialization
	GPIO_PORTA_DIR_R = 0x00;          // 5) PA2,PA3 input
	GPIO_PORTA_DEN_R = 0x0C;          // 8) enable digital pins PA4-PA0
// PORT E Initialization
	GPIO_PORTE_DIR_R = 0x3F;          // 5) PE2,PE1,PE0 output
	GPIO_PORTE_DEN_R = 0x7F;          // 8) enable digital pins PE5-PE0
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
unsigned long Cars_Output;
unsigned long Turning_Output;
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
{Light1, Light2, Time,{}}, // Red Both
{Light1, Light2, Time,{}}, // Yellow 1 Both
{Light1, Light2, Time,{}}, // Yellow 2 Both
{Light1, Light2, Time,{}}, // Green Both
{Light1, Light2, Time,{}}, // Yellow 1 Left
{Light1, Light2, Time,{}}, // Yellow 1 Right
{Light1, Light2, Time,{}}, // Yellow 2 Both False Start
{Light1, Light2, Time,{}}, // Win Right
{Light1, Light2, Time,{}}, // Win Left
{Light1, Light2, Time,{}}, // Red Right
{Light1, Light2, Time,{}}  // Red Left
};

// Variable Decleration
#define BUTTONS              (*((volatile unsigned long *)0x40004070))    //accesses PA4(WALK)-PA3(SOUTH)-PA2(WEST)
#define LIGHT1               (*((volatile unsigned long *)0x400240FC))	  //accesses PE5(RedE)-PE4(YellowE)-PE3(GreenE)-PE2(RedS)-PE1(YellowS)–PE0(GreenS)
#define lIGHT2           		 (*((volatile unsigned long *)0x40025028))	  //accesses PF3( and PF1	
unsigned long CS;
unsigned long Input;
int main(void){ 

	portInit();            // Initializing Ports and Timer
  CS = RedB;             // Initializing State0
  SysTick_Init();				 // Initializing SysTick


  while(1){ 
		// Assigning variables to FSM values
    VEHICLE_LIGHTS = ~FSM[CS].Cars_Output;  
		TURNING_LIGHTS = FSM[CS].Turning_Output;
		// Delay
		SysTick_Wait1ms(FSM[CS].Time);
		// Taking Input
		Input = BUTTONS >> 2; // read sensors
		// Define Next State
		CS = FSM[CS].Next[Input];
	}
        
}
