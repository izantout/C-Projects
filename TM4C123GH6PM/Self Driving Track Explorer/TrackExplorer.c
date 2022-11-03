// TrackExplorer.c
// Runs on TM4C123
// This is the starter file for CECS 347 Project 2 - A Track Explorer
// This project use PWM to control two DC Motors, SysTick timer 
// to control sampling rate, ADC to collect analog inputs from
// Sharp IR sensors and a potentiometer.
// Two GP2Y0A21YK0F analog IR distance sensors are used to allow
// the robot to follow a wall. A Minimum of two IR sensors are mounted
// looking forward to the left and forward to the right. 
// A optional third IR sensor looks directly forward can be used to avoid
// a head-on collision. Basically, the goal is to control power to each wheel so the
// left and right distances to the walls are equal.
// If an object is detected too close to the front of the robot,
// both wheels are immediately stopped.
/*
    ------------------------------------------wall---------
                      /
                     /
                    / 
                   /
         -----------
         |         |
         | Robot   | ---> direction of motion and third sensor
         |         |
         -----------
                   \
                    \
                     \
                      \
    ------------------------------------------wall---------
*/
// The original project is provided by Dr. Daniel Valvano, Jonathan Valvano
// September 12, 2013
// Modification is made by Dr. Min He to provide this starter project.

// PE1 connected to forward facing IR distance sensor
// PE4 connected to forward right IR distance sensor
// PE5 connected to forward left IR distance sensor

#include "ADCMultiSamples.h"
#include "PLL.h"
#include "tm4c123gh6pm.h"
#include "stdint.h"
#include "PWM.h"
#include "GPIO.h"


// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // low power mode
void Delay(void); // Delay function


uint8_t sample=0;

// You use datasheet to calculate the following ADC values
// then test your sensors to adjust the values 
#define CRASH             IR15CM// if there is less than this distance ahead of the robot, it will immediately stop
#define IR15CM            2233  // ADC output for 15cm:1.8v -> (1.8/3.3)4095=2233      RANGE WE GOT: 2024 - 2333
#define IR20CM            1724  // ADC output for 20cm:1.39v -> (1.39/3.3)4095=1724    RANGE WE GOT: 1624 - 1768
#define IR30CM            1116  // ADC output for 30cm:0.9v -> (0.9/3.3)4095=1116      RANGE WE GOT: 1204 - 1320
#define IR40CM            918   // ADC output for 40cm:0.74v -> (0.74/3.3)4095=918     RANGE WE GOT: 855 - 1151
#define IR80CM            496   // ADC output for 80cm:0.4v -> (0.4/3.3)*4095=496       RANGE WE GOT: 479(middle) - 570(left) - 560(right)
                                // with equal power to both motors (LeftH == RightH), the robot still may not drive straight
                                // due to mechanical differences in the motors, so bias the left wheel faster or slower than
                                // the constant right wheel
#define LEFTMINPCT        30    // minimum percent duty cycle of left wheel (10 to 90)
#define LEFTMAXPCT        50    // maximum percent duty cycle of left wheel (10 to 90)
#define RIGHTCONSTPCT     40    // constant percent duty cycle of right wheel (10 to 90)

void System_Init(void);
void LEDSW_Init(void);
void Motor_Init(void);
void SysTick_Init(void);
void steering(uint16_t ahead_dist,uint16_t right_dist, uint16_t left_dist);
void ReadADCFIRFilter(uint16_t *ain2, uint16_t *ain9, uint16_t *ain8);
void ReadADCIIRFilter(uint16_t *ain2, uint16_t *ain9, uint16_t *ain8);
uint16_t median(uint16_t u1, uint16_t u2, uint16_t u3);
void ReadADCMedianFilter(uint16_t *ain2, uint16_t *ain9, uint16_t *ain8);// This function samples AIN2 (PE1), AIN9 (PE4), AIN8 (PE5) and

int main(void){
  uint16_t left, right, ahead;
  int move = 0;
  DisableInterrupts();  // disable interrupts while initializing
  System_Init();
  EnableInterrupts();   // enable after all initialization are done
	// 
  while(1){
		if((GPIO_PORTF_DATA_R & 0x10) == 0x00){
			move = 1;}
		if((GPIO_PORTF_DATA_R & 0x01) == 0x00){
			move = 0;}
		if(move == 1){
				sample = 0;
				// choose one of the software filter methods
				ReadADCMedianFilter(&ahead, &right, &left);
				
				steering(ahead,right,left);}
  }
}

void Delay(void){
	unsigned long volatile time;
  time = 727240*250/91;  // 0.125sec
  while(time){
		time--;
  }
}

void System_Init(void) {
  PLL_Init();           // bus clock at 80 MHz
  SysTick_Init();       // Initialize SysTick timer with interrupt for smapling control
  ADC_Init298();        // initialize ADC to sample AIN2 (PE1), AIN9 (PE4), AIN8 (PE5)
  LEDSW_Init();         // configure onboard LEDs and push buttons
  Motor_Init();         // Initialize signals for the two DC Motors
}

void LEDSW_Init(void){
	LED_Init();
}

void Motor_Init(void){
	Car_Dir_Init();
	PWM_PB76_Init();
  PWM_PB76_Duty(START_SPEED_L, START_SPEED_R);
}

void SysTick_Init(void){
}

void steering(uint16_t ahead_dist,uint16_t right_dist, uint16_t left_dist){
  // Suggest the following simple control as starting point:
  // 1. If any one of the senors see obstacle <20cm, stop
	if(ahead_dist < 700 && right_dist < 700){
		LED = Purple;
		WHEEL_DIR = STOP;
		PWM0_ENABLE_R |= 0x00000000;
	} else if(ahead_dist > 1768){
				LED = Green;
				WHEEL_DIR = LEFTPIVOT;
				PWM0_ENABLE_R |= 0x00000003; // enable both wheels
			} else if(ahead_dist > 1768 && (left_dist > 1768 || right_dist > 1768)){
			if(left_dist > right_dist){
				LED = Blue;
				WHEEL_DIR = RIGHTPIVOT;
				PWM0_ENABLE_R |= 0x00000003; // enable both wheels
			} else if (left_dist < right_dist){
				LED = Green;
				WHEEL_DIR = LEFTPIVOT;
				PWM0_ENABLE_R |= 0x00000003; // enable both wheels
			} else {
				LED = White;
				WHEEL_DIR = BACKWARD;
				PWM0_ENABLE_R |= 0x00000003; // enable both wheels
			}
	} else if( left_dist > 1768){
		LED = Blue;
		WHEEL_DIR = RIGHTPIVOT;
		PWM0_ENABLE_R |= 0x00000003; // enable both wheels
	} else if (right_dist > 1768){
		LED = Green;
		WHEEL_DIR = LEFTPIVOT;
		PWM0_ENABLE_R |= 0x00000003; // enable both wheels
	} else {
		LED = Cran;
		WHEEL_DIR = FORWARD;
		PWM0_ENABLE_R |= 0x00000003; // enable both wheels
	}
	/*
	  if (right_dist > 1768){
			if(right_dist > left_dist){
				LED = Blue;
				WHEEL_DIR = LEFTPIVOT;
				PWM0_ENABLE_R |= 0x00000003; // enable both wheels
			}
		}
		if (left_dist > 1768){
			if (left_dist > right_dist){
				LED = Blue;
				WHEEL_DIR = RIGHTPIVOT;
				PWM0_ENABLE_R |= 0x00000003; // enable both wheels
			}
		}
		if (ahead_dist > 1768){
			LED = Red;
			WHEEL_DIR = STOP;
			PWM0_ENABLE_R |= 0x00000003; // enable both wheels
		} else {
			LED = Green;
			WHEEL_DIR = FORWARD;
			PWM0_ENABLE_R |= 0x00000003; // enable both wheels
		}
		if(ahead_dist > 1768 && (left_dist > 1768 || right_dist > 1768)){
			LED = Blue;
			WHEEL_DIR = RIGHTPIVOT;
			PWM0_ENABLE_R |= 0x00000003; // enable both wheels
		
		}
		*/
  // 2. If all sensors detect no obstacle within 35cm, stop
  // 3. If left sees obstacle within 30cm, turn right
  // 4. If right sees obstacle within 30cm, turn left
  // 5. If both sensors see no obstacle within 30cm, go straight  
  // Feel free to add more controlls to fine tune your robot car.
  // Make sure to take care of both wheel movements and LED display here.
  
}
void SysTick_Handler(void){
  sample = 1;
}

// returns the results in the corresponding variables.  Some
// kind of filtering is required because the IR distance sensors
// output occasional erroneous spikes.  This is an FIR filter:
// y(n) = (x(n) + x(n-1))/2
// Assumes: ADC initialized by previously calling ADC_Init298()
void ReadADCFIRFilter(uint16_t *ain2, uint16_t *ain9, uint16_t *ain8){
  static uint16_t ain2previous=0; // after the first call, the value changed to 12
  static uint16_t ain9previous=0;
  static uint16_t ain8previous=0;
  // save some memory; these do not need to be 'static'
  //            x(n)
  uint16_t ain2newest;
  uint16_t ain9newest;
  uint16_t ain8newest;
  ADC_In298(&ain2newest, &ain9newest, &ain8newest); // sample AIN2(PE1), AIN9 (PE4), AIN8 (PE5)
  *ain2 = (ain2newest + ain2previous)/2;
  *ain9 = (ain9newest + ain9previous)/2;
  *ain8 = (ain8newest + ain8previous)/2;
  ain2previous = ain2newest; ain9previous = ain9newest; ain8previous = ain8newest;
}

// This function samples AIN2 (PE1), AIN9 (PE4), AIN8 (PE5) and
// returns the results in the corresponding variables.  Some
// kind of filtering is required because the IR distance sensors
// output occasional erroneous spikes.  This is an IIR filter:
// y(n) = (x(n) + y(n-1))/2
// Assumes: ADC initialized by previously calling ADC_Init298()
void ReadADCIIRFilter(uint16_t *ain2, uint16_t *ain9, uint16_t *ain8){
  //                   y(n-1)
  static uint16_t filter2previous=0;
  static uint16_t filter9previous=0;
  static uint16_t filter8previous=0;
  // save some memory; these do not need to be 'static'
  //            x(n)
  uint16_t ain2newest;
  uint16_t ain9newest;
  uint16_t ain8newest;
  ADC_In298(&ain2newest, &ain9newest, &ain8newest); // sample AIN2(PE1), AIN9 (PE4), AIN8 (PE5)
  *ain2 = filter2previous = (ain2newest + filter2previous)/2;
  *ain9 = filter9previous = (ain9newest + filter9previous)/2;
  *ain8 = filter8previous = (ain8newest + filter8previous)/2;
}

// Median function from EE345M Lab 7 2011; Program 5.1 from Volume 3
// helper function for ReadADCMedianFilter() but works for general use
uint16_t median(uint16_t u1, uint16_t u2, uint16_t u3){
uint16_t result;
  if(u1>u2)
    if(u2>u3)   result=u2;     // u1>u2,u2>u3       u1>u2>u3
      else
        if(u1>u3) result=u3;   // u1>u2,u3>u2,u1>u3 u1>u3>u2
        else      result=u1;   // u1>u2,u3>u2,u3>u1 u3>u1>u2
  else
    if(u3>u2)   result=u2;     // u2>u1,u3>u2       u3>u2>u1
      else
        if(u1>u3) result=u1;   // u2>u1,u2>u3,u1>u3 u2>u1>u3
        else      result=u3;   // u2>u1,u2>u3,u3>u1 u2>u3>u1
  return(result);
}

// This function samples AIN2 (PE1), AIN9 (PE4), AIN8 (PE5) and
// returns the results in the corresponding variables.  Some
// kind of filtering is required because the IR distance sensors
// output occasional erroneous spikes.  This is a median filter:
// y(n) = median(x(n), x(n-1), x(n-2))
// Assumes: ADC initialized by previously calling ADC_Init298()
void ReadADCMedianFilter(uint16_t *ain2, uint16_t *ain9, uint16_t *ain8){
  //                   x(n-2)        x(n-1)
  static uint16_t ain2oldest=0, ain2middle=0;
  static uint16_t ain9oldest=0, ain9middle=0;
  static uint16_t ain8oldest=0, ain8middle=0;
  // save some memory; these do not need to be 'static'
  //            x(n)
  uint16_t ain2newest;
  uint16_t ain9newest;
  uint16_t ain8newest;
  ADC_In298(&ain2newest, &ain9newest, &ain8newest); // sample AIN2(PE1), AIN9 (PE4), AIN8 (PE5)
  *ain2 = median(ain2newest, ain2middle, ain2oldest);
  *ain9 = median(ain9newest, ain9middle, ain9oldest);
  *ain8 = median(ain8newest, ain8middle, ain8oldest);
  ain2oldest = ain2middle; ain9oldest = ain9middle; ain8oldest = ain8middle;
  ain2middle = ain2newest; ain9middle = ain9newest; ain8middle = ain8newest;
}

