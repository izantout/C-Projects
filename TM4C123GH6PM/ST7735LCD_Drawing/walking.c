/*
	An implimentation of helpful fixed point routines
*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "walking.h"
#include "ST7735.h"

/**************ST7735_XYPlotMan***************
 Function the draw the man
 Inputs:  x   x co-ordinate of the figure
          y   y co-ordinate of the figure
          h   height of the figure
          color:   color to be given 
 Outputs: none
*/

void ST7735_XYPlotMan(int32_t x, int32_t y,int32_t h, uint16_t color)
{
		ST7735_DrawCircle(x - 20, 65, 15,color);							//face
		ST7735_DrawLine(x - 20, 80, x - 20, 125, color);			//body
		ST7735_DrawLine(x - 20, 125, x - 20 - y, 150, color);	//leg1
		ST7735_DrawLine(x - 20, 125, x - 20 + y, 150, color);	//leg2
		ST7735_DrawLine(x - 20, 90, x - 20 + h, 100, color);	//hand1
		ST7735_DrawLine(x - 20, 90, x - 20 - h, 105, color);	//hand2
}
