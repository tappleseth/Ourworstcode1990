#ifndef HEADERTHINGY_H
#define HEADERTHINGY_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "C:\StellarisWare\inc\hw_ints.h"
#include "C:\StellarisWare\inc\hw_memmap.h"
#include "C:\StellarisWare\inc\hw_types.h"

//#include "C:\StellarisWare\inc\lm3s8962.h"
#include "C:\StellarisWare\driverlib\debug.h"
#include "C:\StellarisWare\driverlib\sysctl.h"
#include "C:\StellarisWare\boards\ek-lm3s8962\drivers\rit128x96x4.h"
#include "C:\StellarisWare\driverlib\timer.h"
#include "C:\StellarisWare\driverlib\gpio.h"
#include "C:\StellarisWare\driverlib\pwm.h"
#include "C:\StellarisWare\driverlib\interrupt.h"
#include "C:\StellarisWare\driverlib\uart.h"
#include "C:\StellarisWare\driverlib\adc.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "lcd_message.h"



#include "hw_sysctl.h"


#include "grlib.h"

#include "osram128x64x4.h"
#include "formike128x128x16.h"

//#include "bitmap.h"

#define MAX_QUEUE_LENGTH        6
#define TASK_SELECT             4
#define HIGH                    TRUE
#define LOW                     FALSE
#define SOUND_ENABLE            1

#define PORT_DATA               (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7)  // full 8 bits of port used

//PRIMITIVE TYPE DEFINITION
typedef enum {FALSE = 0, TRUE = 1} bool;


//SEMAPHORE FUN LAND
extern bool TrainComActive;
extern bool SwitchConActive;
extern bool CurrentTrainActive;
//extern bool SerialComActive;


//GLOBAL VARIABLE DECLARATIONS


extern unsigned int tempCount;
extern unsigned int frequencyCount;
extern unsigned int TimerState;
extern unsigned int TrainState;
extern int seed;

extern unsigned long brakeTemp;
extern bool brakeHighTemp;

extern xQueueHandle xOLEDQueue;

//FUNCTION PROTOTYPES
extern void Startup(void);
/*TOM RTOS STUFF
*/

//we'll need these TaskHandles to suspend/resume tasks later

extern  xTaskHandle vTrainCom;
extern  xTaskHandle vSwitchCon;
extern  xTaskHandle vCurrentTrain;
extern  xTaskHandle vSerialCom;
extern  xTaskHandle vSchedule;
extern xTaskHandle vBrakeTemp;
//extern int getStackSize(void);
//extern void popFromStack(void);
//extern void Schedule(void);

extern void TrainCom(void *vParameters);
extern void CurrentTrain(void *vParameters);
extern void SwitchControl(void *vParameters);
extern void SerialCom(void *vParameters);

extern void IntTimer0(void);
extern int randomInteger(int a, int b);
extern void IntGPIOe(void);
extern void IntGPIOf(void);

extern void UARTIntHandler(void);
extern void UARTSend(const unsigned char *pucBuffer, unsigned long ulCount);

extern void BrakeTemp(void *vParameters);
extern void pin(bool);
//END FUNCTION PROTOTYPES

//END GLOBAL VARIABLE DECLARATIONS

//STRUCT DEFINITIONS
 
extern bool north;
extern bool east;
extern bool west;
extern bool south;
extern bool gridlock;
extern bool trainPresent;
extern bool trainComComplete;
extern bool currentTrainComplete;
extern bool switchConComplete;
extern unsigned char fromDirection;
extern double passengerCount;
extern unsigned int globalCount;
extern unsigned int trainSize;
extern unsigned int traversalTime;
extern unsigned int gridlockTime;
extern unsigned int startTime;

extern unsigned int direction;

extern bool toggleNorth;
extern bool toggleSouth;
extern bool toggleWest;
extern bool toggleEast;
  
extern unsigned int brightness;
extern unsigned int flashCount;
extern unsigned int noiseCount;

extern int randd;
extern bool firstCycle;
extern unsigned int brightness;
extern unsigned int startTime;  

#endif
//END STRUCT DEFINITIONS
