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

//LCD INITIATION
void pause(void)
	{
	unsigned char dly1, dly2;
	for (dly1 = 0; dly1 < 0xFF; dly1++)
		{
			for (dly2 = 0; dly2 < 0x01; dly2++)
			{
			}
		}
	}
	
//Write Data	
void LCD_Data_Write (unsigned char *datawrite)
	{
		P0 = P0 | (0x01 << 7); // RS = 1
		P0 = P0 &~ (0x01 << 6); // RW = 0
		P1 = *datawrite;
		P0 = P0 | (0x01 << 5); // E = 1
		pause();
		P0 = P0 &~ (0x01 << 5); // E = 0
	}
	
//Write CMD
void LCD_Command_Write (unsigned char cmd)
	{
		P0 = P0 &~ (0x01 << 7); //RS = 0
		P0 = P0 &~ (0x01 << 6); //RW = 0
		P1 = cmd; //Set D0-D7 
		P0 = P0 | (0x01 << 5); // E = 1
		pause();
		P0 = P0 &~ (0x01 << 5); // E = 0
	}

//Init LCD	
void Init_LCD (void)
	{
		LCD_Command_Write(0x38); // 8bit interface, 2 Lines, 5x7 font
		LCD_Command_Write(0x06); // Autoincrement, no display shift
		LCD_Command_Write(0x0E); // Display On, Cursor On, no blinking
		LCD_Command_Write(0x01); // Clear display, Set cursor to 0
	}	
//END OF LCD INITIATION

void LCD_Conditions (void)
{
	Init_LCD();
	if ( angle == 1)
	{
		LCD_Data_Write("A");
		LCD_Data_Write("N");
		LCD_Data_Write("G");
		LCD_Data_Write("E");
		LCD_Data_Write("L");
		LCD_Data_Write(":");
		LCD_Data_Write(" ");
		LCD_Data_Write("-");
		LCD_Data_Write("9");
		LCD_Data_Write("0");
	}
	else if ( angle == 2)
	{
		LCD_Data_Write("A");
		LCD_Data_Write("N");
		LCD_Data_Write("G");
		LCD_Data_Write("E");
		LCD_Data_Write("L");
		LCD_Data_Write(":");
		LCD_Data_Write(" ");
		LCD_Data_Write("-");
		LCD_Data_Write("8");
		LCD_Data_Write("1");
	}
	else if ( angle == 3)
	{
		LCD_Data_Write("A");
		LCD_Data_Write("N");
		LCD_Data_Write("G");
		LCD_Data_Write("E");
		LCD_Data_Write("L");
		LCD_Data_Write(":");
		LCD_Data_Write(" ");
		LCD_Data_Write("-");
		LCD_Data_Write("7");
		LCD_Data_Write("2");
		}
	else if ( angle == 4)
	{
		LCD_Data_Write("A");
		LCD_Data_Write("N");
		LCD_Data_Write("G");
		LCD_Data_Write("E");
		LCD_Data_Write("L");
		LCD_Data_Write(":");
		LCD_Data_Write(" ");
		LCD_Data_Write("-");
		LCD_Data_Write("6");
		LCD_Data_Write("3");
		}
	else if ( angle == 5)
	{
		LCD_Data_Write("A");
		LCD_Data_Write("N");
		LCD_Data_Write("G");
		LCD_Data_Write("E");
		LCD_Data_Write("L");
		LCD_Data_Write(":");
		LCD_Data_Write(" ");
		LCD_Data_Write("-");
		LCD_Data_Write("5");
		LCD_Data_Write("4");
		}
	else if ( angle == 6)
	{
		LCD_Data_Write("A");
		LCD_Data_Write("N");
		LCD_Data_Write("G");
		LCD_Data_Write("E");
		LCD_Data_Write("L");
		LCD_Data_Write(":");
		LCD_Data_Write(" ");
		LCD_Data_Write("-");
		LCD_Data_Write("4");
		LCD_Data_Write("5");
		}
	else if ( angle == 7)
	{
		LCD_Data_Write("A");
		LCD_Data_Write("N");
		LCD_Data_Write("G");
		LCD_Data_Write("E");
		LCD_Data_Write("L");
		LCD_Data_Write(":");
		LCD_Data_Write(" ");
		LCD_Data_Write("-");
		LCD_Data_Write("3");
		LCD_Data_Write("6");
		}
	else if ( angle == 8)
	{
		LCD_Data_Write("A");
		LCD_Data_Write("N");
		LCD_Data_Write("G");
		LCD_Data_Write("E");
		LCD_Data_Write("L");
		LCD_Data_Write(":");
		LCD_Data_Write(" ");
		LCD_Data_Write("-");
		LCD_Data_Write("2");
		LCD_Data_Write("7");
		}
	else if ( angle == 9)
	{
		LCD_Data_Write("A");
		LCD_Data_Write("N");
		LCD_Data_Write("G");
		LCD_Data_Write("E");
		LCD_Data_Write("L");
		LCD_Data_Write(":");
		LCD_Data_Write(" ");
		LCD_Data_Write("-");
		LCD_Data_Write("1");
		LCD_Data_Write("8");
		}
	else if ( angle == 10)
	{
		LCD_Data_Write("A");
		LCD_Data_Write("N");
		LCD_Data_Write("G");
		LCD_Data_Write("E");
		LCD_Data_Write("L");
		LCD_Data_Write(":");
		LCD_Data_Write(" ");
		LCD_Data_Write("-");
		LCD_Data_Write("9");
		}
	else if ( angle == 11)
	{
		LCD_Data_Write("A");
		LCD_Data_Write("N");
		LCD_Data_Write("G");
		LCD_Data_Write("E");
		LCD_Data_Write("L");
		LCD_Data_Write(":");
		LCD_Data_Write(" ");
		LCD_Data_Write("0");
		}
	else if ( angle == 12)
	{
		LCD_Data_Write("A");
		LCD_Data_Write("N");
		LCD_Data_Write("G");
		LCD_Data_Write("E");
		LCD_Data_Write("L");
		LCD_Data_Write(":");
		LCD_Data_Write(" ");
		LCD_Data_Write("+");
		LCD_Data_Write("9");
		}
	else if ( angle == 13)
	{
		LCD_Data_Write("A");
		LCD_Data_Write("N");
		LCD_Data_Write("G");
		LCD_Data_Write("E");
		LCD_Data_Write("L");
		LCD_Data_Write(":");
		LCD_Data_Write(" ");
		LCD_Data_Write("+");
		LCD_Data_Write("1");
		LCD_Data_Write("8");
		}
	else if ( angle == 14)
	{
		LCD_Data_Write("A");
		LCD_Data_Write("N");
		LCD_Data_Write("G");
		LCD_Data_Write("E");
		LCD_Data_Write("L");
		LCD_Data_Write(":");
		LCD_Data_Write(" ");
		LCD_Data_Write("+");
		LCD_Data_Write("2");
		LCD_Data_Write("7");
		}
	else if ( angle == 15)
	{
		LCD_Data_Write("A");
		LCD_Data_Write("N");
		LCD_Data_Write("G");
		LCD_Data_Write("E");
		LCD_Data_Write("L");
		LCD_Data_Write(":");
		LCD_Data_Write(" ");
		LCD_Data_Write("+");
		LCD_Data_Write("3");
		LCD_Data_Write("6");
		}
	else if ( angle == 16)
	{
		LCD_Data_Write("A");
		LCD_Data_Write("N");
		LCD_Data_Write("G");
		LCD_Data_Write("E");
		LCD_Data_Write("L");
		LCD_Data_Write(":");
		LCD_Data_Write(" ");
		LCD_Data_Write("+");
		LCD_Data_Write("4");
		LCD_Data_Write("5");
		}
	else if ( angle == 17)
	{
		LCD_Data_Write("A");
		LCD_Data_Write("N");
		LCD_Data_Write("G");
		LCD_Data_Write("E");
		LCD_Data_Write("L");
		LCD_Data_Write(":");
		LCD_Data_Write(" ");
		LCD_Data_Write("+");
		LCD_Data_Write("5");
		LCD_Data_Write("4");
		}
	else if ( angle == 18)
	{
		LCD_Data_Write("A");
		LCD_Data_Write("N");
		LCD_Data_Write("G");
		LCD_Data_Write("E");
		LCD_Data_Write("L");
		LCD_Data_Write(":");
		LCD_Data_Write(" ");
		LCD_Data_Write("+");
		LCD_Data_Write("6");
		LCD_Data_Write("3");
		}
	else if ( angle == 19)
	{
		LCD_Data_Write("A");
		LCD_Data_Write("N");
		LCD_Data_Write("G");
		LCD_Data_Write("E");
		LCD_Data_Write("L");
		LCD_Data_Write(":");
		LCD_Data_Write(" ");
		LCD_Data_Write("+");
		LCD_Data_Write("7");
		LCD_Data_Write("2");
		}
	else if ( angle == 20)
	{
		LCD_Data_Write("A");
		LCD_Data_Write("N");
		LCD_Data_Write("G");
		LCD_Data_Write("E");
		LCD_Data_Write("L");
		LCD_Data_Write(":");
		LCD_Data_Write(" ");
		LCD_Data_Write("+");
		LCD_Data_Write("8");
		LCD_Data_Write("1");
		}
		else if ( angle == 21)
	{
		LCD_Data_Write("A");
		LCD_Data_Write("N");
		LCD_Data_Write("G");
		LCD_Data_Write("E");
		LCD_Data_Write("L");
		LCD_Data_Write(":");
		LCD_Data_Write(" ");
		LCD_Data_Write("+");
		LCD_Data_Write("9");
		LCD_Data_Write("0");
		}
	else
	{
		LCD_Data_Write("E");
		LCD_Data_Write("R");
		LCD_Data_Write("R");
		LCD_Data_Write("O");
		LCD_Data_Write("R");
	}
}
	
//Main
void main (void)
	{
		InitDevice();
		angle = 11;
		valueTHTL = 0;
		
		IE = 0x82; // Enable Timer 0 Interrupt
		
		TMOD = 0x11; // 16 BIT TIMER MODS
		TH0 = 0xEC;
		TL0 = 0x10;
		TCON = TCON | (0x01 << 4); // Start Timer 0
		
		while(1)
		{
			if (!(P0&(0x01 << 2))) // If MCU Button is pushed
			{
				angle = angle - 1;
				if (angle < 1) // Check if angle is less than 1
				{
					angle = 1; // Set angle to minimum of 1
				}
				LCD_Conditions();
				while (!(P0&(0x01 << 2)));
			}
			else if (!(P0&(0x01 << 3)))
			{
				angle = angle + 1;
				if (angle > 21) // Check if angle greater than 21
				{
					angle = 21; // Set angle to maximum 21
				}
				LCD_Conditions();
				while (!(P0&(0x01 << 3)));
			}
		}
	}

//Interrupt	
	void T0_ISR(void) interrupt 1
	{
		unsigned int value;
		P0 = P0 | (0x01); // Set signal Pin
		
		value = 12;
		
		TCON = TCON &~ (0x01 << 4); // CLEAR TR0
		TH0 = 0xEC;
		TL0 = 0x10; // Having th0 and tl0 for 20 ms
		TCON = TCON | (0x01 << 4); // Starting Timer 0
		
		
		TCON = TCON &~ (0x01 << 6); // Clearing Timer 1
		valueTHTL = 0xFF01 - (value * angle);
		TH1 = (unsigned char)((valueTHTL >> 8) & 0x00FF); // Having TL1 and TH1 variables depending on the angle
		TL1 = (unsigned char)((valueTHTL) & 0x00FF);
		TCON = TCON | (0x01 << 6); // Starting Timer 1
		
		
		while(!(TCON & (0x01 << 7))); // Waiting until TF1 == 1
		TCON = TCON &~ (0x01 << 6); // Clearing Timer 1
		TCON = TCON &~ (0x01 << 7); // Clearing TF1
		
		P0 = P0 &~ (0x01); // Clearing Signal Pin
	}
	
