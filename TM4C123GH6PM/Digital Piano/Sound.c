// ----- 1. Include Section
#include "tm4c123gh6pm.h"
#include "sound.h"
#include <stdint.h>
#include "ButtonLed.h"

// ----- 2. Variable Decleration
// Assume SysTick clock frequency is 16MHz.
unsigned char Index;	
uint8_t i=0,j=0,k = 0, music_playing=0, piano_mode=1, octave = 3;
const uint8_t SineWave[64] = {32,35,38,41,44,47,49,52,54,56,58,59,61,62,62,63,63,63,62,62,
															61,59,58,56,54,52,49,47,44,41,38,35,32,29,26,23,20,17,15,12,
															10, 8, 6, 5, 3, 2, 2, 1, 1, 1, 2, 2, 3, 5, 6, 8,10,12,15,17,
															20,23,26,29}; // 6-bit: value range 0 to 2^6-1=63, 64 samples
const uint32_t tonetab[] =
// C, D, E, F, G, A, B
// 1, 2, 3, 4, 5, 6, 7
// lower C octave:130.813, 146.832,164.814,174.614,195.998, 220,246.942
// calculate reload value for the whole period:Reload value = Fclk/Ft = 16MHz/Ft
{122137,108844,96970,91429,81633,72727,64777,
 30534*2,27211*2,24242*2,22923*2,20408*2,18182*2,16194*2, // C4 Major notes
 15289*2,13621*2,12135*2,11454*2,10204*2,9091*2,8099*2,   // C5 Major notes
 7645*2,6810*2,6067*2,5727*2,5102*2,4545*2,4050*2};       // C6 Major notes

// ----- 3. Constant Decleration -----
#define C5 0+2*7
#define D5 1+2*7
#define E5 2+2*7
#define F5 3+2*7
#define G5 4+2*7
#define A5 5+2*7
#define B5 6+2*7
#define C6 0+3*7
#define D6 1+3*7
#define E6 2+3*7
#define F6 3+3*7
#define G6 4+3*7
#define A6 5+3*7
#define B6 6+3*7	
#define MAX_NOTES 255 // maximum number of notes for a song to be played in the program
#define NUM_SONGS 3   // number of songs in the play list.
#define SILENCE MAX_NOTES // use the last valid index to indicate a silence note. The song can only have up to 254 notes. 
#define NUM_OCT  3   // number of octave defined in tontab[]
#define NUM_NOTES_PER_OCT 7  // number of notes defined for each octave in tonetab
#define NVIC_EN0_PORTF 0x40000000  // enable PORTF edge interrupt
#define NVIC_EN0_PORTD 0x00000008  // enable PORTD edge interrupt
#define NUM_SAMPLES 64  // number of sample in one sin wave period

// ----- 4. Ntype Section -----
NTyp playlist[][MAX_NOTES] = 
{{//so   so   la   so   doe' ti
   G5,2,G5,2,A5,4,G5,4,C6,4,B5,4,
// pause so   so   la   so   ray' doe'
   SILENCE,4,  G5,2,G5,2,A5,4,G5,4,D6,4,C6,4,
// pause so   so   so'  mi'  doe' ti   la
   SILENCE,4,  G5,2,G5,2,G6,4,E6,4,C6,4,B5,4,A5,8, 
// pause fa'  fa'   mi'  doe' ray' doe' stop
	 SILENCE,4,  F6,2,F6,2, E6,4,C6,4,D6,4,C6,8,0,0},

// Mary Had A Little Lamb
{E5, 4, D5, 4, C5, 4, D5, 4, E5, 4, E5, 4, E5, 8, 
 D5, 4, D5, 4, D5, 8, E5, 4, G5, 4, G5, 8,
 E5, 4, D5, 4, C5, 4, D5, 4, E5, 4, E5, 4, E5, 8, 
 D5, 4, D5, 4, E5, 4, D5, 4, C5, 8, 0, 0 },

// Twinkle Twinkle Little Stars
{C5,4,C5,4,G5,4,G5,4,A5,4,A5,4,G5,8,F5,4,F5,4,E5,4,E5,4,D5,4,D5,4,C5,8, 
 G5,4,G5,4,F5,4,F5,4,E5,4,E5,4,D5,8,G5,4,G5,4,F5,4,F5,4,E5,4,E5,4,D5,8, 
 C5,4,C5,4,G5,4,G5,4,A5,4,A5,4,G5,8,F5,4,F5,4,E5,4,E5,4,D5,4,D5,4,C5,8,0,0}
};
// ----- 5. Function Decleration -----
// **************DAC_Init*********************
// Initialize 6-bit DAC 
// Input: none
// Output: none
void DAC_Init(void){ unsigned long volatile delay;
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB; // activate port B
  delay = SYSCTL_RCGC2_R;               // allow time to finish activating
  GPIO_PORTB_AMSEL_R &= ~0x3F;          // no analog 
  GPIO_PORTB_PCTL_R &= ~0x00FFFFFF;     // regular function
  GPIO_PORTB_DIR_R |= 0x3F;             // make PB5-0 out
  GPIO_PORTB_AFSEL_R &= ~0x3F;          // disable alt funct on PB5-0
  GPIO_PORTB_DEN_R |= 0x3F;             // enable digital I/O on PB5-0
  GPIO_PORTB_DR8R_R |= 0x3F;            // enable 8 mA drive on PB5-0
}

// ************** SysTick_Init *********************
void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;                   											// disable SysTick during setup
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000; 	// priority 2
	NVIC_ST_CTRL_R |= NVIC_ST_CTRL_CLK_SRC+NVIC_ST_CTRL_INTEN;
}

// ************** SysTick_Handler *********************
void SysTick_Handler(void){
	  GPIO_PORTF_DATA_R ^= 0x08;         // toggle PF3, debugging
  Index = (Index+1)&0x3F;              // 16 samples for each period
	GPIO_PORTB_DATA_R = SineWave[Index]; // output to DAC: 3-bit data
}

// ************** SysTick_start *********************
void SysTick_start(void){
	NVIC_ST_CTRL_R |= NVIC_ST_CTRL_ENABLE;
}

// ************** SysTick_stop *********************
void SysTick_stop(void){
	NVIC_ST_CTRL_R &= ~NVIC_ST_CTRL_ENABLE;
}

// ************** SysTick_Set_Current_Note *********************
void SysTick_Set_Current_Note(unsigned long n_value){
  NVIC_ST_RELOAD_R = n_value-1;				// update reload value
  NVIC_ST_CURRENT_R = 0;              // any write to current clears it
}

// ************** Delay to wait 0.1s *********************
void Delay(void){
	uint32_t volatile time;
  time = 727240*20/91;  // 0.1sec
  while(time){
		time--;
  }
}

// ************** is_music_on *********************
uint8_t is_music_on(void){ // Checks if music is on
  return music_playing;
}

// ************** turn_off_music *********************
void turn_off_music(void){ // Turns off the music
	music_playing = 0;
}

// ************** turn_on_music *********************
void turn_on_music(void){ // Turns on the music
	k=0, i=0,j=0;
	music_playing = 1;
}

// ************** piano_mode_toggle *********************
void piano_mode_toggle(void){ // Turns piano mode on and off
	piano_mode ^= 0x01;
	if(piano_mode){
		turn_off_music();
	}
	else{
		turn_on_music();
	}
}

// ************** in_piano_mode *********************
uint8_t in_piano_mode(void){ // Checks if we are in piano mode
	return piano_mode;
}

// ************** play_a_note *********************
void play_a_note(){ // Function that is responsibel to play a sound when a button is pressed
	if(in_piano_mode()){
		uint8_t f = 0x01;
		while(noteC_playing()){ // While we are pressing the button for note C
			if(f){
				SysTick_Set_Current_Note(tonetab[0+((octave-2)*7)]/NUM_SAMPLES);
				SysTick_start();
				f = 0x00;
			}
		}
		
		while(noteD_playing()){ // While we are pressing the button for note D
			if(f){
				SysTick_Set_Current_Note(tonetab[1+((octave-2)*7)]/NUM_SAMPLES);
				SysTick_start();
				f = 0x00;
			}
		}
		
		while(noteE_playing()){ // While we are pressing the button for note E
			if(f){
				SysTick_Set_Current_Note(tonetab[2+((octave-2)*7)]/NUM_SAMPLES);
				SysTick_start();
				f = 0x00;
			}
		}
		
		while(noteF_playing()){ // While we are pressing the button for note F
			if(f){
				SysTick_Set_Current_Note(tonetab[3+((octave-2)*7)]/NUM_SAMPLES);
				SysTick_start();
				f = 0x00;
			}
		}
		SysTick_stop();
	}
}

// **************  next_octave *********************
void next_octave(void){// function responsible to go to next octave
	octave = (octave+1)%3+3;
}

// ************** tone_Index*********************
uint8_t tone_Index(void){ // plays the correct octave of the same note
	if(octave == 3){
		return playlist[k][i].tone_index-2*7;
	}
	else if(octave == 4){
		return playlist[k][i].tone_index-1*7;
	}
	else if(octave == 5){
		return playlist[k][i].tone_index-0*7;
	}
	else{
		return playlist[k][i].tone_index+1*7;
	}
}

// ************** play_a_song *********************
void play_a_song(void){
	i=0, j=0;
	
	while(playlist[k][i].delay){
		if(is_music_on()){
			if(playlist[k][i].tone_index == SILENCE){ // If its a silence note
				SysTick_stop(); // Stop timer
			}
			else{
				SysTick_Set_Current_Note(tonetab[tone_Index()]/NUM_SAMPLES); // Else
				SysTick_start(); // Start the timer
			}
			for(j=0;j<playlist[k][i].delay;j++){
				Delay();
			}
			SysTick_stop();
			i++;
		}
		else{
			break;
		}
	}
	for(j=0;j<15;j++){ // Pausing After each song
		Delay();
	}
}

// ************** next_song *********************
void next_song(void){
	k = (k+1)%3;
	SysTick_stop();
	i=0,j=0;
	for(j=0;j<10;j++){
		Delay();
	}
}
