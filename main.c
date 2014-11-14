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
#include "driverlib/uart.h"

boringTom* head;
boringTom* current;
int TrainState;
int TimerState;
int seed = 1;
unsigned int tempCount;
unsigned int frequencyCount;
//unsigned char globalCountArray[10];
//unsigned char passCountArray[4];
//unsigned char numCars[2];


int main(){
  Startup();
  //OKAY YOU FRAUDS, TIME TO IMPLEMENT AN ACTUAL STACK THINGY.
  TomSchedule(); 
  //THAT WAS FKIN EASY
  return 0;
}

void Startup(void) {
  
 
  //STEP 1: OLED and PWM setup
  unsigned long ulPeriod;
  SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);
  RIT128x96x4Init(1000000);
  SysCtlPWMClockSet(SYSCTL_PWMDIV_1);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
  GPIOPinTypePWM(GPIO_PORTG_BASE, GPIO_PIN_1);
  ulPeriod = SysCtlClockGet() / 440;
  PWMGenConfigure(PWM0_BASE, PWM_GEN_0,PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
  PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, ulPeriod);
  PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, ulPeriod / 16);
  PWMGenEnable(PWM0_BASE, PWM_GEN_0);   
  
  //STEP 2: Timer setup
  //SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);
  TimerState = 0;
  TimerIntUnregister(TIMER0_BASE, TIMER_A);
  TimerIntRegister(TIMER0_BASE, TIMER_A, IntTimer0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
  TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
  TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet());    
  IntEnable(INT_TIMER0A);
  TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
  TimerEnable(TIMER0_BASE, TIMER_A);
  
 // SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);
  
  //STEP 3: Button pad setup
  TrainState=0;
  GPIOPortIntUnregister(GPIO_PORTE_BASE);
  GPIOPortIntRegister(GPIO_PORTE_BASE,IntGPIOe);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);    
  GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2| GPIO_PIN_3 );
  GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2| GPIO_PIN_3 , GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
  GPIOIntTypeSet(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2| GPIO_PIN_3 , GPIO_FALLING_EDGE);
  GPIOPinIntEnable(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2| GPIO_PIN_3 );
  IntEnable(INT_GPIOE);
  
  //STEP 4: Frequency count setup
  tempCount = 0;
  frequencyCount = 0;
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);       
  GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_3);
  //GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
  GPIOPortIntUnregister(GPIO_PORTF_BASE);
  GPIOPortIntRegister(GPIO_PORTF_BASE,portFcrap);
  GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_RISING_EDGE);
  GPIOPinIntEnable(GPIO_PORTF_BASE, GPIO_PIN_3);
  IntEnable(INT_GPIOF);
  
  //STEP 5: UART setup

  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
                      (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                       UART_CONFIG_PAR_NONE));
  IntEnable(INT_UART0); //fails here
  UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

  return;
}

void TomSchedule(){
  
  //data structs
  trainComData ourTrainComData;
  switchControlData ourSwitchControlData;
  currentTrainData ourCurrentTrainData;
  serialCommunicationsData ourSerialCommunicationsData;
  
  ourCurrentTrainData.toggleEast = FALSE;
  ourCurrentTrainData.toggleNorth = FALSE;
  ourCurrentTrainData.toggleSouth = FALSE;
  ourCurrentTrainData.toggleWest = FALSE;
  
  ourSerialCommunicationsData.data = FALSE;
  
  globalData ourGlobalData;
  ourGlobalData.east = FALSE;
  ourGlobalData.stateTom = 0;
  ourGlobalData.globalCount = 0;
  ourGlobalData.gridlock = FALSE;
  ourGlobalData.north = FALSE;
  ourGlobalData.passengerCount = 0;
  ourGlobalData.startTime = 0;
  ourGlobalData.trainPresent = FALSE;
  ourGlobalData.trainSize = 0;
  ourGlobalData.traversalTime = 0;
  ourGlobalData.west = FALSE;
  ourGlobalData.south = FALSE;
  ourGlobalData.currentTrainComplete = FALSE;
  ourGlobalData.switchConComplete = FALSE;
  ourGlobalData.trainComComplete = FALSE;
  ourGlobalData.fromDirection = 'X';
  ourGlobalData.gridlockTime = 0;
  
  
  //pointer structs
  boringTom trainComHell;
  boringTom currentTrainHell;
  boringTom switchControlHell;
  boringTom serialThingyHell;
  
  trainComHell.justTrainTaskThings = NuTrainCom;
  trainComHell.localDataPtr = (void*)&ourTrainComData;
  trainComHell.globalDataPtr = (void*)&ourGlobalData;
  trainComHell.next = NULL;
  
  currentTrainHell.justTrainTaskThings = CurrentTrain;
  currentTrainHell.localDataPtr = (void*)&ourCurrentTrainData;
  currentTrainHell.globalDataPtr = (void*)&ourGlobalData;
  trainComHell.next = NULL;
  
  switchControlHell.justTrainTaskThings = NuSwitchControl;
  switchControlHell.localDataPtr = (void*)&ourSwitchControlData;
  switchControlHell.globalDataPtr = (void*)&ourGlobalData;
  trainComHell.next = NULL;
  
  serialThingyHell.justTrainTaskThings = SerialComTask;
  serialThingyHell.localDataPtr = (void*)&ourSerialCommunicationsData;
  serialThingyHell.globalDataPtr = (void*)&ourGlobalData;
  serialThingyHell.next = NULL;
  
  
  head = NULL;
  head->next = NULL;
  RIT128x96x4Init(1000000);
  static char flairTitle1[] = "Applehansontaft \0";
  static char flairTitle2[] = "Discount Freight \0";
  RIT128x96x4StringDraw(flairTitle1, 10, 10, 15);
  RIT128x96x4StringDraw(flairTitle2, 10, 20, 15);

  //build initial stack
  addToStack(&serialThingyHell);
  addToStack(&trainComHell);
  
  static unsigned int justinCrazy = 0;
  
  static char globalCountArray[] = "         \0"; 
  
  /*for (int tibo = 0; tibo < 9; tibo++){
    globalCountArray[tibo] = ' ';
  }*/
  
  
  while(1){
    //traverse stack
    current = head;
    if (ourGlobalData.trainComComplete==TRUE){
      //if trainCom has done its job, pop trainCom and push SwitchCon and CurrentTrain
      TrainState = 0;
      popFromStack(); 
      addToStack(&currentTrainHell);
      addToStack(&switchControlHell);
      ourGlobalData.trainComComplete=FALSE;
    }
    //if switchCon says traversalTime is over with
    if (ourGlobalData.switchConComplete==TRUE){
      ourGlobalData.switchConComplete=FALSE; //reset for next time
      popFromStack(); //remove switchCon
    }
    //if currentTrain says traversalTime is over with
    if (ourGlobalData.currentTrainComplete==TRUE){
      ourGlobalData.currentTrainComplete=FALSE; //reset for next time
      popFromStack(); //remove currentTrain
       //reset TrainState in anticipation of next train
      addToStack(&trainComHell);
    }
    while (current != NULL){
      current->justTrainTaskThings(current->localDataPtr,current->globalDataPtr);
      current = current->next;
    }
    while (!TimerState) {
      
      }
    TimerState = 0;
    frequencyCount = tempCount;
    tempCount = 0;
      justinCrazy = ourGlobalData.globalCount;
      justinCrazy++;
      ourGlobalData.globalCount = justinCrazy;
    
      RIT128x96x4StringDraw("AHT Time: \0", 0, 80, 15);
     
      
  int i = 9; 
  
  while(justinCrazy > 0) {
    globalCountArray[i] = (justinCrazy%10) + 48;
    justinCrazy = justinCrazy/10;
    i--;     
  }
  
  
  RIT128x96x4StringDraw(globalCountArray, 65, 80, 15);
    
  }
  
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
  GPIOPinIntClear(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2| GPIO_PIN_3 );
  
  //Set the Event State for GPIO pin 0
  TrainState=GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2| GPIO_PIN_3 );
  
  //Switches are normally-high, so flip the polarity of the results:
  TrainState=TrainState^0xF;  //You should work out why and how this works!
}

void portFcrap(void)
{
  //Clear the interrupt to avoid continuously looping here
  GPIOPinIntClear(GPIO_PORTF_BASE, GPIO_PIN_3);
  tempCount++;
}
