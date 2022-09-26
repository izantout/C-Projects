// Ultrasonic_Sensor.c
// This program runs on TM4C123.
// This is an example program to show how to interface HC-SR04 Ultrasonic sensor.
// PB4 connects to echo pin to generate edge-triggered interrupt.
// PB5 connects to Ultrasonic sensor trigger pin.
// SysTick timer is used to generate the required timing for trigger pin and measure echo pulse width.
// Global variable "distance" is used to hold the distance in cemtimeter for the obstacles
// in front of the sensor. 
// By Dr. Min He
// December 10th, 2018

#include <stdint.h>
#include "SysTick.h"
#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "Timer1A.h"
// Constants
#define TRIGGER_PIN 		(*((volatile unsigned long *)0x40005200))  // PB7 is the trigger pin	
#define TRIGGER_VALUE 	0x80            // trigger at bit 7
#define ECHO_PIN 				(*((volatile unsigned long *)0x40005100))  // PB6 is the echo pin	
#define ECHO_VALUE 			0x40            // trigger at bit 6
#define MC_LEN 					0.0250          // length of one machine cyce in microsecond for 16MHz clock change to 40 MHZ
#define SOUND_SPEED 		0.0343          // centimeter per micro-second
int PERIOD = 400000;           	        // number of machine cycles for 10ms, value is based on 40MH system clock
#define MIN_DUTY    PERIOD*0.1					// minimum duty cycle 40%
#define MAX_DUTY    PERIOD*0.9					// maximum duty cycle 90%
#define HALF_DUTY   PERIOD/2            // 50% duty cycle
#define DUTY_STEP		PERIOD/10					  // duty cycle change for each button press
#define NVIC_EN0_PORTF 0x40000000
#define White 				0x0E
#define Red           0x02
int redFlag = 0;



// Global variables: 
// H: number of clocks cycles for duty cycle
// L: number of clock cycles for non-duty cycle
extern void EnableInterrupts(void);
extern void GPIOPortB_Handler(void);
void PortB_Init(void);
void Delay(void);
void PWM_Init(void);
void SysTick_Init(void);
extern void SysTick_Handler(void);
void GPIOPortF_Handler(int distance);

static volatile uint8_t done=0;
static volatile uint32_t distance=0;
volatile unsigned long H,L;

int main(void){
	PortB_Init();
	PWM_Init();
	SysTick_Init();
	PLL_Init();
  EnableInterrupts();
  while(1){
		done = 0;
		distance = 0;
		TRIGGER_PIN &= ~TRIGGER_VALUE; // send low to trigger
		// SysTick_Wait1us(2); // 2 us delay period is 0.5 and duty cycle is 50% for 0.25 on and off
		Timer1A_Init(80); // 2 us delay (2*10^-6)/(1/(40*10^6)) = 80
		TRIGGER_PIN |= TRIGGER_VALUE; // send high to trigger
		// SysTick_Wait1us(10); // 10 us delay
		Timer1A_Init(400); // 10 us delay (10*10^-6)/(1/(40*10^6)) = 400
		TRIGGER_PIN &= ~TRIGGER_VALUE; // send low to trigger		
    while (!done);
  }
}
/*
Distance (cm) | PWM Duty Cycle
     <10      |       Blink red LED frequency 4 HZ 0.25s on 0.25s off
   10 - 19    |        90%
   20 - 29    |        80%
   30 - 39    |        70%
   40 - 49    |        60%
   50 - 59    |        50%
   60 - 69    |        40%
*/

void PortB_Init(void){
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB;           // 1) activate clock for Port A
	while ((SYSCTL_RCGC2_R&SYSCTL_RCGC2_GPIOB)!=SYSCTL_RCGC2_GPIOB){}; // wait for clock to start
                                    // 2) no need to unlock PA2
  GPIO_PORTB_PCTL_R &= ~0xFF000000; // 3) regular GPIO
  GPIO_PORTB_AMSEL_R &= (uint32_t)~0xC0;      // 4) disable analog function on PA2
  GPIO_PORTB_DIR_R &= ~0x40;        // 5) PB6:echo pin, input
  GPIO_PORTB_DIR_R |= 0x80;         // 5) PB7:trigger pin, output
  GPIO_PORTB_AFSEL_R &= ~0xC0;      // 6) regular port function
  GPIO_PORTB_DEN_R |= 0xC0;         // 7) enable digital port
  GPIO_PORTB_IS_R &= ~0x40;         // PB6 is edge-sensitive
  GPIO_PORTB_IBE_R |= 0x40;         // PB6 is both edges
  GPIO_PORTB_IEV_R &= ~0x40;        // PB6 both edge event
  GPIO_PORTB_ICR_R = 0x40;          // clear flag 6
  GPIO_PORTB_IM_R |= 0x40;          // arm interrupt on PB6
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFFFFF1FF)|0x00000600; // (g) priority 3
  NVIC_EN0_R = 0x00000002;          // (h) enable Port B edge interrupt
}


void GPIOPortB_Handler(void){
	
	if (ECHO_PIN==ECHO_VALUE){  // echo pin rising edge is detected, start timing
		Timer1A_Init(4294967295);
		
	}
	else { // echo pin falling edge is detected, end timing and calculate distance.
    // The following code is based on the fact that the HCSR04 ultrasonic sensor 
    // echo pin will always go low after a trigger with bouncing back
    // or after a timeout. The maximum distance can be detected is 400cm.
		// The speed of sound is approximately 340 meters per second, 
		// or  .0343 c/µS.
    // Distance = (echo pulse width * 0.0343)/2; = ((# of mc)*MC_LEN*SOUND_SPEED)/2
		TIMER1_CTL_R = 0x00000000;    // 10) disable TIMER1A
		distance = (4294967295-TIMER1_TAR_R)*MC_LEN*SOUND_SPEED/2;		
		if(distance < 10){
			// Flash Red LED
			// set period in here
			PERIOD = 20000000;
			// change period from #define to global variable - Done
			// set duty cycle
			H = PERIOD/2;
			// raise a red flag and use systick interrupt
			redFlag = 1;
		} else if (distance>=10 && distance<19){
			// 90%
			PERIOD = 400000;
			redFlag = 0;
			H = MAX_DUTY;
		} else if (distance>=20 && distance<29){
			// 80%
			PERIOD = 400000;
			redFlag = 0;
			H = MAX_DUTY - DUTY_STEP;
		} else if (30<=distance && distance<39){
			// 70%
			PERIOD = 400000;
			redFlag = 0;
			H = MAX_DUTY - 2*(DUTY_STEP);
		} else if (40<=distance && distance<49){
			// 60%
			PERIOD = 400000;
			redFlag = 0;
			H = HALF_DUTY + DUTY_STEP;
		} else if (50<=distance && distance<59){
			// 50%
			PERIOD = 400000;
			redFlag = 0;
			H = HALF_DUTY;
		} else if (60<=distance && distance<69){
			// 40%
			PERIOD = 400000;
			redFlag = 0;
			H = HALF_DUTY - DUTY_STEP;
		} else {
			// All LED off
			PERIOD = 400000;
			redFlag = 0;
			H = MIN_DUTY * 0.01;
		}
		L = PERIOD - H; // constant period of 10ms, variable duty cycle
		done = 1;
	}
  GPIO_PORTB_ICR_R = 0x40;      // acknowledge flag 6
}
void PWM_Init(void){
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;     	// activate F clock
	while ((SYSCTL_RCGC2_R & SYSCTL_RCGC2_GPIOF)!= SYSCTL_RCGC2_GPIOF){} // wait for the clock to be ready
		
  H = L = HALF_DUTY;            			// 50% duty cycle, assume system clock 40MHz, 
  GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY; 	// unlock GPIO Port F
  GPIO_PORTF_CR_R |= 0x0E;         		// allow changes to PF3, 2, 1
  GPIO_PORTF_AMSEL_R &= ~0x0E;  			// disable analog functionality on PF3, PF2, PF1
  GPIO_PORTF_PCTL_R &= ~0x000F0F0F; 	// configure PF3, 2, 1 as GPIO
  GPIO_PORTF_DIR_R |= 0x0E;     			// make PF3, 2, 1 out
  GPIO_PORTF_DATA_R &= ~White;				// Turn off White to start White with duty cycle.
  GPIO_PORTF_AFSEL_R &= ~0x0E;  			// disable alt funct on PF3, 2, 1
  GPIO_PORTF_DEN_R |= 0x0E;     			// enable digital I/O on PF3, 2, 1
}
void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;           			// disable SysTick during setup
  NVIC_ST_RELOAD_R = HALF_DUTY-1;     // reload value for 50% duty cycle
  NVIC_ST_CURRENT_R = 0;        			// any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x1FFFFFFF)|0x40000000; // bit 31-29 for SysTick, set priority to 2
  NVIC_ST_CTRL_R |= NVIC_ST_CTRL_CLK_SRC + NVIC_ST_CTRL_INTEN + NVIC_ST_CTRL_ENABLE;  // enable with core clock and interrupts, start systick timer
}
void SysTick_Handler(void){
	// SysTick ISR:
	// 1. Implement timing control for duty cycle and non-duty cycle
	// 2. Output a waveform based on current duty cycle
	NVIC_ST_CTRL_R &= ~NVIC_ST_CTRL_ENABLE;// turn off SysTick 
	if(redFlag == 0){
		GPIO_PORTF_DATA_R ^= White;            // Toggle White LED
		if(GPIO_PORTF_DATA_R&White){           // previous cycle is duty cycle
			NVIC_ST_RELOAD_R = L-1;              // switch to non-duty cycle
		} else{                                // previous cycle is not duty cycle
			NVIC_ST_RELOAD_R = H-1;              // switch to duty cycle
		}
	}else{
		GPIO_PORTF_DATA_R ^= Red;            // Toggle Red LED
		if(GPIO_PORTF_DATA_R&Red){           // previous cycle is duty cycle
			NVIC_ST_RELOAD_R = L-1;              // switch to non-duty cycle
		} else{                                // previous cycle is not duty cycle
			NVIC_ST_RELOAD_R = H-1;              // switch to duty cycle
		}
	}
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_CTRL_R |= NVIC_ST_CTRL_ENABLE; // turn on systick to continue
}
// Color    LED(s) PortF
// dark     ---    0
// red      R--    0x02
// blue     --B    0x04
// green    -G-    0x08
// yellow   RG-    0x0A
// sky blue -GB    0x0C
// white    RGB    0x0E
// pink     R-B    0x06
