//main.c

#include <SI_EFM8BB1_Register_Enums.h>

void InitDevice(void)
{
//Disable Watchdog Timer
//Disable Watchdog with key sequence
WDTCN = 0xDE; //First key
WDTCN = 0xAD; //Second key
// [WDTCN – Watchdog Timer Control]$

//Setup Defaults for P1
P1MDOUT = P1MDOUT_B0__OPEN_DRAIN | P1MDOUT_B1__OPEN_DRAIN
| P1MDOUT_B2__OPEN_DRAIN | P1MDOUT_B3__OPEN_DRAIN
| P1MDOUT_B4__PUSH_PULL | P1MDOUT_B5__OPEN_DRAIN
| P1MDOUT_B6__OPEN_DRAIN | P1MDOUT_B7__OPEN_DRAIN;

//Enable Weak Pullups
XBR2 = XBR2_WEAKPUD__PULL_UPS_ENABLED | XBR2_XBARE__ENABLED;

//Disable Interrupts
IE = IE_EA__DISABLED | IE_EX0__DISABLED | IE_EX1__DISABLED
| IE_ESPI0__DISABLED | IE_ET0__DISABLED | IE_ET1__DISABLED
| IE_ET2__ENABLED | IE_ES0__DISABLED;

}

main(void)
{
unsigned char x,y,s,z;
	unsigned char chng;

InitDevice();
chng = 0x01;
while(1) //Infinite while loop, i.e. Superloop
{	

for (s=0;s<7;s++){ // Iterate 7 times to left shift 7 times
	chng = chng<<1;
	P1 = ~chng;
	//left
	
if(~P0 & 0x04){ // If button is pushed
		
		for (x = 0; x < 0xFF; x++){
			for(y = 0; y < 0x96; y++){
				}}
	}else{ // Button is not pushed
		for (x = 0; x < 0xFF; x++){
			for(y = 0; y < 0xFF; y++){
				for(z=0;z<0x01;z++){
				}}}
		}
	}
for (s=0;s<7;s++){ // Iterate 7 times to right shift 7 times
	chng = chng>>1;
	P1 = ~chng;
	//right
	
if(~P0 & 0x04){ // If button is pushed
		
		for (x = 0; x < 0xFF; x++){
			for(y = 0; y < 0x96; y++){			
				}}
	}else{ // Button is not pushed
		for (x = 0; x < 0xFF; x++){
			for(y = 0; y < 0xFF; y++){
				for(z=0;z<0x01;z++){
				}}}
		}
	}}

}
