// Pin 1 GND
// Pin 2 5V
// RS Pin 0.7
// RW Pin 0.6
// E Pin 0.5
// D0 Pin 1.7
// D1 Pin 1.6
// D2 Pin 1.5
// D3 Pin 1.4
// D4 Pin 1.3
// D5 Pin 1.2
// D6 Pin 1.1
// D7 Pin 1.0

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

//Setup Defaults for P0
P0MDOUT = P0MDOUT_B0__PUSH_PULL | P0MDOUT_B1__PUSH_PULL
| P0MDOUT_B2__PUSH_PULL | P0MDOUT_B3__PUSH_PULL
| P0MDOUT_B4__PUSH_PULL | P0MDOUT_B5__PUSH_PULL
| P0MDOUT_B6__PUSH_PULL | P0MDOUT_B7__PUSH_PULL;
}

void pause(void)
	{
	unsigned char dly1, dly2, dly3;
	for (dly1 = 0; dly1 < 0xFF; dly1++)
		{
			for (dly2 = 0; dly2 < 0xFF; dly2++)
			{
				for (dly3 = 0; dly3 < 0x01; dly3++)
				{
				}
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
	
void main (void)
	{
		InitDevice();
		
		while(1)
		{
			Init_LCD();
			LCD_Data_Write (" ");
			LCD_Data_Write ("I");
			LCD_Data_Write ("S");
			LCD_Data_Write ("S");
			LCD_Data_Write ("A");
			LCD_Data_Write ("M");
			LCD_Data_Write (" ");
			LCD_Data_Write (" ");
			LCD_Data_Write ("Z");
			LCD_Data_Write ("A");
			LCD_Data_Write ("N");
			LCD_Data_Write ("T");
			LCD_Data_Write ("O");
			LCD_Data_Write ("U");
			LCD_Data_Write ("T");
			LCD_Data_Write (" ");
		}
	}
