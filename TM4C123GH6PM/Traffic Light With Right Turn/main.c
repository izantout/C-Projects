// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Issam Zantout
// January 25 2022

// ***** 1. Pre-processor Directives Section *****
#include "SysTick.h"
#include "tm4c123gh6pm.h"
	

void delay10ms(int delay)
{

    unsigned long volatile time;

    time = (727240*200/91)* delay;  // 0.1sec * delay

    while(time){

        time--;

  }
}
void portInit(void)
	{
	volatile unsigned long delay;
		
		SYSCTL_RCGC2_R |= 0x0000011; // PA, PE clock
	delay = SYSCTL_RCGC2_R; // delay            
// PORT A
	GPIO_PORTA_DIR_R = 0x00;  // 5) PA2,PA3 input
	GPIO_PORTA_DEN_R = 0x0C;  // 8) enable digital pins PA4-PA0
// PORT E		
	GPIO_PORTE_DIR_R = 0x3F;  // 5) PE2,PE1,PE0 output
	GPIO_PORTE_DEN_R = 0x7F;  // 8) enable digital pins PE5-PE0
// PORT F		
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
unsigned long Time;    // 10 ms units
unsigned long Next[8]; // list of next states
};
typedef const struct State STyp;
#define goS   0   			//  Output of North/South light is 0x21
#define waitS 1   			//  Output of North/South light is 0x22
#define goW   2   			//  Output of East/West light is 0x0C
#define waitW 3   			//  Output of East/west light is 0x14
#define goR   4   			//  Output of North/South light is 0x21, Output of turning light is 0x08
#define waitR 5  			  //  Output of North/South light is 0x22, Output of turning light is 0x0A
#define stopR 6   			//  Output of North/South light is 0x0c, Output of turning light is 0x02


STyp FSM[7] = {
{0x21, 0x02, 6500,{goS,waitS,goS,waitS,goR,waitS,goR,waitS}}, 	      	  // goS
{0x22, 0x02, 1500,{goW,goW,goW,goW,goW,goW,goW,goW}}, 									  // waitS
{0x0C, 0x02, 6500,{goW,goW,waitW,waitW,waitW,waitW,waitW,waitW}}, 			  // goW
{0x14, 0x02, 1500,{goS,goS,goS,goS,goR,goR,goR,goR}}, 								  	// waitW
{0x21, 0x08, 6500,{waitR,waitR,waitR,waitR,goR,goR,goR,goR}}, 					  // goR
{0x21, 0x0A, 1500,{stopR,stopR,stopR,stopR,stopR,stopR,stopR,stopR}},     // waitR
{0x21, 0x02, 6500,{goS,goW,goS,goS,goR,goR,goR,goR}} 					            // stopR
};

// Variable Decleration
#define BUTTONS              (*((volatile unsigned long *)0x40004070))  //acesses PA4(WALK)-PA3(SOUTH)-PA2(WEST)
#define VEHICLE_LIGHTS       (*((volatile unsigned long *)0x400240FC))	//accesses PE5(RedE)-PE4(YellowE)-PE3(GreenE)-PE2(RedS)-PE1(YellowS)–PE0(GreenS)
#define TURNING_LIGHTS   		 (*((volatile unsigned long *)0x40025028))	//accesses PF3( and PF1	
unsigned long CS;
unsigned long Input;
int main(void){ 

	portInit();           // Initializing Ports and Timer
  CS = goW;             // Initializing State0
  SysTick_Init();				// Initializing SysTick


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
