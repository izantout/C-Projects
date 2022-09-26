// Timer1A.c
// Runs on LM4F120/TM4C123
// Use TIMER1 in 32-bit periodic mode to request interrupts at a periodic rate

#include "tm4c123gh6pm.h"
#include "Timer1A.h"

// ***************** Timer1_Init ****************
// Activate TIMER1 interrupts to run user task periodically
// Inputs: period in units (1/clockfreq)
// Outputs: none
void Timer1A_Init(unsigned long period){
  SYSCTL_RCGCTIMER_R |= 0x02;   // 0) activate TIMER1A
  TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER1A during setup
  TIMER1_CFG_R = 0x00000000;    // 2) configure for 32-bit mode todo
  TIMER1_TAMR_R = 0x00000002;   // 3) configure for periodic down-count mode
  TIMER1_TAILR_R = period-1;    // 4) reload value
  TIMER1_TAPR_R = 0;            // 5) bus clock prescale
  TIMER1_ICR_R = 0x00000001;    // 6) clear TIMER1A timeout flag
  TIMER1_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF1FFF)|0x00006000; // 8) NVIC_PRI5_R bits 13-15
// interrupts enabled in the main program after all devices initialized
// vector number 37, interrupt number 21 NVIC_EN0_R since 21<32
  NVIC_EN0_R = 1<<21;           // 9) enable IRQ 21 in NVIC
  TIMER1_CTL_R = 0x00000001;    // 10) enable TIMER1A
}

void Timer1A_Handler(void){
  TIMER1_ICR_R = TIMER_ICR_TATOCINT; // acknowledge TIMER1A timeout
}

// NVIC_EN0_R controls IRQ 0->31
// NVIC_EN1_R controls IRQ 32->63
// NVIC_EN2_R controls IRQ 64->95
// NVIC_EN3_R controls IRQ 96->127
// NVIC_EN4_R controls IRQ 128->138
