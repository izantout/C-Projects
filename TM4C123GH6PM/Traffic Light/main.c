// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

void delay10ms(int delay)
{

    unsigned long volatile time;

    time = (727240*200/91) * delay;  // 0.1sec * delay

    while(time){

        time--;

  }
}
void portInit(void)
    {
    volatile unsigned long delay;
        
    SYSCTL_RCGC2_R |= 0x00000011; // 1) enable both A and E clock
    delay = SYSCTL_RCGC2_R; // delay                                            // PORT A
    GPIO_PORTA_DIR_R = 0x00; // 5) PA2,PA3 input
    GPIO_PORTA_DEN_R = 0x0C; // 8) enable digital pins PF4-PF0
        
    GPIO_PORTE_DIR_R = 0x3F; // PE2,PE1,PE0 output
    GPIO_PORTE_DEN_R = 0x3F; // 8) enable digital pins PF4-PF0
    }

#define SW1 (*((volatile unsigned long *)0x40004010))  // PA2
#define SW2 (*((volatile unsigned long *)0x40004020))  // PA3	
#define LED6 (*((volatile unsigned long *)0x40024080)) // PE5
#define LED5 (*((volatile unsigned long *)0x40024040)) // PE4
#define LED4 (*((volatile unsigned long *)0x40024020)) // PE3
#define LED3 (*((volatile unsigned long *)0x40024010)) // PE2
#define LED2 (*((volatile unsigned long *)0x40024008)) // PE1
#define LED1 (*((volatile unsigned long *)0x40024004)) // PE0
    
// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void delay10ms(int);

// ***** 3. Subroutines Section *****

struct State {
unsigned long Out;
unsigned long Time; // 10 ms units
unsigned long Next[4]; // list of next states
};
typedef const struct State STyp;
#define goN 0 //00
#define waitN 1
#define goE 2
#define waitE 3 //03

STyp FSM[4] = {
{0x21,3000,{goN,waitN,goN,waitN}},
{0x22, 500,{goE,goE,goE,goE}},
{0x0C,3000,{goE,goE,waitE,waitE}},
{0x14, 500,{goN,goN,goN,goN}}
};

int main(void){ 
   unsigned long CS; // index of current state
   unsigned long Input;
   TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210,ScopeOff); // activate grader and set system clock to 80 MHz
   EnableInterrupts();
   portInit();

// initialize ports and timer
    CS = goN; // initial state
 

  while(1){ 
    GPIO_PORTE_DATA_R = ~FSM[CS].Out; // set signals
    //delay10ms(FSM[CS].Time);
    delay10ms(5);
    Input = (~GPIO_PORTA_DATA_R&(0x0C)) >> 2; // read sensors
    CS = FSM[CS].Next[Input];
    }
        
}
