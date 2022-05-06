// ***** 1. Include Files  *****
#include <stdint.h>
#include "SysTick.h"
#include "Motor.h"

// ***** 2. Global Variable Declerations *****
#define GPIO_PORTF_DATA_R       (*((volatile unsigned long *)0x400253FC))
#define NVIC_EN0_R              (*((volatile unsigned long *)0xE000E100))
#define NVIC_PRI7_R             (*((volatile unsigned long *)0xE000E41C))
#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_IS_R         (*((volatile unsigned long *)0x40025404))
#define GPIO_PORTF_IBE_R        (*((volatile unsigned long *)0x40025408))
#define GPIO_PORTF_IEV_R        (*((volatile unsigned long *)0x4002540C))
#define GPIO_PORTF_IM_R         (*((volatile unsigned long *)0x40025410))
#define GPIO_PORTF_RIS_R        (*((volatile unsigned long *)0x40025414))
#define GPIO_PORTF_ICR_R        (*((volatile unsigned long *)0x4002541C))
#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_PUR_R        (*((volatile unsigned long *)0x40025510))
#define GPIO_PORTF_PDR_R        (*((volatile unsigned long *)0x40025514))
#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *)0x4002552C))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define SYSCTL_RCGC2_GPIOF      0x00000020
#define GPIO_PORTF_LOCK_R       (*((volatile unsigned long *)0x40025520))
#define GPIO_PORTF_CR_R         (*((volatile unsigned long *)0x40025524))
	
#define GPIO_PORTA_DATA_R				(*((volatile unsigned long *)0x400043FC))
#define NVIC_PRI0_R 						(*((volatile unsigned long *)0xE000E400))
#define GPIO_PORTA_DIR_R        (*((volatile unsigned long *)0x40004400))
#define GPIO_PORTA_IS_R					(*((volatile unsigned long *)0x40004404))
#define GPIO_PORTA_IBE_R				(*((volatile unsigned long *)0x40004408))
#define GPIO_PORTA_IEV_R				(*((volatile unsigned long *)0x4000440C))
#define GPIO_PORTA_IM_R					(*((volatile unsigned long *)0x40004410))
#define GPIO_PORTA_RIS_R				(*((volatile unsigned long *)0x40004414))
#define GPIO_PORTA_ICR_R				(*((volatile unsigned long *)0x4000441C))
#define GPIO_PORTA_AFSEL_R			(*((volatile unsigned long *)0x40004420))
#define GPIO_PORTA_AMSEL_R			(*((volatile unsigned long *)0x40004528))
#define GPIO_PORTA_PCTL_R				(*((volatile unsigned long *)0x4000452C))
#define GPIO_PORTA_DEN_R				(*((volatile unsigned long *)0x4000451C))
	
#define NVIC_SYS_PRI3_R         (*((volatile unsigned long *)0xE000ED20))  
#define NVIC_ST_CTRL_R          (*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R        (*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R       (*((volatile unsigned long *)0xE000E018))

unsigned int flag = 0;
unsigned int previous = 0;
unsigned int stop = 0;
unsigned int btn1 = 0;
unsigned int btn2 = 0;
unsigned int go = 1;
unsigned int i = 0;

// ***** 3. Function Prototypes *****
void PortF_Init(void);
void GPIOPortF_Handler(void);
void PortA_Init(void);
void EnableInterrupts(void);
void WaitForInterrupt(void);
#define time 3000 

// ***** 4. main function  *****
int main( void )
{  
	// PORT INITILIZATIONS
	PortF_Init();
	PortA_Init();
	EnableInterrupts();
  
	// SUPER LOOP
	while ( 1 ) 
	{
		// ********* SWITCH ONE PROCESSES ********* //
		if ( btn1 == 1 )
		{
      if( flag == 1 )
      { 
        if ( go == 1 ) // Go straight forward
        {
          Stepper_Forward(10*time);
          if( stop == 1 ) // if object is detected
          {
            SysTick_Wait1ms(5);
            go = 0;
          }
        } 
        if ( previous == 1 ) // if the object is removed
        {
          SysTick_Wait1ms(100*15);
          for ( i = 0; i < 2500; i++ )
          {
            Stepper_Forward(10*time);
          }
          SysTick_Wait1ms(5);
          previous = 0;
          btn1 = 0;
        }
      }
    }	
    
    // ********  SWITCH TWO PROCESSES ********* //
    if ( btn2 == 1 )
    {
        // REVERSE 360 DEGREES
        for( i = 0; i < 2500; i++ )
          {
            Stepper_Back(10*time);
          }
        SysTick_Wait1ms(5);
        btn2 = 0;
    }	
  }
}

// ***** 5. Function Definition  *****
void GPIOPortF_Handler(void)
{ // PortF is for the switches
	if((GPIO_PORTF_RIS_R & 0x10)) // If switch 1 is pressed
		{ 
			btn1 = 1;
			flag = 1;
			btn2 = 0;			
		  GPIO_PORTF_ICR_R = 0x10;
		}

	if((GPIO_PORTF_RIS_R & 0x01)) // if switch 2 is pressed
		{ 
			btn2 = 1;
			btn1 = 0;
			flag = 0;
	    GPIO_PORTF_ICR_R = 0x01;
		}
}

void GPIOPortA_Handler(void) 
{ // PortA, pin 7 is for the IR sensor
	GPIO_PORTA_ICR_R = 0x80;
	if ( go == 1 ) // detected an object
		{ 
			stop = 1;
			previous = 0;
		}
  
	if ( go == 0 ) // no object detected
		{ 
			go = 0;
			previous = 1;
			stop = 0;
		}
}		

void PortF_Init(void)
{ 
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) PORTF clock
	delay = SYSCTL_RCGC2_R;
	GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) PortF unlock
	GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0
	GPIO_PORTF_DIR_R = 0x0E;          // 5)PF3,PF2,PF1 output   
  GPIO_PORTF_AFSEL_R &= ~0x1F;      // 6) no alternate function
  GPIO_PORTF_DEN_R |= 0x1F;         // enable digital I/O on PF4  
  GPIO_PORTF_PCTL_R &= ~0x000FFFFF; // configure PF4 as GPIO
  GPIO_PORTF_AMSEL_R = 0;           // 3) disable analog function
  GPIO_PORTF_PUR_R |= 0x11;         // enable weak pull-up on PF4
  GPIO_PORTF_IS_R &= ~0x11;         // PF4 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x11;        // PF4 is not both edges
  GPIO_PORTF_IEV_R &= ~0x11;        // PF4 falling edge event
  GPIO_PORTF_ICR_R = 0x11;          // clear flag4
  GPIO_PORTF_IM_R |= 0x11;          // arm interrupt on PF4
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // priority 5
  NVIC_EN0_R |= 0x40000000;         // enable interrupt 30 in NVIC
}

void PortA_Init(void)
{
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000001;
	delay = SYSCTL_RCGC2_R;
	GPIO_PORTA_DIR_R &= ~0x80; // Input, PA7
	GPIO_PORTA_AFSEL_R &= ~0xFF;
	GPIO_PORTA_DEN_R |= 0x80;
	GPIO_PORTA_PCTL_R &= ~0xF0000000;
	GPIO_PORTA_AMSEL_R = 0;
	GPIO_PORTA_IS_R &= ~0x80;
	GPIO_PORTA_IBE_R |= 0x80; // Both edges
	GPIO_PORTA_ICR_R = 0x80;  
	GPIO_PORTA_IM_R |= 0x80;
	NVIC_PRI0_R = (NVIC_PRI0_R&0xFFFFFF00) | 0x00000080; //priority 4
	NVIC_EN0_R |= 0x00000001;
}
