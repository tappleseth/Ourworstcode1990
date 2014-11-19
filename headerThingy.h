#ifndef HEADERTHINGY_H
#define HEADERTHINGY_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "C:\StellarisWare\inc\hw_ints.h"
#include "C:\StellarisWare\inc\hw_memmap.h"
#include "C:\StellarisWare\inc\hw_types.h"

#include "C:\StellarisWare\inc\lm3s8962.h"
#include "C:\StellarisWare\driverlib\debug.h"
#include "C:\StellarisWare\driverlib\sysctl.h"
#include "C:\StellarisWare\boards\ek-lm3s8962\drivers\rit128x96x4.h"
#include "C:\StellarisWare\driverlib\timer.h"
#include "C:\StellarisWare\driverlib\gpio.h"
#include "C:\StellarisWare\driverlib\pwm.h"
#include "C:\StellarisWare\driverlib\interrupt.h"
#include "C:\StellarisWare\driverlib\uart.h"

#define MAX_QUEUE_LENGTH        6
#define TASK_SELECT             4
#define HIGH                    TRUE
#define LOW                     FALSE
#define SOUND_ENABLE            1

#define PORT_DATA               (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7)  // full 8 bits of port used

//PRIMITIVE TYPE DEFINITION
typedef enum {FALSE = 0, TRUE = 1} bool;


//GLOBAL VARIABLE DECLARATIONS


extern unsigned int tempCount;
extern unsigned int frequencyCount;
extern unsigned int TimerState;
extern unsigned int TrainState;
extern int seed;

//FUNCTION PROTOTYPES
extern void Startup(void);


extern int getStackSize(void);
extern void popFromStack(void);
extern void Schedule(void);

extern void TrainCom(void);
extern void CurrentTrain(void);
extern void SwitchControl(void);
extern void SerialCom(void);

extern void IntTimer0(void);
extern int randomInteger(int a, int b);
extern void IntGPIOe(void);
extern void IntGPIOf(void);

extern void UARTIntHandler(void);
extern void UARTSend(const unsigned char *pucBuffer, unsigned long ulCount);

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

//extern int rand;
extern bool firstCycle;
extern unsigned int brightness;
extern unsigned int startTime;  

#endif
//END STRUCT DEFINITIONS
