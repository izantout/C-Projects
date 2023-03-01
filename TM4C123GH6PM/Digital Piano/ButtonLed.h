// ButtonLed.h
#include <stdint.h>

#define PIANO     0  // piano mode: press a key to play a tone
#define AUTO_PLAY 1  // auto play mode: automatic playing a song
void ButtonLed_Init(void);
void PianoKeys_Init(void);
uint8_t noteC_playing(void);
uint8_t noteD_playing(void);
uint8_t noteE_playing(void);
uint8_t noteF_playing(void);
uint8_t get_current_mode(void);
void noteStop(void);
