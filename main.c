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
  
  TimerState = 0;
  
  // Set the clocking to run directly from the crystal.
  SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                 SYSCTL_XTAL_8MHZ);
  
  //Clear the default ISR handler and install IntGPIOe as the handler:
  GPIOPortIntUnregister(GPIO_PORTE_BASE);
  GPIOPortIntRegister(GPIO_PORTE_BASE,IntGPIOe);
  
  //Enable GPIO port E, set pin 0 as an input
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);    
  GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, 0xF);
  
  //Activate the pull-up on GPIO port E
  GPIOPadConfigSet(GPIO_PORTE_BASE, 0xF, GPIO_STRENGTH_2MA,
                   GPIO_PIN_TYPE_STD_WPU);
  
  //Configure GPIO port E as triggering on falling edges
  GPIOIntTypeSet(GPIO_PORTE_BASE, 0xF, GPIO_FALLING_EDGE);
  
  //Enable interrupts for GPIO port E
  GPIOPinIntEnable(GPIO_PORTE_BASE, 0xF);
  IntEnable(INT_GPIOE);

  
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
  currentTrainData ourCurrentTrainData;
  
  globalData ourGlobalData;
  ourGlobalData.east = FALSE;
  ourGlobalData.stateTom = 0;
  ourGlobalData.fromDirection = 0; 
  ourGlobalData.globalCount = 0;
  ourGlobalData.gridlock = FALSE;
  ourGlobalData.north = FALSE;
  ourGlobalData.startTime = 0;
  ourGlobalData.trainPresent = FALSE;
  ourGlobalData.trainSize = 0;
  ourGlobalData.traversalTime = 0;
  ourGlobalData.west = FALSE;
  ourGlobalData.currentTrainComplete = FALSE;
  ourGlobalData.switchConComplete = FALSE;
  ourGlobalData.trainComComplete = FALSE;
  
  
  //data struct fields
  ourTrainComData.east = &east;
  ourTrainComData.north = &north;
  ourTrainComData.west = &west;
  ourTrainComData.trainSize = &trainSize;
  ourTrainComData.trainPresent = &trainPresent;
  ourTrainComData.gridlock = &gridlock;
  
  
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
  
  TimerState = 1;
  
  return;
}

//stolen wholesale from professor's provided code
void IntGPIOe(void)
{
  //Clear the interrupt to avoid continuously looping here
  GPIOPinIntClear(GPIO_PORTE_BASE, 0xF);
  
  //Set the Event State for GPIO pin 0
  TrainState=GPIOPinRead(GPIO_PORTE_BASE, 0xF);
  
  //Switches are normally-high, so flip the polarity of the results:
  TrainState=TrainState^0xF;  //You should work out why and how this works!
}
