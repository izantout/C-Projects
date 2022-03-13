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
		
		SYSCTL_RCGC2_R |= 0x0000011; // PA, PB clock
	delay = SYSCTL_RCGC2_R; // delay            
// PORT A
	GPIO_PORTA_DIR_R = 0x00;  // 5) PA2,PA3 input
	GPIO_PORTA_DEN_R = 0x0C;  // 8) enable digital pins PA4-PA0
// PORT E		
	GPIO_PORTE_DIR_R = 0x3F;  // 5) PE2,PE1,PE0 output
	GPIO_PORTE_DEN_R = 0x7F;  // 8) enable digital pins PB5-PB0
// PORT F		
		SYSCTL_RCGC2_R |= 0X0000020; // PF clock
	GPIO_PORTF_DIR_R = 	0x0A; // 5) PF4 input, PF3,PF1 output
	GPIO_PORTF_DEN_R =  0x0C; // 8) enable digital pins PF4-PF0
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
{0x21, 0x02, 3000,{goS,waitS,goS,waitS,goS,waitS,goS,waitS}}, 					// goS   .
{0x22, 0x02, 500,{goW,goW,goW,goW,goW,goW,goW,goW}}, 									  // waitS .
{0x0C, 0x02, 3000,{goW,goW,waitW,waitW,waitW,waitW,waitW,waitW}}, 			// GoW   .
{0x14, 0x02, 500,{goS,goS,goS,goS,goS,goS,goS,goS}}, 								  	// waitW .
{0x0A, 0x08, 3000,{waitR,waitR,waitR,waitR,goR,goR,goR,goR}}, 					// goR   .
{0x0A, 0x0A, 500,{stopR,stopR,stopR,stopR,stopR,stopR,stopR,stopR}},    // waitR .
{0x0A, 0x02, 3000,{stopR,stopR,stopR,stopR,goR,goR,goR,goR}} 					  // stopR .
};

unsigned long CS;     // index of current state
unsigned long Input;

int main(void){ 

	portInit();           // initialize ports and timer
  CS = goS;             // initial state
  SysTick_Init();


  while(1){ 
		//GPIO_PORTB_DATA_R = FSM[CS].Cars_Output; // set signals
    GPIO_PORTE_DATA_R = ~FSM[CS].Cars_Output;  
		GPIO_PORTF_DATA_R = FSM[CS].Turning_Output;		
		// delay10ms(FSM[CS].Time);
		// delay10ms(5);
		// SysTick_wait1ms(changes depending on the state)
		SysTick_Wait1ms(FSM[CS].Time);
		Input = GPIO_PORTA_DATA_R >> 2; // read sensors
		CS = FSM[CS].Next[Input];
	}
        
}
