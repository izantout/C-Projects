#ifndef __TIMER0INTS_H__ // do not include more than once
#define __TIMER0INTS_H__

// ***************** Timer1_Init ****************
// Activate Timer1 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq)
// Outputs: none
void Timer1A_Init(unsigned long period);

#endif // __TIMER2INTS_H__
