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
  unsigned char s,x;
  unsigned char chng, count;

  InitDevice();
  TMOD = 0x01; //Timer 0 is in 16bit Mode
  TL0 = 0x00; // Initializing TH0
															// when Overflow happens 71ms occure
  TH0 = 0x00; // Initializing TL0
  TCON = TCON | (0x01 << 4); //TR0 = 1 starting Timer 0 
  chng = 0x01;
  count = 0;
  x=7;
	
	while(1) //Infinite while loop, i.e. Superloop
	{
		for (s=0;s<7;s++)
		{
			if (TCON & (0x01 << 5)) // Check if TF0 = 1 ... If 1 Overflow happened.
			{
				TCON = TCON &~(0x01 << 4); // Stopping the timer TR0 = 0
				TL0 = 0x00;
				TH0 = 0x00;
				TCON = TCON &~(0x01 << 5); // Clearing the flag TF0 = 0
				TCON = TCON | (0x01 << 4); // Start the timer again
				count = count+1;
			}
			if (~P0 & 0x04) // if button 0 is pushed
			{
				if (count == 2)
				{ 
					chng = chng << 1; //Left shift
					if (P0 & 0x02) // If button 1 is not pushed (button 1 is the added button on the breadboard)
					{
						P1 = ~chng;
					}
					else
					{
						P1 = chng;
					}
					count = 0; //reset count
					x = x-1;
				}
				else
				{
					chng = chng;
					if (~P0 & 0x02) // If button 1 is pushed (button 1 is the added button on the breadboard)
					{
						P1 = ~chng;
					}
					else
					{
						P1 = chng;
					}
				}
			}
			else
			{
				if (count == 5) // Button is not pushed
				{
					chng = chng << 1; //Left shift
					if (~P0 & 0x02) // If button 1 is pushed (button 1 is the added button on the breadboard)
					{
						P1 = ~chng;
					}
					else
					{
						P1 = chng;
					}
					count = 0; //reset count
					x = x-1;
				}
				else
				{
					chng = chng;
					if (~P0 & 0x02) // If button 1 is pushed (button 1 is the added button on the breadboard)
					{
						P1 = ~chng;
					}
					else
					{
						P1 = chng;
					}
				}
			}	
			if (x < 1 & x > -1)
			{
				for (s=0;x<7;s++)
				{
					if (TCON & (0x01 << 5)) // Check if TF0 = 1 ... If 1 Overflow happened.
					{
						TCON = TCON &~(0x01 << 4); // Stopping the timer TR0 = 0
						TL0 = 0x00;
						TH0 = 0x00;
						TCON = TCON &~(0x01 << 5); // Clearing the flag TF0 = 0
						TCON = TCON | (0x01 << 4); // Start the timer again
						count = count+1;
					}
					if (~P0 & 0x04) // if button is pushed
					{
						if (count == 2)
						{ 
							chng = chng >> 1; //Right shift
							if (~P0 & 0x02) // If button 1 is pushed (button 1 is the added button on the breadboard)
							{
								P1 = ~chng;
							}
							else
							{
								P1 = chng;
							}
							count = 0; //reset count
							x = x+1;
						}
						else
						{
							chng = chng;
							if (~P0 & 0x02) // If button 1 is pushed (button 1 is the added button on the breadboard)
							{
								P1 = ~chng;
							}
							else
							{
								P1 = chng;
							}
						}
					}
					else
					{
						if (count == 5) // Button is not pushed
						{
							chng = chng >> 1; //Right shift
							if (~P0 & 0x02) // If button 1 is pushed (button 1 is the added button on the breadboard)
							{
								P1 = ~chng;
							}
							else
							{
								P1 = chng;
							}
							count = 0; //reset count
							x = x+1;
						}
						else
						{
							chng = chng;
							if (~P0 & 0x02) // If button 1 is pushed (button 1 is the added button on the breadboard)
							{
								P1 = ~chng;
							}
							else
							{
								P1 = chng;
							}
						}
					}
				}	
			}	
		}	
	}
}
