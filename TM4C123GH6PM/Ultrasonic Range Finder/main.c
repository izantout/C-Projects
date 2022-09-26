#include <stdint.h>
#include "SysTick.h"
#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "Timer1A.h"#define TRIGGER_PIN 		(*((volatile unsigned long *)0x40005080))// PB5 is the trigger pin	
#define TRIGGER_VALUE 	0x20                                     // trigger at bit 5
#define ECHO_PIN 				(*((volatile unsigned long *)0x40005040))// PB4 is the echo pin	
#define ECHO_VALUE 			0x10                                     // trigger at bit 4
#define MC_LEN 					0.025                                    // length of one machine cycle in microsecond for 40MHz clock
#define SOUND_SPEED 		0.0343 // centimeter per micro-second

#define PF1       			(*((volatile unsigned long *)0x40025008))
#define PF2       			(*((volatile unsigned long *)0x40025010))
#define PF3       			(*((volatile unsigned long *)0x40025020))
#define LEDS      			(*((volatile unsigned long *)0x40025038))
#define RED       			0x02
#define WHITE      			0x0E

extern void EnableInterrupts(void);
extern void GPIOPortB_Handler(void);
void PortB_Init(void);
void Delay(void);
void WaitForInterrupt(void);
void PWM_Init(void);

static volatile uint8_t done=0;
static volatile uint32_t distance=0;
volatile unsigned long H,L;

int main(void){
	// Port B initialization
	PortB_Init(); // PB4, PB5
	
	// PLL initialization
	PLL_Init();   // bus clock at 40 MHz
	
	// PMW initialization
  PWM_Init();
	
	distance = 0; // distance
	H = 0;        // High trigger
	while(1){
		done = 0;   // Done is false
		
		TRIGGER_PIN &= ~TRIGGER_VALUE; // Send low to trigger
		Timer1A_Init(80);              // Wait 2us
		TRIGGER_PIN |= TRIGGER_VALUE;  // Send high to trigger
		Timer1A_Init(80);              // Wait 2us
		TRIGGER_PIN &= ~TRIGGER_VALUE; // Send low to trigger
		
    while (!done);
		if(distance < 10){ 						               // If the distance is less than 10 cm
			GPIO_PORTF_DATA_R = 0x02;                  // Red light on
			SysTick_Wait1us(250000);                   // Wait 0.25s
			GPIO_PORTF_DATA_R = 0x00;                  // Turn red LED off
			SysTick_Wait1us(250000);                   // Wait 0.25
		} else if(distance >= 10  && distance < 20){ // If the distance is between 10 inclusive and 20
			H = 900;                                   // H is 900
		} else if(distance >= 20  && distance < 30){ // If the distance is between 20 inclusive and 30
			H = 800;                                   // H is 800
		} else if(distance >= 30  && distance < 40){ // If the distance is between 30 inclusive and 40
			H = 700;                                   // H is 700
		} else if(distance >= 40  && distance < 50){ // If the distance is between 40 inclusive and 50
			H = 600;                                   // H is 600
		} else if(distance >= 50  && distance < 60){ // If the distance is between 50 inclusive and 60
			H = 500;                                   // H is 500
		} else if(distance >= 60  && distance < 70){ // If the distance is between 60 inclusive and 70
			H = 400;                                   // H is 400
		} else if(distance >= 70){                   // If the distance is 70 or higher
			GPIO_PORTF_DATA_R = 0x00;                  // All the LEDs are off
		} else{                                      // Else case
			GPIO_PORTF_DATA_R = 0x0E;
			SysTick_Wait1us(H);
			GPIO_PORTF_DATA_R = 0x00;
			SysTick_Wait1us(L);
		}
		L = 1000 - H;                                // Update L to be 1000-H
  }
}

void PortB_Init(void){ 
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB;              // 1) activate clock for Port B
	while ((SYSCTL_RCGC2_R&SYSCTL_RCGC2_GPIOB)!=SYSCTL_RCGC2_GPIOB){}; // wait for clock to start
                                                     // 2) no need to unlock PB4-5
  GPIO_PORTB_PCTL_R &= ~0x00FF0000;                  // 3) regular GPIO
  GPIO_PORTB_AMSEL_R &= (uint32_t)~0x30;             // 4) disable analog function on PB4-5
  GPIO_PORTB_DIR_R &= ~0x10;                         // 5) PB4: echo pin, input
  GPIO_PORTB_DIR_R |= 0x20;                          // 5) PB5: trigger pin, output
  GPIO_PORTB_AFSEL_R &= ~0x30;                       // 6) regular port function
  GPIO_PORTB_DEN_R |= 0x30;                          // 7) enable digital port
  GPIO_PORTB_IS_R &= ~0x10;                          // PB4 is edge-sensitive
  GPIO_PORTB_IBE_R |= 0x10;                          // PB4 is both edges
  GPIO_PORTB_IEV_R &= ~0x10;                         // PB4 both edge event
  GPIO_PORTB_ICR_R = 0x10;                           // clear flag 4
  GPIO_PORTB_IM_R |= 0x10;                           // arm interrupt on PB4
  NVIC_PRI0_R = (NVIC_PRI0_R&0xFFFF1FFF)|0x00006000; // (g) priority 3
  NVIC_EN0_R = 0x00000002;                           // (h) enable Port B edge interrupt
}

void GPIOPortB_Handler(void){
	
	if (ECHO_PIN==ECHO_VALUE){  // echo pin rising edge is detected, start timing
		SysTick_Start();
	}
	else { // echo pin falling edge is detected, end timing and calculate distance.
    // The following code is based on the fact that the HCSR04 ultrasonic sensor 
    // echo pin will always go low after a trigger with bouncing back
    // or after a timeout. The maximum distance can be detected is 400cm.
		// The speed of sound is approximately 340 meters per second, 
		// or  .0343 c/µS.
    // Distance = (echo pulse width * 0.0343)/2; = ((# of mc)*MC_LEN*SOUND_SPEED)/2
		SysTick_Stop();
		distance = (uint32_t)(SysTick_Get_MC_Elapsed()*MC_LEN*SOUND_SPEED)/2;	// Time* speed of sound * mcLen/2	
		done = 1;
	}
  GPIO_PORTB_ICR_R = 0x10;      // acknowledge flag 4
}

void PWM_Init(void){
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;     	// activate F clock
	while ((SYSCTL_RCGC2_R & SYSCTL_RCGC2_GPIOF)!= SYSCTL_RCGC2_GPIOF){} // wait for the clock to be ready
		
  GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY; 	// unlock GPIO Port F
  GPIO_PORTF_CR_R |= 0x0E;         		// allow changes to PF3-1
  GPIO_PORTF_AMSEL_R &= ~0x0E;  			// disable analog functionality on PF3-1
  GPIO_PORTF_PCTL_R &= ~0x0000FFF0; 	// configure PF3-1 as GPIO
  GPIO_PORTF_DIR_R |= 0x0E;     			// make PF3-1 out
  GPIO_PORTF_DATA_R &= ~0x0E;					// Turn off Blue to start Blue with duty cycle.
  GPIO_PORTF_AFSEL_R &= ~0x0E;  			// disable alt funct on PF2
  GPIO_PORTF_DEN_R |= 0x0E;     			// enable digital I/O on PF2
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF1FFFFF)|0x00400000; // (g) bits:23-21 for PORTF, set priority to 2
  NVIC_EN0_R = 0x40000000;      // (h) enable interrupt 30 in NVIC
}
