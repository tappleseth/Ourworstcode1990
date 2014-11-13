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
unsigned int TrainState;
unsigned int TimerState;
unsigned int seed = 1;
unsigned int tempCount = 0;
unsigned int frequencyCount = 0;
unsigned char globalCountArray[10];
unsigned char passCountArray[4];
//unsigned char numCars[2];

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
  
  /*GPIO PORT D INTERRUPT SETUP*/
    //Clear the default ISR handler and install IntGPIOe as the handler:
  GPIOPortIntUnregister(GPIO_PORTD_BASE);
  GPIOPortIntRegister(GPIO_PORTD_BASE,IntGPIOd);
  
  //Enable GPIO port E, set pin 0 as an input
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);    
  GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, 0x20);
  
  //Activate the pull-up on GPIO port E
  GPIOPadConfigSet(GPIO_PORTD_BASE, 0x20, GPIO_STRENGTH_2MA,
                   GPIO_PIN_TYPE_STD_WPU);
  
  //Configure GPIO port E as triggering on falling edges
  GPIOIntTypeSet(GPIO_PORTD_BASE, 0x20, GPIO_FALLING_EDGE);
  
  //Enable interrupts for GPIO port E
  GPIOPinIntEnable(GPIO_PORTD_BASE, 0x20);
  IntEnable(INT_GPIOD);
  /*GPIO PORT D INTERRUPT SETUP*/  
  
  /*
  
  UART STUFF ***
  
  */
  
  
  //
    // Enable the peripherals used by this example.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable processor interrupts.
    //
    IntMasterEnable();

    //
    // Set GPIO A0 and A1 as UART pins.
    //
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Configure the UART for 115,200, 8-N-1 operation.
    //
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));

    //
    // Enable the UART interrupt.
    //
    IntEnable(INT_UART0);
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
/*
    
    END UART MADNESS
    
*/
  TrainState = 0;
  TimerState = 0;
  
  
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
  
  
  RIT128x96x4Init(1000000); 
  
  head = NULL;
  head->next = NULL;
  
  static char flairTitle1[] = "Applehansontaft \0";
  static char flairTitle2[] = "Discount Freight \0";
  RIT128x96x4StringDraw(flairTitle1, 10, 10, 15);
  RIT128x96x4StringDraw(flairTitle2, 10, 20, 15);

  //build initial stack
 // addToStack(&serialThingyHell);
  addToStack(&trainComHell);
  
  static unsigned int justinCrazy = 0;
 
  for (int tibo = 0; tibo < 9; tibo++)
    globalCountArray[tibo] = ' ';
    globalCountArray[9] = '\0'; 
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
    TimerState=0;
    frequencyCount = tempCount;
    tempCount = 0;
      justinCrazy = ourGlobalData.globalCount;
      justinCrazy++;
      ourGlobalData.globalCount = justinCrazy;
      char scheduleTitle[] = "AHT Time: \0";
      RIT128x96x4StringDraw(scheduleTitle, 0, 80, 15);
     
      
  int i = 9; 
  
  while(justinCrazy > 0) {
    globalCountArray[i] = (justinCrazy%10) + 48;
    justinCrazy = justinCrazy/10;
    i--;     
  }
  
  
  RIT128x96x4StringDraw((char const*)globalCountArray, 65, 80, 15);
    
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

void IntGPIOd(void)
{
  //Clear the interrupt to avoid continuously looping here
  GPIOPinIntClear(GPIO_PORTD_BASE, 0x20);
  
  tempCount+=2;
}
