// ----- 1. Include Section -----
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "Sound.h"
#include "ButtonLed.h"

// ----- 2. Function Prototypes -----
extern void DisableInterrupts(void);
extern void EnableInterrupts(void);
extern void WaitForInterrupt(void);

// ----- 3. Main function -----
int main(void){
  DisableInterrupts();    
  DAC_Init();        // Port B 
	SysTick_Init();		 // SysTick
	ButtonLed_Init();  // Port F
  PianoKeys_Init();  // Port D
	EnableInterrupts();
	
	while(1){
		if(in_piano_mode()){  // If we are in piano mode	
			WaitForInterrupt(); // Wait for Interrups
			play_a_note();      // Call the function play a note that is responsible for the key playing when a button is pressed
		}
		else{                 // If we are not in piano mode
			play_a_song();      // auto play the 3 pre selected songs
		}
	}
}
