#include <reg51.h>
//main provdes the entry point to our code
void main(void) {
	
	//Variable decleration at beginning of functions
	unsigned char x;
	
	//Superloop Architecture/Structure of Embedded Programming
	while(1) {
		
	      P1=0xF5; //Output to Port 1 the value 0xf5
		
	      //Busy Wait Delay   	
        for(x = 0; x < 0xFE; x = x+1);
		
        P1=0x0A; //Output to Port 1 the value 0xf5
		
		    //Busy Wait Delay 
        for(x = 0; x < 0xFE; x = x+1);
		
	};
	
}
