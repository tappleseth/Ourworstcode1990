/*
THOMAS APPLESETH && BRANDON JOHANSON && JUSTIN "JUSTIN" TAFT
Professor A. Ecker
EE 472, AUT 2014
Project 2
10/23/2014

DESC: A delightful little bit of embedded programming that simulates a train switching yard. 

Random number generator code provided by professor, originally contained in "rand1.c"
*/


#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "headerThingy.h"

#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "drivers/rit128x96x4.h"
#include "inc/lm3s8962.h"

//Include hardware memory map, GPIO driver, and PWM driver
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"

int randomInteger(int a, int b);
void Delay(int* foolioJenkins);
void setupPWM(void);
void setupOutput(void);
void pin(bool);

int main(){
  setupPWM();
  setupOutput();
  
  north = FALSE;
  east = FALSE;
  west = FALSE;
  
  toggleNorth = FALSE;
  toggleEast = FALSE;
  toggleWest = FALSE;
  
  gridlock = FALSE;
  trainPresent = FALSE;
  trainSize = 2;
  globalCount = 0;
  traversalTime = 0;
  startTime = 0;
  
  seed = 1;
  
  trainComData ourTrainComData;
  northTrainData ourNorthTrainData;
  westTrainData ourWestTrainData;
  eastTrainData ourEastTrainData;
  switchControlData ourSwitchControlData;
  scheduleData ourScheduleData;
  
  ourTrainComData.east = &east;
  ourTrainComData.north = &north;
  ourTrainComData.west = &west;
  ourTrainComData.trainSize = &trainSize;
  ourTrainComData.trainPresent = &trainPresent;
  ourTrainComData.gridlock = &gridlock;
  
  ourNorthTrainData.globalCount = &globalCount;
  ourNorthTrainData.north = &north;
  ourNorthTrainData.traversalTime = &traversalTime;
  ourNorthTrainData.toggleNorth = &toggleNorth;
  
  ourWestTrainData.globalCount = &globalCount;
  ourWestTrainData.west = &west;
  ourWestTrainData.traversalTime = &traversalTime;
  ourWestTrainData.toggleWest = &toggleWest;
  
  ourEastTrainData.globalCount = &globalCount;  
  ourEastTrainData.east = &east;
  ourEastTrainData.traversalTime = &traversalTime;
  ourEastTrainData.toggleEast = &toggleEast;
  
  ourSwitchControlData.east = &east;
  ourSwitchControlData.globalCount = &globalCount;
  ourSwitchControlData.gridlock = &gridlock;
  ourSwitchControlData.north = &north;
  ourSwitchControlData.startTime = &startTime;
  ourSwitchControlData.trainPresent = &trainPresent;
  ourSwitchControlData.trainSize = &trainSize;
  ourSwitchControlData.traversalTime = &traversalTime;
  ourSwitchControlData.west = &west;
  
  ourScheduleData.globalCount = &globalCount;
  
  TCB tomQueue[MAX_QUEUE_LENGTH];
  
  tomQueue[0].justTrainTaskThings = TrainCom;
  tomQueue[0].trainTaskPtr = (void*)&ourTrainComData;
  tomQueue[1].justTrainTaskThings = SwitchControl;
  tomQueue[1].trainTaskPtr = (void*)&ourSwitchControlData;
  tomQueue[2].justTrainTaskThings = NorthTrain;
  tomQueue[2].trainTaskPtr = (void*)&ourNorthTrainData;
  tomQueue[3].justTrainTaskThings = WestTrain;
  tomQueue[3].trainTaskPtr = (void*)&ourWestTrainData;
  tomQueue[4].justTrainTaskThings = EastTrain;
  tomQueue[4].trainTaskPtr = (void*)&ourEastTrainData;
  tomQueue[5].justTrainTaskThings = Schedule;
  tomQueue[5].trainTaskPtr = (void*)&ourScheduleData;
  
  RIT128x96x4Init(1000000); 
  
  //it breaks HERE? WHAT? NOTE TO SELF: WHAAT?
  static char flairTitle1[] = "Applehansontaft \0";
  static char flairTitle2[] = "Discount Freight \0";
  RIT128x96x4StringDraw(flairTitle1, 10, 10, 15);
  RIT128x96x4StringDraw(flairTitle2, 10, 20, 15);
  
  while(1) {
    for(int i = 0; i < MAX_QUEUE_LENGTH; i++) {
      tomQueue[i].justTrainTaskThings(tomQueue[i].trainTaskPtr);
    }    
  }  
}

void TrainCom(void* data) {
#if TASK_SELECT == 0 || TASK_SELECT == -1
  pin(HIGH);
#endif
  
  int direction;
  trainComData* ptr = (trainComData*)data;
  static int brightness;
  
  //Preparing a trainSize in preparation for switchControl
  //(when switchControl decides there is a train, it will use this size)
  if (!*ptr->trainPresent && !*ptr->gridlock) {
    *ptr->trainSize = randomInteger(2, 9);
    brightness = 15;
  }
  
  if(*ptr->gridlock)
    brightness = 0;
  
  char numCars[] = {(char)(48 + *ptr->trainSize), '\0'};
  RIT128x96x4StringDraw("Cars: \0", 10, 50, brightness);
  RIT128x96x4StringDraw(numCars, 50, 50, brightness);
  
  if (*ptr->trainPresent && !*ptr->east && !*ptr->west && !*ptr->north) {
    direction = randomInteger(0, 2);
    
    if(direction == 1) {
      *ptr->east = TRUE;
      *ptr->north = FALSE;    
      *ptr->west = FALSE;
    }
    else if(direction == 2) {
      *ptr->east = FALSE;
      *ptr->north = TRUE;    
      *ptr->west = FALSE;
    }
    else {
      *ptr->east = FALSE;
      *ptr->north = FALSE;    
      *ptr->west = TRUE;
    }    
  }
  
  
#if TASK_SELECT == 0 || TASK_SELECT == -1
  pin(LOW);
#endif
  
  return;
}

void SwitchControl(void* data) {
#if TASK_SELECT == 1 || TASK_SELECT == -1
  pin(HIGH);
#endif
  
  switchControlData* ptr = (switchControlData*)data;
  static int rand = 0;
  static char brando9k[] = "GRIDLOCK! \0";
  static int brightness = 15;
  
  if(!*ptr->gridlock && !*ptr->trainPresent) {
    //startTime is logged as whatever current globalCount value is
    *ptr->startTime = *ptr->globalCount;
    //generate a random number between -2 and 2
    rand = randomInteger(-2, 2);
    
    //if random number is less than 0, we have GRIDLOCK!
    if(rand < 0) {
      *ptr->gridlock = TRUE;
      *ptr->traversalTime = *ptr->globalCount + (-70*rand);
    }
    //if random number is greater than 0, we have a train passing through!
    else {
      *ptr->trainPresent = TRUE;
      *ptr->traversalTime = 35*(*ptr->trainSize) + *ptr->globalCount;
    }
  }
  //CASE 2
  //gridlock is true
  if(*ptr->gridlock) {
    //if globalCount exceeds gridlock timer, end gridlock
    if ((*ptr->globalCount - *ptr->traversalTime) % 6 == 0)
      brightness = 15; 
    
    if ((*ptr->globalCount - *ptr->traversalTime) % 6 == 3)
      brightness = 0;
    
    RIT128x96x4StringDraw(brando9k, 10, 40, brightness); 
    
    if(*ptr->globalCount >= *ptr->traversalTime)
      *ptr->gridlock = FALSE;
  }
  
  //CASE 3
  //train is present
  if(*ptr->trainPresent) {
    if(*ptr->globalCount >= *ptr->traversalTime) {
      //*ptr->trainSize = 0;
      *ptr->trainPresent = FALSE;
      *ptr->north = FALSE;
      *ptr->east = FALSE;
      *ptr->west = FALSE;
    }
  }
  
#if TASK_SELECT == 1 || TASK_SELECT == -1
  pin(LOW);
#endif
  
  return;
}

void NorthTrain(void* data) {
#if TASK_SELECT == 2 || TASK_SELECT == -1
  pin(HIGH);
#endif
  
  northTrainData* ptr = (northTrainData*)data;
  static unsigned int noiseCount = 0;
  static unsigned int northFlashCount = 0;  
  static char northDisplay[] = "NorthTrain \0";
  static int brightness = 15;
  
  if(*ptr->north) {
    if(!*ptr->toggleNorth){
      *ptr->toggleNorth = TRUE;
      noiseCount = *ptr->globalCount + 60;
      northFlashCount = *ptr->globalCount;
      brightness = 15;
    }
    
    if(*ptr->toggleNorth) {
      //FLASH EVENTS
      //loops indefinitely (will end when train passes)
      if((*ptr->globalCount - northFlashCount) % 18 == 0)
        brightness = 15; 
      
      if((*ptr->globalCount - northFlashCount) % 18 == 9)
        brightness = 0;
      
      RIT128x96x4StringDraw(northDisplay, 10, 40, brightness); 
      //END FLASH EVENTS
      
      //SOUND EVENTS
      
      //first blast: lasts 2 seconds
      if(*ptr->globalCount == (noiseCount - 60))
        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
      
      //off time between first and second blast
      if(*ptr->globalCount == (noiseCount - 48))
        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
      
      //wait one second, then start second blast
      if(*ptr->globalCount == (noiseCount - 42))
        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
      
      //wait two seconds, stop second blast
      if(*ptr->globalCount == (noiseCount - 30))
        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
      
      //wait one second, then start first short blast
      if(*ptr->globalCount == (noiseCount - 24))
        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
      
      //wait one second, then stop first short blast
      if(*ptr->globalCount == (noiseCount - 18))
        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
      
      //wait one second, then start second short blast
      if(*ptr->globalCount == (noiseCount - 12))
        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
      
      //wait one second, end all blasts, set bigglesOn to FALSE
      if(*ptr->globalCount == (noiseCount - 6))
        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
      
      if(noiseCount == *ptr->globalCount) {
        noiseCount = *ptr->globalCount + 60;
        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
      }      
    }
  }
  
  //placeholder: need code to STOP SOME SHIT
  if(*ptr->globalCount >= *ptr->traversalTime) {
    PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
    noiseCount = 0;
    northFlashCount = 0;
    brightness = 15;
    *ptr->toggleNorth = FALSE;
    RIT128x96x4StringDraw(northDisplay, 10, 40, 0);
  }
  
#if TASK_SELECT == 2 || TASK_SELECT == -1
  pin(LOW); 
#endif
  
  return;
}

void WestTrain(void* data) {
#if TASK_SELECT == 3 || TASK_SELECT == -1
  pin(HIGH);
#endif
  
  westTrainData* ptr = (westTrainData*)data;  
  static unsigned int westNoiseCount = 0;
  static unsigned int westFlashCount = 0;
  static char westDisplay[] = "WestTrain \0";
  static int brightness = 15;  
  
  if(*ptr->west) {
    if(!*ptr->toggleWest) {
      *ptr->toggleWest = TRUE;
      westNoiseCount = *ptr->globalCount + 42;
      westFlashCount = *ptr->globalCount;
      brightness = 15;
    }
    
    //FLASH EVENTS
    if(*ptr->toggleWest) {
      if ((*ptr->globalCount - westFlashCount) % 12 == 0)
        brightness = 15; 
      
      if ((*ptr->globalCount - westFlashCount) % 12 == 6)
        brightness = 0;
      
      RIT128x96x4StringDraw(westDisplay, 10, 40, brightness); 
      
      //SOUND EVENTS
      
      //first blast: lasts 2 seconds
      if(*ptr->globalCount == (westNoiseCount - 42))
        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
      
      //off time between first and second blast
      if(*ptr->globalCount == (westNoiseCount - 30))
        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
      
      //wait one second, then start second blast
      if(*ptr->globalCount == (westNoiseCount - 24))
        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
      
      //wait two seconds, stop second blast
      if(*ptr->globalCount == (westNoiseCount - 18))
        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
      
      //wait one second, then start first short blast
      if(*ptr->globalCount == (westNoiseCount - 12))
        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
      
      //wait one second, then stop first short blast
      if(*ptr->globalCount == (westNoiseCount - 6))
        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);    
      
      if(westNoiseCount == *ptr->globalCount) {
        westNoiseCount = *ptr->globalCount + 42;
        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
      }
    }
  }
  
  if(*ptr->globalCount >= *ptr->traversalTime) {
    PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
    westNoiseCount = 0;
    westFlashCount = 0;
    brightness = 15;
    *ptr->toggleWest = FALSE;
    RIT128x96x4StringDraw(westDisplay, 10, 40, 0);
  }
  
#if TASK_SELECT == 3 || TASK_SELECT == -1
  pin(LOW);
#endif
  
  return;
}

void EastTrain(void* data){
#if TASK_SELECT == 4 || TASK_SELECT == -1
  pin(HIGH);
#endif
  
  eastTrainData* ptr = (eastTrainData*)data;  
  static unsigned int eastNoiseCount = 0;
  static unsigned int eastFlashCount = 0;
  static char eastDisplay[] = "EastTrain \0";
  static int brightness = 15;
  
  if(*ptr->east) {
    if(!*ptr->toggleEast) {
      *ptr->toggleEast = TRUE;
      eastNoiseCount = *ptr->globalCount + 78;
      eastFlashCount = *ptr->globalCount;
      brightness = 15;
    }
    
    //*****FLASH EVENTS*****  
    
    if(*ptr->toggleEast) {
      //FLASH OFF
      
      if ((*ptr->globalCount - eastFlashCount) % 24 == 0)
        brightness = 15; 
      
      if ((*ptr->globalCount - eastFlashCount) % 24 == 12)
        brightness = 0;
      
      RIT128x96x4StringDraw(eastDisplay, 10, 40, brightness); 
      
      //*****SOUND EVENTS*****
      
      //first blast: lasts 2 seconds
      if(*ptr->globalCount == (eastNoiseCount - 78)) 
        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
      
      //off time between first and second blast
      if(*ptr->globalCount == (eastNoiseCount - 66)) 
        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
      
      //wait one second, then start second blast
      if(*ptr->globalCount == (eastNoiseCount - 60)) 
        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
      
      //wait two seconds, stop second blast
      if(*ptr->globalCount == (eastNoiseCount - 48)) 
        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
      
      //wait one second, then start first short blast
      if(*ptr->globalCount == (eastNoiseCount - 42)) 
        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
      
      //wait one second, then stop first short blast
      if(*ptr->globalCount == (eastNoiseCount - 30)) 
        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
      
      //first blast: lasts 2 seconds
      if(*ptr->globalCount == (eastNoiseCount - 24)) 
        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
      
      //off time between first and second blast
      if(*ptr->globalCount == (eastNoiseCount - 18)) 
        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
      
      //wait one second, then start second blast
      if(*ptr->globalCount == (eastNoiseCount - 12))
        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
      
      //wait two seconds, stop second blast
      if(*ptr->globalCount == (eastNoiseCount - 6))
        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
      
      if(eastNoiseCount == *ptr->globalCount) {
        eastNoiseCount = *ptr->globalCount + 78;
        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
      }
    }   
  }
  
  if(*ptr->globalCount >= *ptr->traversalTime) {
    PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
    eastNoiseCount = 0;
    eastFlashCount = 0;
    brightness = 15;
    *ptr->toggleEast = FALSE;
    RIT128x96x4StringDraw(eastDisplay, 10, 40, 0);
  }
  
#if TASK_SELECT == 4 || TASK_SELECT == -1
  pin(LOW);
#endif
  
  return;
}

void Schedule(void* data) {
#if TASK_SELECT == 5 || TASK_SELECT == -1
  pin(HIGH);
#endif
  
  static unsigned int justinCrazy;
  static char globalCountArray[10];
  
  for (int tibo = 0; tibo < 10; tibo++)
    globalCountArray[tibo] = ' ';
  
  scheduleData* tomPtr = (scheduleData*) data; 
  int delay1 = DELAY;
  Delay(&delay1);
  justinCrazy = *tomPtr->globalCount;
  justinCrazy++;
  *tomPtr->globalCount = justinCrazy;
  char scheduleTitle[] = "TOM TIME: \0";
  RIT128x96x4StringDraw(scheduleTitle, 0, 75, 15);
  
  int i = 9; 
  
  while(justinCrazy > 0) {
    globalCountArray[i] = (justinCrazy%10) + 48;
    justinCrazy = justinCrazy/10;
    i--;     
  }
  
  RIT128x96x4StringDraw(globalCountArray, 50, 75, 15);
  
#if TASK_SELECT == 5 || TASK_SELECT == -1
  pin(LOW);
#endif
  
  return;
}
void Delay(int *waffle)
{
  for(int d = *waffle; d > 0; d--);
  return;
}
//random number generator code provided by instructor (originally rand1.c)
int randomInteger(int low, int high)
{
  double randNum = 0.0;
  int multiplier = 2743;
  int addOn = 5923;
  double max = INT_MAX + 1.0;
  
  int retVal = 0;
  
  if (low > high)
    retVal = randomInteger(high, low);
  else
  {
    seed = seed*multiplier + addOn;
    randNum = seed;
    
    if (randNum <0)
    {
      randNum = randNum + max;
    }
    
    randNum = randNum/max;
    
    retVal = ((int)((high-low+1)*randNum))+low;
  }
  
  return retVal;
}

void setupPWM(void) {
  //Set PWM Divide Ratio to 1
  SysCtlPWMClockSet(SYSCTL_PWMDIV_1);
  
  //Set Device: PWM0 Enabled
  SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
  
  //Set GPIO Port: G Enabled
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
  
  //Tell Port G, Pin 1, to take input from PWM 0
  GPIOPinTypePWM(GPIO_PORTG_BASE, GPIO_PIN_1);
  
  //Set a 440 Hz frequency as u1Period
  unsigned int ulPeriod = SysCtlClockGet() / 100;
  
  //Configure PWM0 in up-down count mode, no sync to clock
  PWMGenConfigure(PWM0_BASE, PWM_GEN_0,
                  PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
  
  //Set u1Period (440 Hz) as the period of PWM0
  PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, ulPeriod);
  
  //Set PWM0, output 1 to a duty cycle of 1/8
  PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, ulPeriod / 16);
  
  //Activate PWM0
  PWMGenEnable(PWM0_BASE, PWM_GEN_0);   
  PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
  
  return;
}

void setupOutput(void) {
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
  GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, PORT_DATA);
  return;
}

void pin(bool status)
{
  if(status)
    GPIOPinWrite(GPIO_PORTD_BASE, PORT_DATA, 0x20);
  else
    GPIOPinWrite(GPIO_PORTD_BASE, PORT_DATA, 0x00);
  return;
}
