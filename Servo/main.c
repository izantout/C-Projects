/*
Pin 1 GND
Pin 2 5V
VO Potentiometer
Button 1 Pin 0.3
Servo Pin 0.2
RS Pin 0.7
RW Pin 0.6
E Pin 0.5
D0 Pin 1.0
D1 Pin 1.1
D2 Pin 1.2
D3 Pin 1.3
D4 Pin 1.4
D5 Pin 1.5
D6 Pin 1.6
D7 Pin 1.7

Button 0 is on the MCU, used for increase
Button 1 is on the breadboard, used for decrease
*/
#include <SI_EFM8BB1_Register_Enums.h>
void InitDevice(void)
{
//Disable Watchdog Timer
//Disable Watchdog with key sequence
WDTCN = 0xDE; //First key
WDTCN = 0xAD; //Second key
// [WDTCN – Watchdog Timer Control]$

//Setup Defaults for P1
P1MDOUT = P1MDOUT_B0__PUSH_PULL | P1MDOUT_B1__PUSH_PULL | P1MDOUT_B2__PUSH_PULL 
| P1MDOUT_B3__PUSH_PULL | P1MDOUT_B4__PUSH_PULL 
| P1MDOUT_B5__PUSH_PULL | P1MDOUT_B6__PUSH_PULL | P1MDOUT_B7__PUSH_PULL;

//Setup Defaults for P0
P0MDOUT = P0MDOUT_B0__PUSH_PULL | P0MDOUT_B1__PUSH_PULL
| P0MDOUT_B2__PUSH_PULL | P0MDOUT_B3__PUSH_PULL
| P0MDOUT_B4__PUSH_PULL | P0MDOUT_B5__PUSH_PULL
| P0MDOUT_B6__PUSH_PULL | P0MDOUT_B7__PUSH_PULL;
	
//Enable Weak Pullups
XBR2 = XBR2_WEAKPUD__PULL_UPS_ENABLED | XBR2_XBARE__ENABLED;

//Disable Interrupts
IE = IE_EA__DISABLED | IE_EX0__DISABLED | IE_EX1__DISABLED
| IE_ESPI0__DISABLED | IE_ET0__DISABLED | IE_ET1__DISABLED
| IE_ET2__ENABLED | IE_ES0__DISABLED;
}

unsigned int valueTHTL;
unsigned char angle;	

void T0_ISR(void) interrupt 1
	{
		unsigned int value;
		P0 = P0 | (0x01); // Set signal Pin
		
		value = 12;
		
		TCON = TCON &~ (0x01 << 4); // CLEAR TR0
		TL0 = 0x10; // Having th0 and tl0 for 20 ms
		TH0 = 0xEC;
		TCON = TCON | (0x01 << 4); // Starting Timer 0
		
		
		TCON = TCON &~ (0x01 << 6); // Clearing Timer 1
		valueTHTL = 0xFF01 - (value * angle);
		TL1 = (unsigned char)((valueTHTL) & 0x00FF);
		TH1 = (unsigned char)((valueTHTL>>8) & 0x00FF); // Having TL1 and TH1 variables depending on the angle
		TCON = TCON | (0x01 << 6); // Starting Timer 1
		
		
		while(!TCON & (0x01 << 7)); // Waiting until TF1 == 1
		TCON = TCON &~ (0x01 << 6); // Clearing Timer 1
		TCON = TCON &~ (0x01 << 6); // Clearing TF1
		
		P0 = P0 &~ (0x01); // Clearing Signal Pin
	}
	
void main (void)
	{
		InitDevice();
		angle = 10;
		valueTHTL = 0;
		IE = 0; // Enable Timer 0 Interrupt
		TMOD = 0x11; // 16 BIT TIMER MODS
		TH0 = 0xEC;
		TL0 = 0x10;
		TCON = TCON | (0x01 << 4); // Start Timer 0
		while(1)
		{
			if (!(P0&(0x01 << 2))) // If MCU Button is pushed
			{
				angle = angle + 1;
				if (angle > 19) // Check if angle is greater than 19
				{
					angle = 19; // Set angle to maximum of 19
				}
				while (!(P0&(0x01 << 2)));
			}
			else if (!(P0&(0x01 << 3)))
			{
				angle = angle - 1;
				if (angle < 0) // Check if angle less than o
				{
					angle = 0; // Set angle to minimum 0 
				}
				while (!(P0&(0x01 << 3)));
			}
			else
			{
				angle = angle;
			}
		}
	}
