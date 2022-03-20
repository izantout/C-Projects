#include <stdint.h>           // C99 data types
#include "tm4c123gh6pm.h"

// Function Prototypes (from startup.s)
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // Go to low power mode while waiting for the next interrupt

// Function Prototypes
void EdgeCounter_Init(void);  // Initialize edge trigger interrupt for PF0 (SW2) rising edge
void PortF_LEDInit(void);     // Initialize Port F LEDs
void SysTick_Init(void);      // Initialize SysTick timer for 0.1s delay with interrupt enabled
void SysTick_Wait1ms(unsigned long delay);
void SysTick_Wait(unsigned long delay);

void GPIOPortF_Handler(void); // Handle GPIO Port F interrupts
void SysTick_Handler(void);   // Handle SysTick generated interrupts

// global variable visible in Watch and Memory window of debugger
// increments at least once per button release
volatile uint32_t RisingEdges = 0;
volatile uint32_t count = 0;

int main(void){
  PortF_LEDInit();
	EdgeCounter_Init();           // initialize GPIO Port F interrupt
	SysTick_Init();
	// initialize LaunchPad LEDs to red
	GPIO_PORTF_DATA_R = (GPIO_PORTF_DATA_R & ~0x0E) + 0x02;

  while(1){
		WaitForInterrupt();
  }
}

// Color    LED(s) PortF
// red      R--    0x02
// blue     --B    0x04
// green    -G-    0x08

// Initialize Port F LEDs
void PortF_LEDInit(void) {          //LEDs are PF1(red) & PF2(blue)
	volatile unsigned long delay; 	  //clock for F
	SYSCTL_RCGC2_R |= 0x00000020;
	delay = SYSCTL_RCGC2_R; //run delay
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) Unlock PortF PF0
  GPIO_PORTF_CR_R = 0x1F;           // Allow changes to PF4-0
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) Disable analog function
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL
  GPIO_PORTF_DIR_R = 0x06;          // 5) PF2,PF1 output
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) No alternate function
  GPIO_PORTF_PUR_R = 0x11;          // Enable pullup resistors on PF4,PF0
  GPIO_PORTF_DEN_R = 0x06;          // 7) Enable digital pins PF1,2
  
}

// Initialize SysTick timer for 0.1s delay with interrupt enabled
void SysTick_Init(void) {
	NVIC_ST_CTRL_R = 0;                                         // Disable SysTick during setup
  NVIC_ST_RELOAD_R = 1600000;                                 // 10 ms reload value
  NVIC_ST_CURRENT_R = 0;                                      // Any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000;  // Enable SysTick with core clock and interrupts
  NVIC_ST_CTRL_R = 0x07;                                      // 1 to system clock, Interrupt enable, and systick enable
	EnableInterrupts();
	
	

}

// Initialize edge trigger interrupt for PF0 (SW2) rising edge
void EdgeCounter_Init(void) {
	RisingEdges = 0;
	GPIO_PORTF_IS_R &= ~0x01;  // PF0 is edge sensitive
	GPIO_PORTF_IBE_R &= ~0x01; // PF0 is not both edges
	GPIO_PORTF_IEV_R |= 0x01;  // PF0 is on rising edge
	GPIO_PORTF_ICR_R = 0x01;   // Clearing flag0
	GPIO_PORTF_IM_R |= 0x01;   // Arming Interrupt on PF0
	NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF) | 0x00A00000;
	NVIC_EN0_R = 0x40000000;   // Enables interrupt 30 in NVIC
	GPIO_PORTF_DEN_R |= 0x01;
	EnableInterrupts();        // Clears the I bit
	
	

}

// Handle GPIO Port F interrupts. When Port F interrupt triggers, do what's necessary then increment global variable RisingEdges
void GPIOPortF_Handler(void) {
	GPIO_PORTF_ICR_R = 0x01;    // Acknowledge flag0
	RisingEdges = RisingEdges + 1;
	

}

// Handle SysTick generated interrupts. When timer interrupt triggers, do what's necessary then toggle red and blue LEDs at the same time
void SysTick_Handler(void) {
	GPIO_PORTF_DATA_R ^= 0x06; // Toggles red and blue LEDs
	count = count + 1;
}
