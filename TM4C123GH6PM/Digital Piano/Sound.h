// Sound.h
#include <stdint.h>

struct Note { // Music note Struct
  uint32_t tone_index;
  uint8_t delay;
};
typedef const struct Note NTyp;
void DAC_Init(void);
void Sound_Start(uint32_t period);
void Sound_stop(void);
void Delay(void);
void SysTick_Init(void);
void SysTick_start(void);
void SysTick_stop(void);
void SysTick_Set_Current_Note(unsigned long n_value);
void turn_off_music(void);
void turn_on_music(void);
uint8_t is_music_on(void);
uint8_t in_piano_mode(void);
void piano_mode_toggle(void);
void play_a_note();
void next_octave(void);
uint8_t tone_Index(void);
void play_a_song(void);
void next_song(void);
