/*
 * main.c - Example project for UT.6.02x Embedded Systems - Shape the World
 * Jonathan Valvano and Ramesh Yerraballi
 * March 1, 2015
 * Hardware requirements 
     TM4C123 LaunchPad, optional Nokia5110
     CC3100 wifi booster and 
     an internet access point with OPEN, WPA, or WEP security
 
 * derived from TI's getweather example
 * Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

/*
 * Application Name     -   Get weather
 * Application Overview -   This is a sample application demonstrating how to
                            connect to openweathermap.org server and request for
              weather details of a city.
 * Application Details  -   http://processors.wiki.ti.com/index.php/CC31xx_SLS_Get_Weather_Application
 *                          doc\examples\sls_get_weather.pdf
 */
 /* CC3100 booster pack connections (unused pins can be used by user application)
Pin  Signal        Direction      Pin   Signal     Direction
P1.1  3.3 VCC         IN          P2.1  Gnd   GND      IN
P1.2  PB5 UNUSED      NA          P2.2  PB2   IRQ      OUT
P1.3  PB0 UART1_TX    OUT         P2.3  PE0   SSI2_CS  IN
P1.4  PB1 UART1_RX    IN          P2.4  PF0   UNUSED   NA
P1.5  PE4 nHIB        IN          P2.5  Reset nRESET   IN
P1.6  PE5 UNUSED      NA          P2.6  PB7  SSI2_MOSI IN
P1.7  PB4 SSI2_CLK    IN          P2.7  PB6  SSI2_MISO OUT
P1.8  PA5 UNUSED      NA          P2.8  PA4   UNUSED   NA
P1.9  PA6 UNUSED      NA          P2.9  PA3   UNUSED   NA
P1.10 PA7 UNUSED      NA          P2.10 PA2   UNUSED   NA

Pin  Signal        Direction      Pin   Signal      Direction
P3.1  +5  +5 V       IN           P4.1  PF2 UNUSED      OUT
P3.2  Gnd GND        IN           P4.2  PF3 UNUSED      OUT
P3.3  PD0 UNUSED     NA           P4.3  PB3 UNUSED      NA
P3.4  PD1 UNUSED     NA           P4.4  PC4 UART1_CTS   IN
P3.5  PD2 UNUSED     NA           P4.5  PC5 UART1_RTS   OUT
P3.6  PD3 UNUSED     NA           P4.6  PC6 UNUSED      NA
P3.7  PE1 UNUSED     NA           P4.7  PC7 NWP_LOG_TX  OUT
P3.8  PE2 UNUSED     NA           P4.8  PD6 WLAN_LOG_TX OUT
P3.9  PE3 UNUSED     NA           P4.9  PD7 UNUSED      IN (see R74)
P3.10 PF1 UNUSED     NA           P4.10 PF4 UNUSED      OUT(see R75)

UART0 (PA1, PA0) sends data to the PC via the USB debug cable, 115200 baud rate
Port A, SSI0 (PA2, PA3, PA5, PA6, PA7) sends data to Nokia5110 LCD

*/
#include "..\cc3100\simplelink\include\simplelink.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "utils/cmdline.h"
#include "application_commands.h"
#include "ST7735.h"
#include "LED.h"
#include "Nokia5110.h"
#include <string.h>
#include <time.h>
#include <math.h>
#define SERVER "api.openweathermap.org"
/*
 * Application's entry point
 */
// 1) change "Long Beach" to your city
// 2) metric(for celsius), imperial(for fahrenheit)
// api.openweathermap.org/data/2.5/weather?q={city name},{state code}&appid={API key}
#define REQUEST "GET /data/2.5/weather?q=Long Beach&APPID=ef7c57a9ec9bb8e62d92babeab782750&units=imperial HTTP/1.1\r\nUser-Agent: Keil\r\nHost:api.openweathermap.org\r\nAccept: */*\r\n\r\n"
char cityQ[] = "GET /data/2.5/weather?q=";
char cityId[] = "GET /data/2.5/weather?id=";
char cityZip[] = "GET /data/2.5/weather?zip=";
char cityLat[] = "GET /data/2.5/weather?lat=";
char cityLong[] = "&lon=";
char requestEnd[] = "&APPID=ef7c57a9ec9bb8e62d92babeab782750&units=imperial HTTP/1.1\r\nUser-Agent: Keil\r\nHost:api.openweathermap.org\r\nAccept: */*\r\n\r\n";
char sendRequest[200] = "";
// 1) To Do: go to http://openweathermap.org/appid#use 
// 2) Register on the Sign up page
// 3) get an API key (APPID) replace the 7907b2abac2053aed180a74b9310b119 with your APPID

// these three strings will be filled by getWeather
#define MAXLEN 100
char City[MAXLEN];
char Temperature[MAXLEN];
char tempMin[MAXLEN];
char tempMax[MAXLEN];
char Weather[MAXLEN];
char Humidity[MAXLEN];
char choice[200] = "";
char cityInput[200] = "";
// To Do: replace the following three lines with your access point information
#define SSID_NAME  "Pixel6" /* Access point name to connect to */
#define SEC_TYPE   SL_SEC_TYPE_WPA
#define PASSKEY    "fdd76bef4361"  /* Password in case of secure AP */ 
void LCD_OutString(char *pcBuf){
  ST7735_OutString(pcBuf); // send to LCD
  UARTprintf(pcBuf);          // send to UART
}
void LCD_Init(void){
  ST7735_InitR(INITR_REDTAB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
//	ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  UARTStdioConfig(0,115200,50000000);
}
void UARToutString(char *pcBuf){
  char myBuf[2];
  myBuf[1] = 0;
  uint32_t charCount=0;
  while(*pcBuf){
    myBuf[0] = *pcBuf;
    UARTprintf(myBuf); // one character at a time
    if(*pcBuf == '\n'){
      charCount = 0;
    }else{
      charCount++;
      if(charCount>50){
    	charCount = 0;
    	myBuf[0] = '\n';
    	UARTprintf(myBuf);
      }
    }
    pcBuf++;
  }
}

/**/
#define LOOP_FOREVER(line_number) \
            {\
                while(1); \
            }

#define ASSERT_ON_ERROR(line_number, error_code) \
            {\
                /* Handling the error-codes is specific to the application */ \
                if (error_code < 0) \
                {\
                   LCD_OutString(error_code);\
                   return error_code; \
                }\
                /* else, continue w/ execution */ \
            }


#define BAUD_RATE           115200
#define MAX_RECV_BUFF_SIZE  1024
#define MAX_SEND_BUFF_SIZE  512
#define MAX_HOSTNAME_SIZE   40
#define MAX_PASSKEY_SIZE    32
#define MAX_SSID_SIZE       32


#define SUCCESS             0

#define CONNECTION_STATUS_BIT   0
#define IP_AQUIRED_STATUS_BIT   1

//#ifdef SL_IF_TYPE_UART
//#define COMM_PORT_NUM 21
//SlUartIfParams_t params;
//#endif /* SL_IF_TYPE_UART */

/* Application specific status/error codes */
typedef enum{
    DEVICE_NOT_IN_STATION_MODE = -0x7D0,/* Choosing this number to avoid overlap w/ host-driver's error codes */

    STATUS_CODE_MAX = -0xBB8
}e_AppStatusCodes;


/* Status bits - These are used to set/reset the corresponding bits in 'g_Status' */
typedef enum{
    STATUS_BIT_CONNECTION =  0, /* If this bit is:
                                 *      1 in 'g_Status', the device is connected to the AP
                                 *      0 in 'g_Status', the device is not connected to the AP
                                 */

    STATUS_BIT_IP_AQUIRED,       /* If this bit is:
                                 *      1 in 'g_Status', the device has acquired an IP
                                 *      0 in 'g_Status', the device has not acquired an IP
                                 */

}e_StatusBits;


#define SET_STATUS_BIT(status_variable, bit)    status_variable |= (1<<(bit))
#define CLR_STATUS_BIT(status_variable, bit)    status_variable &= ~(1<<(bit))
#define GET_STATUS_BIT(status_variable, bit)    (0 != (status_variable & (1<<(bit))))

#define IS_CONNECTED(status_variable)           GET_STATUS_BIT(status_variable, \
                                                               STATUS_BIT_CONNECTION)
#define IS_IP_AQUIRED(status_variable)          GET_STATUS_BIT(status_variable, \
                                                               STATUS_BIT_IP_AQUIRED)

typedef struct{
    UINT8 SSID[MAX_SSID_SIZE];
    INT32 encryption;
    UINT8 password[MAX_PASSKEY_SIZE];
}UserInfo;

/*
 * GLOBAL VARIABLES -- Start
 */
struct{
  char Recvbuff[MAX_RECV_BUFF_SIZE];
  char SendBuff[MAX_SEND_BUFF_SIZE];
  char HostName[MAX_HOSTNAME_SIZE];
  unsigned long DestinationIP;
  int SockID;
}appData;

typedef enum{
    CONNECTED = 0x01,
    IP_AQUIRED = 0x02,
    IP_LEASED = 0x04,
    PING_DONE = 0x08

}e_Status;
UINT32  g_Status = 0;
/*
 * GLOBAL VARIABLES -- End
 */


 /*
 * STATIC FUNCTION DEFINITIONS  -- Start
 */
void WlanConnect(void);
static int32_t configureSimpleLinkToDefaultState(char *);
static uint32_t initializeAppVariables(void);
static int CreateConnection(void);
static int32_t getWeather(void);
static int32_t GetHostIP(void);
/*
 * STATIC FUNCTION DEFINITIONS -- End
 */


/*
 * * ASYNCHRONOUS EVENT HANDLERS -- Start
 */

/*!
    \brief This function handles WLAN events

    \param[in]      pWlanEvent is the event passed to the handler

    \return         None

    \note

    \warning
*/
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent){
  switch(pWlanEvent->Event){
    case SL_WLAN_CONNECT_EVENT:
    {
      SET_STATUS_BIT(g_Status, STATUS_BIT_CONNECTION);

            /*
             * Information about the connected AP (like name, MAC etc) will be
             * available in 'sl_protocol_wlanConnectAsyncResponse_t' - Applications
             * can use it if required
             *
             * sl_protocol_wlanConnectAsyncResponse_t *pEventData = NULL;
             * pEventData = &pWlanEvent->EventData.STAandP2PModeWlanConnected;
             *
             */
    }
    break;

    case SL_WLAN_DISCONNECT_EVENT:
    {
      sl_protocol_wlanConnectAsyncResponse_t*  pEventData = NULL;

      CLR_STATUS_BIT(g_Status, STATUS_BIT_CONNECTION);
      CLR_STATUS_BIT(g_Status, STATUS_BIT_IP_AQUIRED);

      pEventData = &pWlanEvent->EventData.STAandP2PModeDisconnected;

            /* If the user has initiated 'Disconnect' request, 'reason_code' is SL_USER_INITIATED_DISCONNECTION */
      if(SL_USER_INITIATED_DISCONNECTION == pEventData->reason_code){
        LCD_OutString(" Device disconnected from the AP on application's request \r\n");
      }
      else{
        LCD_OutString(" Device disconnected from the AP on an ERROR..!! \r\n");
      }
    }
    break;

    default:
    {
      LCD_OutString(" [WLAN EVENT] Unexpected event \r\n");
    }
    break;
  }
}

/*!
    \brief This function handles events for IP address acquisition via DHCP
           indication

    \param[in]      pNetAppEvent is the event passed to the handler

    \return         None

    \note

    \warning
*/
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent){
  switch(pNetAppEvent->Event)
  {
    case SL_NETAPP_IPV4_ACQUIRED:
    {

      SET_STATUS_BIT(g_Status, STATUS_BIT_IP_AQUIRED);
        /*
             * Information about the connected AP's ip, gateway, DNS etc
             * will be available in 'SlIpV4AcquiredAsync_t' - Applications
             * can use it if required
             *
             * SlIpV4AcquiredAsync_t *pEventData = NULL;
             * pEventData = &pNetAppEvent->EventData.ipAcquiredV4;
             * <gateway_ip> = pEventData->gateway;
             *
             */

    }
    break;

    default:
    {
            LCD_OutString(" [NETAPP EVENT] Unexpected event \r\n");
    }
    break;
  }
}

/*!
    \brief This function handles callback for the HTTP server events

    \param[in]      pServerEvent - Contains the relevant event information
    \param[in]      pServerResponse - Should be filled by the user with the
                    relevant response information

    \return         None

    \note

    \warning
*/
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent,
                                  SlHttpServerResponse_t *pHttpResponse){
    /*
     * This application doesn't work with HTTP server - Hence these
     * events are not handled here
     */
  LCD_OutString(" [HTTP EVENT] Unexpected event \r\n");
}

/*!
    \brief This function handles general error events indication

    \param[in]      pDevEvent is the event passed to the handler

    \return         None
*/
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent){
    /*
     * Most of the general errors are not FATAL are are to be handled
     * appropriately by the application
     */
  LCD_OutString(" [GENERAL EVENT] \r\n");
}

/*!
    \brief This function handles socket events indication

    \param[in]      pSock is the event passed to the handler

    \return         None
*/
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock){
  switch( pSock->Event )
  {
    case SL_NETAPP_SOCKET_TX_FAILED:
    {
            /*
            * TX Failed
            *
            * Information about the socket descriptor and status will be
            * available in 'SlSockEventData_t' - Applications can use it if
            * required
            *
            * SlSockEventData_t *pEventData = NULL;
            * pEventData = & pSock->EventData;
            */
      switch( pSock->EventData.status )
      {
        case SL_ECLOSE:
          LCD_OutString(" [SOCK EVENT] Close socket operation failed to transmit all queued packets\r\n");
          break;


        default:
          LCD_OutString(" [SOCK EVENT] Unexpected event \r\n");
          break;
      }
    }
    break;

    default:
      LCD_OutString(" [SOCK EVENT] Unexpected event \r\n");
    break;
  }
}
/*
 * * ASYNCHRONOUS EVENT HANDLERS -- End
 */
void Crash(uint32_t time){
  while(1){
    for(int i=time;i;i--){};
    LED_RedToggle();
  }
}
/*
 * Application's entry point
 */

void DelayWait10ms(uint32_t n){uint32_t volatile time;
  while(n){
//    time = 727240*2/91;  // 10msec for launchpad
    time = 727240/91;  // for simulation
    while(time){
	  	time--;
    }
    n--;
  }
}

void mainPromptOutput(){
	UARTprintf("Welcome to my Embedded Weather Quester!\n");
	UARTprintf("Please choose your query criteria:\n");
	UARTprintf("1.	City Name\n");
	UARTprintf("2.	City ID\n");
	UARTprintf("3.	Geographic Coordinates\n");
	UARTprintf("4.	Zip Code\n");
	ST7735_FillScreen(ST7735_BLACK);
	UARTgets(choice, 2);
}



// Draw a raindrop using ST7735_FillCircle() function
void drawRainDrop(uint8_t x0, uint8_t y0, uint8_t r, uint16_t color) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  ST7735_DrawFastVLine(x0, y0-r, r, color);
  ST7735_DrawFastVLine(x0, y0-r/2, r, color);
  ST7735_DrawFastVLine(x0, y0, r, color);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
  
    ST7735_DrawFastVLine(x0+x, y0-y, y, color);
    ST7735_DrawFastVLine(x0-x, y0-y, y, color);
    ST7735_DrawFastVLine(x0+y, y0-x, x, color);
    ST7735_DrawFastVLine(x0-y, y0-x, x, color);
  }
}



void makeItRain(){
		drawRainDrop(40, 100, 5, ST7735_BLUE);
		drawRainDrop(20, 110, 5, ST7735_BLUE);
		drawRainDrop(40, 120, 5, ST7735_BLUE);
		drawRainDrop(20, 130, 5, ST7735_BLUE);
		drawRainDrop(40, 140, 5, ST7735_BLUE);
		drawRainDrop(60, 103, 5, ST7735_BLUE);
		drawRainDrop(80, 113, 5, ST7735_BLUE);
		drawRainDrop(60, 123, 5, ST7735_BLUE);
		drawRainDrop(80, 133, 5, ST7735_BLUE);
		drawRainDrop(60, 143, 5, ST7735_BLUE);
		DelayWait10ms(500);
		drawRainDrop(40, 100, 5, ST7735_BLACK);
		drawRainDrop(20, 110, 5, ST7735_BLACK);
		drawRainDrop(40, 120, 5, ST7735_BLACK);
		drawRainDrop(20, 130, 5, ST7735_BLACK);
		drawRainDrop(40, 140, 5, ST7735_BLACK);
		drawRainDrop(60, 103, 5, ST7735_BLACK);
		drawRainDrop(80, 113, 5, ST7735_BLACK);
		drawRainDrop(60, 123, 5, ST7735_BLACK);
		drawRainDrop(80, 133, 5, ST7735_BLACK);
		drawRainDrop(60, 143, 5, ST7735_BLACK);
		
		drawRainDrop(20, 100, 5, ST7735_BLUE);
		drawRainDrop(40, 110, 5, ST7735_BLUE);
		drawRainDrop(20, 120, 5, ST7735_BLUE);
		drawRainDrop(40, 130, 5, ST7735_BLUE);
		drawRainDrop(20, 140, 5, ST7735_BLUE);
		drawRainDrop(80, 103, 5, ST7735_BLUE);
		drawRainDrop(60, 113, 5, ST7735_BLUE);
		drawRainDrop(80, 123, 5, ST7735_BLUE);
		drawRainDrop(60, 133, 5, ST7735_BLUE);
		drawRainDrop(80, 143, 5, ST7735_BLUE);
		DelayWait10ms(500);
		drawRainDrop(20, 100, 5, ST7735_BLACK);
		drawRainDrop(40, 110, 5, ST7735_BLACK);
		drawRainDrop(20, 120, 5, ST7735_BLACK);
		drawRainDrop(40, 130, 5, ST7735_BLACK);
		drawRainDrop(20, 140, 5, ST7735_BLACK);
		drawRainDrop(80, 103, 5, ST7735_BLACK);
		drawRainDrop(60, 113, 5, ST7735_BLACK);
		drawRainDrop(80, 123, 5, ST7735_BLACK);
		drawRainDrop(60, 133, 5, ST7735_BLACK);
		drawRainDrop(80, 143, 5, ST7735_BLACK);
}


void drawSun(uint8_t x0, uint8_t y0, uint8_t r, uint16_t color) {
    // Draw a circle for the sun
    ST7735_FillCircle(x0, y0, r, color);
  
    // Draw rays coming out of the sun
		ST7735_DrawLine(x0+20, y0+20, x0+30, y0+30, ST7735_YELLOW);
		ST7735_DrawLine(x0-20, y0-20, x0-30, y0-30, ST7735_YELLOW);
		ST7735_DrawLine(x0+20, y0-20, x0+30, y0-30, ST7735_YELLOW);
		ST7735_DrawLine(x0-20, y0+20, x0-30, y0+30, ST7735_YELLOW);
		DelayWait10ms(500);
		ST7735_DrawLine(x0+20, y0+20, x0+30, y0+30, ST7735_BLACK);
		ST7735_DrawLine(x0-20, y0-20, x0-30, y0-30, ST7735_BLACK);
		ST7735_DrawLine(x0+20, y0-20, x0+30, y0-30, ST7735_BLACK);
		ST7735_DrawLine(x0-20, y0+20, x0-30, y0+30, ST7735_BLACK);
	
		ST7735_DrawLine(x0+20, y0, x0+30, y0, ST7735_YELLOW);
		ST7735_DrawLine(x0-20, y0, x0-30, y0, ST7735_YELLOW);
		ST7735_DrawLine(x0, y0-20, x0, y0-30, ST7735_YELLOW);
		ST7735_DrawLine(x0, y0+20, x0, y0+30, ST7735_YELLOW);
		DelayWait10ms(500);
		ST7735_DrawLine(x0+20, y0, x0+30, y0, ST7735_BLACK);
		ST7735_DrawLine(x0-20, y0, x0-30, y0, ST7735_BLACK);
		ST7735_DrawLine(x0, y0-20, x0, y0-30, ST7735_BLACK);
		ST7735_DrawLine(x0, y0+20, x0, y0+30, ST7735_BLACK);
}

void drawCloud(uint8_t x0, uint8_t y0, uint8_t r, uint16_t color){
	ST7735_FillCircle(x0, y0, r, color);
	ST7735_FillCircle(x0+10, y0-10, r, color);
	ST7735_FillCircle(x0+20, y0, r, color);
	DelayWait10ms(500);
	ST7735_FillCircle(x0, y0, r, ST7735_BLACK);
	ST7735_FillCircle(x0+10, y0-10, r, ST7735_BLACK);
	ST7735_FillCircle(x0+20, y0, r, ST7735_BLACK);
	
	ST7735_FillCircle(x0+30, y0, r, color);
	ST7735_FillCircle(x0+40, y0-10, r, color);
	ST7735_FillCircle(x0+50, y0, r, color);
	DelayWait10ms(500);
	ST7735_FillCircle(x0+30, y0, r, ST7735_BLACK);
	ST7735_FillCircle(x0+40, y0-10, r, ST7735_BLACK);
	ST7735_FillCircle(x0+50, y0, r, ST7735_BLACK);
	
	ST7735_FillCircle(x0+60, y0, r, color);
	ST7735_FillCircle(x0+70, y0-10, r, color);
	ST7735_FillCircle(x0+80, y0, r, color);
	DelayWait10ms(500);
	ST7735_FillCircle(x0+60, y0, r, ST7735_BLACK);
	ST7735_FillCircle(x0+70, y0-10, r, ST7735_BLACK);
	ST7735_FillCircle(x0+80, y0, r, ST7735_BLACK);
}

void drawingDesign(char weather[MAXLEN]){
	if(strstr(weather, "rain")){
		makeItRain();
	} else if (strstr(weather, "sun") || strstr(weather, "clear")){
		drawSun(50, 120, 20, ST7735_YELLOW);
	} else if (strstr(weather, "cloud") || strstr(weather, "clouds") || strstr(weather, "cloudy")){
		drawCloud(30, 130, 10, ST7735_WHITE);
	} else {
	}
}

int main(void){
  int32_t retVal = 0;
  char *pConfig = NULL;
  retVal = initializeAppVariables();
  stopWDT();        // Stop WDT 
  initClk();        // PLL 50 MHz
  LCD_Init();
  LED_Init();       // initialize LaunchPad I/O 
	mainPromptOutput();
	switch (choice[0]) {
    case '1':
				UARTprintf("You chose option 1.\nPlease enter the city name: ");
				UARTgets(cityInput, 200);
				UARTprintf("You chose: ");
				UARTprintf(cityInput);
				UARTprintf("\n");
				strcpy(sendRequest,cityQ);
				strcat(sendRequest,cityInput);
				strcat(sendRequest,requestEnd);
				strcpy(cityInput,sendRequest);
        break;
    case '2':
        UARTprintf("You chose option 2.\n");
				UARTprintf("City ID: ");
				UARTgets(cityInput,200);
				UARTprintf("You chose: ");
				UARTprintf(cityInput);
				UARTprintf("\n");
				strcpy(sendRequest,cityId);
				strcat(sendRequest,cityInput);
				strcat(sendRequest,requestEnd);
				strcpy(cityInput,sendRequest);
        break;
    case '3':
        UARTprintf("You chose option 3.\n");
				UARTprintf("User lat: ");
				UARTgets(cityInput,200);
				UARTprintf("You chose: ");
				UARTprintf(cityInput);
				UARTprintf("\n");
				strcpy(sendRequest,cityLat);
				strcat(sendRequest,cityInput);
				strcat(sendRequest,cityLong);
				UARTprintf("User long: ");
				UARTgets(cityInput,200);
				UARTprintf("You chose: ");
				UARTprintf(cityInput);
				UARTprintf("\n");
				strcat(sendRequest,cityInput);
				strcat(sendRequest,requestEnd);
				strcpy(cityInput,sendRequest);
        break;
    case '4':
			UARTprintf("You chose option 4.\nPlease enter the city Zipcode: ");
				UARTgets(cityInput, 200);
				UARTprintf("You chose: ");
				UARTprintf(cityInput);
				UARTprintf("\n");
				strcpy(sendRequest,cityZip);
				strcat(sendRequest,cityInput);
				strcat(sendRequest,requestEnd);
				strcpy(cityInput,sendRequest);
        break;
    default:
        UARTprintf("Invalid input.\n");
}
  /*
     * Following function configures the device to default state by cleaning
     * the persistent settings stored in NVMEM (viz. connection profiles &
     * policies, power policy etc)
     *
     * Applications may choose to skip this step if the developer is sure
     * that the device is in its default state at start of application
     *
     * Note that all profiles and persistent settings that were done on the
     * device will be lost
     */
  retVal = configureSimpleLinkToDefaultState(pConfig);
  if(retVal < 0){
    if(DEVICE_NOT_IN_STATION_MODE == retVal){
       LCD_OutString(" Failed to configure the device in its default state \r\n");
       Crash(4000000);
    }
  }

    /*
     * Assumption is that the device is configured in station mode already
     * and it is in its default state
     */
  retVal = sl_Start(0, pConfig, 0);
  if((retVal < 0) || (ROLE_STA != retVal) ){
    LCD_OutString(" Failed to start the device \r\n");
    Crash(8000000);

  }
  WlanConnect();
  UARTprintf("Connected\n");

/* Get weather report */
  while(1){
    retVal = getWeather();
    if(retVal == 0){  // valid
      LED_GreenOn();
      UARTprintf("\r\n\r\n");
			LCD_OutString("City: "); LCD_OutString(City); LCD_OutString("\n");
			LCD_OutString("Min Temp: "); LCD_OutString(tempMin); LCD_OutString("\nFahrenheit\n");
			LCD_OutString("Max Temp: "); LCD_OutString(tempMax); LCD_OutString("\nFahrenheit\n");
			LCD_OutString("Humidity: "); LCD_OutString(Humidity); LCD_OutString("\n");
			LCD_OutString("Weather: "); LCD_OutString(Weather); LCD_OutString("\n");
    }
    while(Board_Input()==0){
			drawingDesign(Weather);
		}; // wait for touch
    LED_GreenOff();
		//drawingDesign(Weather);
  }
}

/*!
    \brief This function puts the device in its default state. It:
           - Set the mode to STATION
           - Configures connection policy to Auto and AutoSmartConfig
           - Deletes all the stored profiles
           - Enables DHCP
           - Disables Scan policy
           - Sets Tx power to maximum
           - Sets power policy to normal
           - Unregister mDNS services

    \param[in]      none

    \return         On success, zero is returned. On error, negative is returned
*/
static int32_t configureSimpleLinkToDefaultState(char *pConfig){
  SlVersionFull   ver = {0};
  UINT8           val = 1;
  UINT8           configOpt = 0;
  UINT8           configLen = 0;
  UINT8           power = 0;

  INT32           retVal = -1;
  INT32           mode = -1;

  mode = sl_Start(0, pConfig, 0);


    /* If the device is not in station-mode, try putting it in station-mode */
  if (ROLE_STA != mode){
    if (ROLE_AP == mode){
            /* If the device is in AP mode, we need to wait for this event before doing anything */
      while(!IS_IP_AQUIRED(g_Status));
    }

        /* Switch to STA role and restart */
    retVal = sl_WlanSetMode(ROLE_STA);

    retVal = sl_Stop(0xFF);

    retVal = sl_Start(0, pConfig, 0);

        /* Check if the device is in station again */
    if (ROLE_STA != retVal){
            /* We don't want to proceed if the device is not coming up in station-mode */
      return DEVICE_NOT_IN_STATION_MODE;
    }
  }
    /* Get the device's version-information */
  configOpt = SL_DEVICE_GENERAL_VERSION;
  configLen = sizeof(ver);
  retVal = sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &configOpt, &configLen, (unsigned char *)(&ver));

    /* Set connection policy to Auto + SmartConfig (Device's default connection policy) */
  retVal = sl_WlanPolicySet(SL_POLICY_CONNECTION, SL_CONNECTION_POLICY(1, 0, 0, 0, 1), NULL, 0);

    /* Remove all profiles */
  retVal = sl_WlanProfileDel(0xFF);

    /*
     * Device in station-mode. Disconnect previous connection if any
     * The function returns 0 if 'Disconnected done', negative number if already disconnected
     * Wait for 'disconnection' event if 0 is returned, Ignore other return-codes
     */
  retVal = sl_WlanDisconnect();
  if(0 == retVal){
        /* Wait */
     while(IS_CONNECTED(g_Status));
  }

    /* Enable DHCP client*/
  retVal = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE,1,1,&val);

    /* Disable scan */
  configOpt = SL_SCAN_POLICY(0);
  retVal = sl_WlanPolicySet(SL_POLICY_SCAN , configOpt, NULL, 0);

    /* Set Tx power level for station mode
       Number between 0-15, as dB offset from max power - 0 will set maximum power */
  power = 0;
  retVal = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (unsigned char *)&power);

    /* Set PM policy to normal */
  retVal = sl_WlanPolicySet(SL_POLICY_PM , SL_NORMAL_POLICY, NULL, 0);

    /* TBD - Unregister mDNS services */
  retVal = sl_NetAppMDNSUnRegisterService(0, 0);


  retVal = sl_Stop(0xFF);


  retVal = initializeAppVariables();


  return retVal; /* Success */
}

/*!
    \brief This function initializes the application variables

    \param[in]  None

    \return     0 on success, negative error-code on error
*/
static uint32_t initializeAppVariables(void){
  g_Status = 0;
  memset(&appData, 0, sizeof(appData));
  return SUCCESS;
}


/*!
    \brief Create TCP connection with openweathermap.org

    \param[in]      none

    \return         Socket descriptor for success otherwise negative

    \warning
*/
static int CreateConnection(void){
  SlSockAddrIn_t  Addr;

  INT32 sd = 0;
  INT32 AddrSize = 0;
  INT16 ret_val = 0;

  Addr.sin_family = SL_AF_INET;
  Addr.sin_port = sl_Htons(80);

    /* Change the DestinationIP endianity, to big endian */
  Addr.sin_addr.s_addr = sl_Htonl(appData.DestinationIP);

  AddrSize = sizeof(SlSockAddrIn_t);

  sd = sl_Socket(SL_AF_INET,SL_SOCK_STREAM, 0);
  if( sd < 0 ){
    LCD_OutString("Error creating socket\r\n");
    return sd;
  }

  ret_val = sl_Connect(sd, ( SlSockAddr_t *)&Addr, AddrSize);
  if( ret_val < 0 ){
        /* error */
    LCD_OutString("Error connecting to socket\r\n");
    return ret_val;
  }

  return sd;
}

/*!
    \brief This function obtains the server IP address

    \param[in]      none

    \return         zero for success and -1 for error

    \warning
*/
static int32_t GetHostIP(void){
  int32_t status = -1;

  status = sl_NetAppDnsGetHostByName(appData.HostName,
                                       strlen(appData.HostName),
                                       &appData.DestinationIP, SL_AF_INET);
  if (status < 0){
    LCD_OutString("Unable to reach Host\n");
    return status;
  }
  return SUCCESS;
}

/*
******************************************************************************
    \brief Connecting to a WLAN Access point

    This function connects to the required AP (SSID_NAME).
    This code example can use OPEN, WPA, or WEP security.
    The function will return once we are connected and have acquired IP address

    \param[in]  None

    \return     None

    \note

    \warning    If the WLAN connection fails or we don't aquire an IP address,
                We will be stuck in this function forever.
******************************************************************************
*/
void WlanConnect(void){
  SlSecParams_t secParams;

  secParams.Key = PASSKEY;
  secParams.KeyLen = strlen(PASSKEY);
  secParams.Type = SEC_TYPE; // OPEN, WPA, or WEP

  sl_WlanConnect(SSID_NAME, strlen(SSID_NAME), 0, &secParams, 0);

  while((0 == (g_Status & CONNECTED)) || (0 == (g_Status & IP_AQUIRED))){
    _SlNonOsMainLoopTask();
  }
}
/*!
    \brief Get the Weather from server

    \param[in]      none

    \return         zero for success and -1 for error

    \warning
*/
static int32_t getWeather(void){uint32_t i;
  char *pt = NULL;

  memcpy(appData.HostName,SERVER,strlen(SERVER));
  if(GetHostIP() == 0){
    if( (appData.SockID = CreateConnection()) < 0 ) return -1;

/* HTTP GET string. */
    strcpy(appData.SendBuff,sendRequest); 
// 1) change Austin Texas to your city
// 2) you can change metric to imperial if you want temperature in F
    /* Send the HTTP GET string to the open TCP/IP socket. */
    sl_Send(appData.SockID, appData.SendBuff, strlen(appData.SendBuff), 0);

/* Receive response */
    sl_Recv(appData.SockID, &appData.Recvbuff[0], MAX_RECV_BUFF_SIZE, 0);
    appData.Recvbuff[strlen(appData.Recvbuff)] = '\0';

/* find ticker name in response*/
    pt = strstr(appData.Recvbuff, "\"name\"");
    i = 0; 
    if( NULL != pt ){
      pt = pt + 8; // skip over "name":"
      while((i<MAXLEN)&&(*pt)&&(*pt!='\"')){
        City[i] = *pt; // copy into City string
        pt++; i++;    
      }
    }
    City[i] = 0;

/* find Temperature Value in response */
    pt = strstr(appData.Recvbuff, "\"temp\"");
    i = 0; 
    if( NULL != pt ){
      pt = pt + 7; // skip over "temp":
      while((i<MAXLEN)&&(*pt)&&(*pt!=',')){
        Temperature[i] = *pt; // copy into Temperature string
        pt++; i++;    
      }
    }
    Temperature[i] = 0;
		
		/* find min temp Value in response */
    pt = strstr(appData.Recvbuff, "\"temp_min\"");
    i = 0; 
    if( NULL != pt ){
      pt = pt + 11; // skip over "temp_min":
      while((i<MAXLEN)&&(*pt)&&(*pt!=',')){
        tempMin[i] = *pt; // copy into tempMin string
        pt++; i++;    
      }
    }
    tempMin[i] = 0;
		
		/* find max temp Value in response */
    pt = strstr(appData.Recvbuff, "\"temp_max\"");
    i = 0; 
    if( NULL != pt ){
      pt = pt + 11; // skip over "temp_max":
      while((i<MAXLEN)&&(*pt)&&(*pt!=',')){
        tempMax[i] = *pt; // copy into tempMax string
        pt++; i++;    
      }
    }
    tempMax[i] = 0;

		/* find weather in response */
    pt = strstr(appData.Recvbuff, "\"description\"");
    i = 0; 
    if( NULL != pt ){
      pt = pt + 15; // skip over "description":"
      while((i<MAXLEN)&&(*pt)&&(*pt!='\"')){
        Weather[i] = *pt; // copy into weather string
        pt++; i++;    
      }
    }
    Weather[i] = 0;   
		
		/* find humidity in response */
    pt = strstr(appData.Recvbuff, "\"humidity\"");
    i = 0; 
    if( NULL != pt ){
      pt = pt + 11; // skip over "humidity":"
      while((i<MAXLEN)&&(*pt)&&(*pt!='}')){
        Humidity[i] = *pt; // copy into humidity string
        pt++; i++;    
      }
    }
    Humidity[i] = 0;
		
    sl_Close(appData.SockID);
  }

  return 0;
}
