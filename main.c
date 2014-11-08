#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "headerThingy.h"
#include "proj3Stuff.c"
#include "currentTrain.c"

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
void IntGPIOe(void);
void TomSchedule(void);
unsigned int State = 0;

int main(){
  Startup();
  //OKAY YOU FRAUDS, TIME TO IMPLEMENT AN ACTUAL STACK THINGY.
  TomSchedule(); 
  //THAT WAS FKIN EASY
  return 0;
}

void Startup(void) {
  // Set the clocking to run directly from the crystal.
  SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                 SYSCTL_XTAL_8MHZ);
  
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
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
  GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, PORT_DATA); 
  
  State = 0;
  
  // Set the clocking to run directly from the crystal.
  SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                 SYSCTL_XTAL_8MHZ);
  
  //Clear the default ISR handler and install IntGPIOe as the handler:
  GPIOPortIntUnregister(GPIO_PORTE_BASE);
  GPIOPortIntRegister(GPIO_PORTE_BASE,IntGPIOe);
  
  //Enable GPIO port E, set pin 0 as an input
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);    
  GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_0);
  
  //Activate the pull-up on GPIO port E
  GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA,
                   GPIO_PIN_TYPE_STD_WPU);
  
  //Configure GPIO port E as triggering on falling edges
  GPIOIntTypeSet(GPIO_PORTE_BASE, GPIO_PIN_0, GPIO_FALLING_EDGE);
  
  //Enable interrupts for GPIO port E
  GPIOPinIntEnable(GPIO_PORTE_BASE, GPIO_PIN_0);
  IntEnable(INT_GPIOE);
  
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

void TomSchedule(){
  
  //data structs
  trainComData ourTrainComData;
  northTrainData ourNorthTrainData;
  westTrainData ourWestTrainData;
  eastTrainData ourEastTrainData;
  switchControlData ourSwitchControlData;
  scheduleData ourScheduleData;
  
 globalData ourGlobalData;
  ourGlobalData.east = FALSE;
  ourGlobalData.globalCount = 0;
  ourGlobalData.gridlock = FALSE;
  ourGlobalData.north = FALSE;
  ourGlobalData.startTime = 0;
  ourGlobalData.trainPresent = FALSE;
  ourGlobalData.trainSize = 0;
  ourGlobalData.traversalTime = 0;
  ourGlobalData.west = FALSE;
  
  //data struct fields
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
  
  //pointer structs
  boringTom trainComHell;
  boringTom currentTrainHell;
  boringTom switchControlHell;
  boringTom serialThingyHell;
  
  trainComHell.justTrainTaskThings = NuTrainCom;
  trainComHell.localDataPtr = (void*)&ourTrainComData;
  trainComHell.globalDataPtr = (void*)&ourGlobalData;
  
  currentTrainHell.justTrainTaskThings = CurrentTrain;
  currentTrainHell.localDataPtr = (void*)&ourTrainComData;
  currentTrainHell.globalDataPtr = (void*)&ourGlobalData;
  
  RIT128x96x4Init(1000000); 
  
  boringTom* head; 
  head->next = NULL;
  head->previous = NULL;
  
  static char flairTitle1[] = "Applehansontaft \0";
  static char flairTitle2[] = "Discount Freight \0";
  RIT128x96x4StringDraw(flairTitle1, 10, 10, 15);
  RIT128x96x4StringDraw(flairTitle2, 10, 20, 15);
 static int checkSize = 0;
  while(1){
    checkSize = getStackSize();
    if (checkSize==0){
      RIT128x96x4StringDraw("Justin is an abomination \0", 10, 40, 15);
    } else {
      RIT128x96x4StringDraw((char*)checkSize, 10, 40, 15);
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

void Schedule(void* data) {
#if TASK_SELECT == 5 || TASK_SELECT == -1
  pin(HIGH);
#endif
  
  static unsigned int justinCrazy;
  static char globalCountArray[10];
  
  for (int tibo = 0; tibo < 10; tibo++)
    globalCountArray[tibo] = ' ';
  
  scheduleData* tomPtr = (scheduleData*) data; 
  
  while(State == 0) {
    
  }
  
  State = 0;
  
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
#if SOUND_ENABLE == 1
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
#endif
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
      if ((*ptr->globalCount - westFlashCount) % 24 == 0)
        brightness = 15; 
      
      if ((*ptr->globalCount - westFlashCount) % 24 == 12)
        brightness = 0;
      
      RIT128x96x4StringDraw(westDisplay, 10, 40, brightness); 
      
      //SOUND EVENTS
#if SOUND_ENABLE == 1      
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
#endif
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

void EastTrain(void* data) {
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
      
      if ((*ptr->globalCount - eastFlashCount) % 12 == 0)
        brightness = 15; 
      
      if ((*ptr->globalCount - eastFlashCount) % 12 == 6)
        brightness = 0;
      
      RIT128x96x4StringDraw(eastDisplay, 10, 40, brightness); 
      
      //*****SOUND EVENTS*****
#if SOUND_ENABLE == 1
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
#endif
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

//stolen wholesale from professor's provided code
void IntGPIOe(void)
{
  //Clear the interrupt to avoid continuously looping here
  GPIOPinIntClear(GPIO_PORTE_BASE, GPIO_PIN_0);
  
  //Set the Event State for GPIO pin 0
  State=GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_0);
  
  //Switches are normally-high, so flip the polarity of the results:
  State=~State;  //You should work out why and how this works!
}
