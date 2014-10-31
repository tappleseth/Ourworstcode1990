#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "headerThingy.h"

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "inc/lm3s8962.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "drivers/rit128x96x4.h"
#include "driverlib/timer.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/interrupt.h"

void IntTimer0(void);
int randomInteger(int a, int b);
void Delay(int* foolioJenkins);
void pin(bool);

unsigned int State = 0;

int main(){
  Startup();
  
  globalData ourGlobalData;
  
  trainComData ourTrainComData;
  northTrainData ourNorthTrainData;
  westTrainData ourWestTrainData;
  eastTrainData ourEastTrainData;
  switchControlData ourSwitchControlData;
  scheduleData ourScheduleData;

  /*GLOBAL VARIABLES*/
  ourGlobalData.north = &north;
  ourGlobalData.east = &east;
  ourGlobalData.west = &west;
  ourGlobalData.gridlock = &gridlock;
  ourGlobalData.trainPresent = &trainPresent;  
  ourGlobalData.globalCount = &globalCount;
  ourGlobalData.trainSize = &trainSize;
  ourGlobalData.traversalTime = &traversalTime;
  ourGlobalData.startTime = &startTime;
  /*END GLOBAL VARIABLES*/
  
  /*LOCAL VARIABLES*/
  ourNorthTrainData.toggleNorth = &toggleNorth;
  ourWestTrainData.toggleWest = &toggleWest;
  ourEastTrainData.toggleEast = &toggleEast;
  /*END LOCAL VARIABLES*/
  
  TCB tomQueue[MAX_QUEUE_LENGTH];
  
  tomQueue[0].justTrainTaskThings = TrainCom;
  tomQueue[0].globalDataPtr = (void*)&ourGlobalData;
  tomQueue[0].localDataPtr = (void*)&ourTrainComData;  
  tomQueue[1].justTrainTaskThings = SwitchControl;
  tomQueue[1].globalDataPtr = (void*)&ourGlobalData;
  tomQueue[1].localDataPtr = (void*)&ourSwitchControlData;  
  tomQueue[2].justTrainTaskThings = NorthTrain;
  tomQueue[2].globalDataPtr = (void*)&ourGlobalData;
  tomQueue[2].localDataPtr = (void*)&ourNorthTrainData;
  tomQueue[3].justTrainTaskThings = WestTrain;
  tomQueue[3].globalDataPtr = (void*)&ourGlobalData;
  tomQueue[3].localDataPtr = (void*)&ourWestTrainData;
  tomQueue[4].justTrainTaskThings = EastTrain;
  tomQueue[4].globalDataPtr = (void*)&ourGlobalData;
  tomQueue[4].localDataPtr = (void*)&ourEastTrainData;
  tomQueue[5].justTrainTaskThings = Schedule;
  tomQueue[5].globalDataPtr = (void*)&ourGlobalData;
  tomQueue[5].localDataPtr = (void*)&ourScheduleData;
  
  RIT128x96x4Init(1000000); 
  
  //it breaks HERE? WHAT? NOTE TO SELF: WHAAT?
  static char flairTitle1[] = "Applehansontaft \0";
  static char flairTitle2[] = "Discount Freight \0";
  RIT128x96x4StringDraw(flairTitle1, 10, 10, 15);
  RIT128x96x4StringDraw(flairTitle2, 10, 20, 15);
  
  while(1) {
    for(int i = 0; i < MAX_QUEUE_LENGTH; i++) {
      tomQueue[i].justTrainTaskThings(tomQueue[i].localDataPtr, tomQueue[i].globalDataPtr);
    }    
  }  
}

void Startup(void) {
  /*SYSTEM CONTROL CONFIGURATION*/
  //Set the clocking to run directly from the crystal.
  SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                 SYSCTL_XTAL_8MHZ);
  /*END SYSTEM CONTROL CONFIGURATION*/

  /*TIMER CONFIGURATION*/    
  //Clear the default ISR handler and install IntTimer0 as the handler:
  TimerIntUnregister(TIMER0_BASE, TIMER_A);
  TimerIntRegister(TIMER0_BASE, TIMER_A, IntTimer0);
  //Enable Timer 0    
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
  //Configure Timer 0 and set the timebase to 1 second    
  TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
  TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet());    
  //Enable interrupts for Timer0 and activate it
  IntEnable(INT_TIMER0A);
  TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
  TimerEnable(TIMER0_BASE, TIMER_A);  
  /*END TIMER CONFIGURATION*/
  
  /*PWM CONFIGURATION*/
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
  /*END PWM CONFIGURATION*/
  
  /*GPIO D CONFIGURATION*/
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
  GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, PORT_DATA); 
  /*END GPIO D CONFIGURATION*/
  
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
  
  return;
}

void TrainCom(void* localData, void* sharedData) {
  #if TASK_SELECT == 0 || TASK_SELECT == -1
    pin(HIGH);
  #endif
  
  int direction;
  globalData* globalPtr = (globalData*)sharedData;
  trainComData* localPtr = (trainComData*)localData;
  static int brightness;
  
  //Preparing a trainSize in preparation for switchControl
  //(when switchControl decides there is a train, it will use this size)
  if (!*globalPtr->trainPresent && !*globalPtr->gridlock) {
    *globalPtr->trainSize = randomInteger(2, 9);
    brightness = 15;
  }
  
  if(*globalPtr->gridlock)
    brightness = 0;
  
  char numCars[] = {(char)(48 + *globalPtr->trainSize), '\0'};
  RIT128x96x4StringDraw("Cars: \0", 10, 50, brightness);
  RIT128x96x4StringDraw(numCars, 50, 50, brightness);
  
  if (*globalPtr->trainPresent && !*globalPtr->east && !*globalPtr->west && !*globalPtr->north) {
    direction = randomInteger(0, 2);
    
    if(direction == 1) {
      *globalPtr->east = TRUE;
      *globalPtr->north = FALSE;    
      *globalPtr->west = FALSE;
    }
    else if(direction == 2) {
      *globalPtr->east = FALSE;
      *globalPtr->north = TRUE;    
      *globalPtr->west = FALSE;
    }
    else {
      *globalPtr->east = FALSE;
      *globalPtr->north = FALSE;    
      *globalPtr->west = TRUE;
    }    
  }
  
  #if TASK_SELECT == 0 || TASK_SELECT == -1
    pin(LOW);
  #endif
  
  return;
}

void CurrentTrain(void* localData, void* globalData) {
  //produce OLED/audio alarms
  //remove from task queue if no train is present
}

void SwitchControl(void* localData, void* sharedData) {
  #if TASK_SELECT == 1 || TASK_SELECT == -1
    pin(HIGH);
  #endif
  
  globalData* globalPtr = (globalData*)sharedData;
  switchControlData* localPtr = (switchControlData*)localData;
  static int rand = 0;
  static char brando9k[] = "GRIDLOCK! \0";
  static int brightness = 15;
  
  if(!*globalPtr->gridlock && !*globalPtr->trainPresent) {
    //startTime is logged as whatever current globalCount value is
    *globalPtr->startTime = *globalPtr->globalCount;
    //generate a random number between -2 and 2
    rand = randomInteger(-2, 2);
    
    //if random number is less than 0, we have GRIDLOCK!
    if(rand < 0) {
      *globalPtr->gridlock = TRUE;
      *globalPtr->traversalTime = *globalPtr->globalCount + (-70*rand);
    }
    //if random number is greater than 0, we have a train passing through!
    else {
      *globalPtr->trainPresent = TRUE;
      *globalPtr->traversalTime = 35*(*globalPtr->trainSize) + *globalPtr->globalCount;
    }
  }
  //CASE 2
  //gridlock is true
  if(*globalPtr->gridlock) {
    //if globalCount exceeds gridlock timer, end gridlock
    if ((*globalPtr->globalCount - *globalPtr->traversalTime) % 6 == 0)
      brightness = 15; 
    
    if ((*globalPtr->globalCount - *globalPtr->traversalTime) % 6 == 3)
      brightness = 0;
    
    RIT128x96x4StringDraw(brando9k, 10, 40, brightness); 
    
    if(*globalPtr->globalCount >= *globalPtr->traversalTime)
      *globalPtr->gridlock = FALSE;
  }
  
  //CASE 3
  //train is present
  if(*globalPtr->trainPresent) {
    if(*globalPtr->globalCount >= *globalPtr->traversalTime) {
      //*globalPtr->trainSize = 0;
      *globalPtr->trainPresent = FALSE;
      *globalPtr->north = FALSE;
      *globalPtr->east = FALSE;
      *globalPtr->west = FALSE;
    }
  }
  
  #if TASK_SELECT == 1 || TASK_SELECT == -1
    pin(LOW);
  #endif
  
  return;
}

void SerialCommunications(void* localData, void* sharedData) {
  //mirror OLED display
  //remove if not needed (condition?)
}

void Schedule(void* localData, void* sharedData) {
  #if TASK_SELECT == 5 || TASK_SELECT == -1
    pin(HIGH);
  #endif
    
  static unsigned int justinCrazy;
  static char globalCountArray[10];
  
  for (int tibo = 0; tibo < 10; tibo++)
    globalCountArray[tibo] = ' ';
  
  globalData* globalPtr = (globalData*)sharedData;
  scheduleData* localPtr = (scheduleData*)localData; 
  
  while(State == 0) {
    
  }
  
  State = 0;
  
  justinCrazy = *globalPtr->globalCount;
  justinCrazy++;
  *globalPtr->globalCount = justinCrazy;
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

void NorthTrain(void* localData, void* sharedData) {
  #if TASK_SELECT == 2 || TASK_SELECT == -1
    pin(HIGH);
  #endif
    
  globalData* globalPtr = (globalData*)sharedData;    
  northTrainData* localPtr = (northTrainData*)localData;
  
  static unsigned int noiseCount = 0;
  static unsigned int northFlashCount = 0;  
  static char northDisplay[] = "NorthTrain \0";
  static int brightness = 15;
  
  if(*globalPtr->north) {
    if(!*localPtr->toggleNorth){
      *localPtr->toggleNorth = TRUE;
      noiseCount = *globalPtr->globalCount + 60;
      northFlashCount = *globalPtr->globalCount;
      brightness = 15;
    }
    
    if(*localPtr->toggleNorth) {
      //FLASH EVENTS
      //loops indefinitely (will end when train passes)
      if((*globalPtr->globalCount - northFlashCount) % 18 == 0)
        brightness = 15; 
      
      if((*globalPtr->globalCount - northFlashCount) % 18 == 9)
        brightness = 0;
      
      RIT128x96x4StringDraw(northDisplay, 10, 40, brightness); 
      //END FLASH EVENTS
      
      //SOUND EVENTS
      #if SOUND_ENABLE == 1
        //first blast: lasts 2 seconds
        if(*globalPtr->globalCount == (noiseCount - 60))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //off time between first and second blast
        if(*globalPtr->globalCount == (noiseCount - 48))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        //wait one second, then start second blast
        if(*globalPtr->globalCount == (noiseCount - 42))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //wait two seconds, stop second blast
        if(*globalPtr->globalCount == (noiseCount - 30))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        //wait one second, then start first short blast
        if(*globalPtr->globalCount == (noiseCount - 24))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //wait one second, then stop first short blast
        if(*globalPtr->globalCount == (noiseCount - 18))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        //wait one second, then start second short blast
        if(*globalPtr->globalCount == (noiseCount - 12))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //wait one second, end all blasts, set bigglesOn to FALSE
        if(*globalPtr->globalCount == (noiseCount - 6))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        if(noiseCount == *globalPtr->globalCount) {
          noiseCount = *globalPtr->globalCount + 60;
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        }      
      #endif
    }
  }
  
  //placeholder: need code to STOP SOME SHIT
  if(*globalPtr->globalCount >= *globalPtr->traversalTime) {
    PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
    noiseCount = 0;
    northFlashCount = 0;
    brightness = 15;
    *localPtr->toggleNorth = FALSE;
    RIT128x96x4StringDraw(northDisplay, 10, 40, 0);
  }
  
  #if TASK_SELECT == 2 || TASK_SELECT == -1
    pin(LOW); 
  #endif
  
  return;
}

void WestTrain(void* localData, void* sharedData) {
  #if TASK_SELECT == 3 || TASK_SELECT == -1
    pin(HIGH);
  #endif

  globalData* globalPtr = (globalData*)sharedData;
  westTrainData* localPtr = (westTrainData*)localData;  
  
  static unsigned int westNoiseCount = 0;
  static unsigned int westFlashCount = 0;
  static char westDisplay[] = "WestTrain \0";
  static int brightness = 15;  
  
  if(*globalPtr->west) {
    if(!*localPtr->toggleWest) {
      *localPtr->toggleWest = TRUE;
      westNoiseCount = *globalPtr->globalCount + 42;
      westFlashCount = *globalPtr->globalCount;
      brightness = 15;
    }
    
    //FLASH EVENTS
    if(*localPtr->toggleWest) {
      if ((*globalPtr->globalCount - westFlashCount) % 24 == 0)
        brightness = 15; 
      
      if ((*globalPtr->globalCount - westFlashCount) % 24 == 12)
        brightness = 0;
      
      RIT128x96x4StringDraw(westDisplay, 10, 40, brightness); 
      
      //SOUND EVENTS
      #if SOUND_ENABLE == 1      
        //first blast: lasts 2 seconds
        if(*globalPtr->globalCount == (westNoiseCount - 42))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //off time between first and second blast
        if(*globalPtr->globalCount == (westNoiseCount - 30))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        //wait one second, then start second blast
        if(*globalPtr->globalCount == (westNoiseCount - 24))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //wait two seconds, stop second blast
        if(*globalPtr->globalCount == (westNoiseCount - 18))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        //wait one second, then start first short blast
        if(*globalPtr->globalCount == (westNoiseCount - 12))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //wait one second, then stop first short blast
        if(*globalPtr->globalCount == (westNoiseCount - 6))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);    
        
        if(westNoiseCount == *globalPtr->globalCount) {
          westNoiseCount = *globalPtr->globalCount + 42;
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        }
      #endif
    }
  }
  
  if(*globalPtr->globalCount >= *globalPtr->traversalTime) {
    PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
    westNoiseCount = 0;
    westFlashCount = 0;
    brightness = 15;
    *localPtr->toggleWest = FALSE;
    RIT128x96x4StringDraw(westDisplay, 10, 40, 0);
  }
  
  #if TASK_SELECT == 3 || TASK_SELECT == -1
    pin(LOW);
  #endif
  
  return;
}

void EastTrain(void* localData, void* sharedData) {
  #if TASK_SELECT == 4 || TASK_SELECT == -1
    pin(HIGH);
  #endif
    
  globalData* globalPtr = (globalData*)sharedData;  
  eastTrainData* localPtr = (eastTrainData*)localData;  
  
  static unsigned int eastNoiseCount = 0;
  static unsigned int eastFlashCount = 0;
  static char eastDisplay[] = "EastTrain \0";
  static int brightness = 15;
  
  if(*globalPtr->east) {
    if(!*localPtr->toggleEast) {
      *localPtr->toggleEast = TRUE;
      eastNoiseCount = *globalPtr->globalCount + 78;
      eastFlashCount = *globalPtr->globalCount;
      brightness = 15;
    }
    
    //*****FLASH EVENTS*****  
    
    if(*localPtr->toggleEast) {
      //FLASH OFF
      
      if ((*globalPtr->globalCount - eastFlashCount) % 12 == 0)
        brightness = 15; 
      
      if ((*globalPtr->globalCount - eastFlashCount) % 12 == 6)
        brightness = 0;
      
      RIT128x96x4StringDraw(eastDisplay, 10, 40, brightness); 
      
      //*****SOUND EVENTS*****
      #if SOUND_ENABLE == 1
        //first blast: lasts 2 seconds
        if(*globalPtr->globalCount == (eastNoiseCount - 78)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //off time between first and second blast
        if(*globalPtr->globalCount == (eastNoiseCount - 66)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        //wait one second, then start second blast
        if(*globalPtr->globalCount == (eastNoiseCount - 60)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //wait two seconds, stop second blast
        if(*globalPtr->globalCount == (eastNoiseCount - 48)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        //wait one second, then start first short blast
        if(*globalPtr->globalCount == (eastNoiseCount - 42)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //wait one second, then stop first short blast
        if(*globalPtr->globalCount == (eastNoiseCount - 30)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        //first blast: lasts 2 seconds
        if(*globalPtr->globalCount == (eastNoiseCount - 24)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //off time between first and second blast
        if(*globalPtr->globalCount == (eastNoiseCount - 18)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        //wait one second, then start second blast
        if(*globalPtr->globalCount == (eastNoiseCount - 12))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //wait two seconds, stop second blast
        if(*globalPtr->globalCount == (eastNoiseCount - 6))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        if(eastNoiseCount == *globalPtr->globalCount) {
          eastNoiseCount = *globalPtr->globalCount + 78;
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        }
      #endif
    }   
  }
  
  if(*globalPtr->globalCount >= *globalPtr->traversalTime) {
    PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
    eastNoiseCount = 0;
    eastFlashCount = 0;
    brightness = 15;
    *localPtr->toggleEast = FALSE;
    RIT128x96x4StringDraw(eastDisplay, 10, 40, 0);
  }

  #if TASK_SELECT == 4 || TASK_SELECT == -1
    pin(LOW);
  #endif
  
  return;
}

int randomInteger(int low, int high) {
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

void pin(bool status) {
  if(status)
    GPIOPinWrite(GPIO_PORTD_BASE, PORT_DATA, 0x20);
  else
    GPIOPinWrite(GPIO_PORTD_BASE, PORT_DATA, 0x00);
  return;
}

void IntTimer0(void) {
  //Clear the interrupt to avoid continuously looping here
  TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
  
  State = 1;
  
  return;
}
