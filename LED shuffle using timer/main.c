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
unsigned char x,s;
	unsigned char chng;

InitDevice();
P0 = P0 | (0x01 << 2); //initializing P0.2(button 0) to be 1 ... open
TMOD = 0x01; //Timer 0 is in 16bit Mode
TH0 = 0x00; // Initializing TH0
															// when Overflow happens 71ms occure
TL0 = 0x00; // Initializing TL0
TCON = TCON | (0x01 <<4); //TR0 = 1 starting Timer 0 
chng = 0x01;
	
while(1) //Infinite while loop, i.e. Superloop
{
	if (~(P0 = P0 & (0x01 << 2))) // if button is pressed we need 0.5s per change
		{ 					
				for (s=0;s<7;s++)
				{
						for (x=0;x<8;x++) // Iterate 7 times to result in 497 milliseconds or around 0.5s
						{
							if (TCON & 0x01 << 5) // Check if TF0 = 1 ... If 1 Overflow happened. 
							{
									chng = chng << 1; // Left Shift
									P1 = ~chng;
									TCON = TCON &~(0x01 << 4); // Stopping the timer TR0 = 0
									TCON = TCON &~(0x01 << 5); // Clearing the flag TF0 = 0
									TCON = TCON | (0x01 << 4); // Start the timer again
							}
						
						
							else
							{
								chng = chng; // Keeping the current LED ON
							}
						}
				}
				
				for (s=0;s<7;s++)
				{
						for (x=0;x<8;x++)
					{
							if (TCON & 0x01 << 5) // Check if TF0 = 1 ... If 1 Overflow happened. 
							{
									chng = chng >> 1; // Right Shift
									P1 = ~chng;
									TCON = TCON &~(0x01 << 4); // Stopping the timer TR0 = 0
									TCON = TCON &~(0x01 << 5); // Clearing the flag TF0 = 0
									TCON = TCON | (0x01 << 4); // Start the timer again
							}
					
						
							else
							{
								chng = chng; // Keeping the current LED ON
							}
					}	
				}				

		}
	else
		{
 
				for (s=0;s<7;s++)
				{
						if (TCON & 0x01 << 5) // Check if TF0 = 1 ... If 1 Overflow happened. 
						{
								chng = chng << 1; // Left Shift
								P1 = ~chng;
								TCON = TCON &~(0x01 << 4); // Stopping the timer TR0 = 0
								TCON = TCON &~(0x01 << 5); // Clearing the flag TF0 = 0
								TCON = TCON | (0x01 << 4); // Start the timer again
						}
						
						else
						{
							chng = chng; // Keeping the current LED ON
						}
						
				}
				
				for (s=0;s<7;s++)
				{
						if (TCON & 0x01 << 5) // Check if TF0 = 1 ... If 1 Overflow happened. 
						{
								chng = chng >> 1; // Right Shift
								P1 = ~chng;
								TCON = TCON &~(0x01 << 4); // Stopping the timer TR0 = 0
								TCON = TCON &~(0x01 << 5); // Clearing the flag TF0 = 0
								TCON = TCON | (0x01 << 4); // Start the timer again
						}
						
						else
						{
							chng = chng; // Keeping the current LED ON
						}
						
				}
				
		}
		
}

}
