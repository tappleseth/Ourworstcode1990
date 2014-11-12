#include "headerThingy.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "inc/lm3s8962.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "drivers/rit128x96x4.h"
#include "driverlib/timer.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/interrupt.h"
#include "driverlib/uart.h"

void addToStack(boringTom* addMe){
 // head->previous = addMe;
  addMe->next = head;
  head = addMe;
}

void popFromStack(){
  //boringTom* lynnsIdea;
  //lynnsIdea = head->next;
  //head = lynnsIdea;
  head = head->next;
  //head->previous = NULL;
}

int getStackSize(){
  boringTom* traverseStack;
  traverseStack = head;
  int countElements;
  countElements = 0;
  while (traverseStack != NULL){
    countElements++;
    traverseStack = traverseStack->next;
  }
  return countElements;
}

void NuTrainCom(void* localData, void* sharedData){
  globalData* globalPtr = (globalData*)sharedData;    
  trainComData* localPtr = (trainComData*)localData;
  int direction = 0;
  
   //IF !trainPresent and some button was pressed, THEN generate train
  if ((TrainState == 1 || TrainState == 2 || TrainState == 4 || TrainState == 8)&&(globalPtr->trainPresent==FALSE)) {
   globalPtr->trainSize = 0;
    
    
    RIT128x96x4StringDraw("I'm a dummy!!!!!!!! \0", 10, 30, 0);
    //step 0: set trainPresent to true
    globalPtr->trainPresent = TRUE;
    
    //step 1: set from direction
    if (TrainState == 1){
      globalPtr->fromDirection = 'N';
      RIT128x96x4StringDraw("From: North \0", 10, 30, 15);
    } else if (TrainState == 2){
      globalPtr->fromDirection = 'S';
      RIT128x96x4StringDraw("From: South \0", 10, 30, 15);
    }  else if (TrainState == 4){
      globalPtr->fromDirection = 'W';
      RIT128x96x4StringDraw("From: West \0", 10, 30, 15);
    } else {
      globalPtr->fromDirection = 'E';
      RIT128x96x4StringDraw("From: East \0", 10, 30, 15);
    }
    
    // step 2: generate TO train direction
    direction = randomInteger(0, 3);
    if(direction == 0) {
      globalPtr->east = TRUE;
      globalPtr->north = FALSE;    
      globalPtr->west = FALSE;
      globalPtr->south = FALSE;
    }
    else if(direction == 1) {
      globalPtr->east = FALSE;
      globalPtr->north = TRUE;    
      globalPtr->west = FALSE;
      globalPtr->south = FALSE;
    }
    else if (direction == 2){
      globalPtr->east = FALSE;
      globalPtr->north = FALSE;    
      globalPtr->west = TRUE;
      globalPtr->south = FALSE;
    } else {
      globalPtr->east = FALSE;
      globalPtr->north = FALSE;    
      globalPtr->west = FALSE;
      globalPtr->south = TRUE;
    }
             
    // step 3: generate train size
    globalPtr->trainSize = randomInteger(2,9);         
    numCars[] = {(char) (48+ globalPtr->trainSize),'\0'};
   // step 4: print things
  static char waffleThingy[] = "Train Size: \0";
  RIT128x96x4StringDraw(waffleThingy, 10, 50, 15);
  RIT128x96x4StringDraw(numCars, 80, 50, 15);
  
  //step 5: set flag to pop this mother from the stack
  globalPtr->trainComComplete = TRUE;
  
  
  //step 6: do passengerCount things
  for (int tibo = 0; tibo < 4; tibo++){
    passCountArray[tibo] = ' ';
    if (tibo == 3){
       passCountArray[tibo] = '\0';
    }
  }

  
  int z = 3;
  
  globalPtr->passengerCount = (double) 300.0*((frequencyCount-1000.0)/1000.0);
  int tempPassengerCount = (int) globalPtr->passengerCount;
  
  while(tempPassengerCount > 0) {
    passCountArray[z] = (tempPassengerCount%10) + 48;
    tempPassengerCount = tempPassengerCount/10;
    z--;   
  }
  
    RIT128x96x4StringDraw("Passengers: \0", 10, 60, 15);
    RIT128x96x4StringDraw(passCountArray, 80, 60, 15);
  return;
  }
}
             //NOTE: currentTrain has its own best nest
void NuSwitchControl(void* localData, void* sharedData){
#if TASK_SELECT == 1 || TASK_SELECT == -1
  pin(HIGH);
#endif
  
  globalData* globalPtr = (globalData*)sharedData;
  switchControlData* localPtr = (switchControlData*)localData;
  static int firstCycle = 0;
  static int rand = 0;
  static char brando9k[] = "GRIDLOCK! \0";
  static int brightness = 15;
  
  //CASE 1
  //train just generated, gridlock is false
  if(!globalPtr->gridlock && globalPtr->trainPresent && firstCycle == 0) {
    
    //The next line clears the idle track message already on the screen
    //RIT128x96x4StringDraw("Turn this off! Aaaaa! \0", 10, 30, 0); 
    
    //startTime is logged as whatever current globalCount value is
    firstCycle = 1;
    localPtr->startTime = globalPtr->globalCount;
    //generate a random number between -2 and 2
    rand = randomInteger(-2, 2);
    
    //if random number is less than 0, we have GRIDLOCK!
    //note: TRAIN MUST RUN AFTER GRIDLOCK FINISHES
    if(rand < 0) {
      globalPtr->gridlock = TRUE;
      globalPtr->gridlockTime = globalPtr->globalCount + (-70*rand);
      globalPtr->traversalTime = 35*(globalPtr->trainSize) + globalPtr->globalCount + globalPtr->gridlockTime;
    }
    //if random number is greater than 0, we have a train passing through!
    else {
      
      globalPtr->traversalTime = 35*(globalPtr->trainSize) + globalPtr->globalCount;
    }
  }
  //CASE 2
  //gridlock is true
  if(globalPtr->gridlock) {
    
    if ((globalPtr->globalCount - globalPtr->gridlockTime) % 6 == 0)
      brightness = 15; 
    
    if ((globalPtr->globalCount - globalPtr->gridlockTime) % 6 == 3)
      brightness = 0;
    
    RIT128x96x4StringDraw(brando9k, 10, 40, brightness); 
    //if globalCount exceeds gridlock timer, end gridlock
    if(globalPtr->globalCount >= globalPtr->gridlockTime)
      globalPtr->gridlock = FALSE;
  }
  
  //CASE 3
  //train is present
  if(globalPtr->trainPresent) {
    if(globalPtr->globalCount >= globalPtr->traversalTime) {
      globalPtr->trainSize = 0;
      globalPtr->trainPresent = FALSE;
      globalPtr->north = FALSE;
      globalPtr->east = FALSE;
      globalPtr->west = FALSE;
      globalPtr->south = FALSE;
      globalPtr->switchConComplete = TRUE;
      PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE); //turn off sound
      RIT128x96x4StringDraw("I don't care \0", 10, 40, 0); //turn off bullshit
     // RIT128x96x4StringDraw("Tracks Idle", 10, 30, 15);
      firstCycle = 0;
    }
  }
  
#if TASK_SELECT == 1 || TASK_SELECT == -1
  pin(LOW);
#endif
}
             
void SerialComTask(void* localData, void* sharedData){
  globalData* globalPtr = (globalData*)sharedData;    
    
  if(globalPtr->north) {
    UARTSend((unsigned char *)"Heading: North", 14);
  }
  if(globalPtr->east) {
    UARTSend((unsigned char *)"Heading: East", 13);
  }
  if(globalPtr->south) {
    UARTSend((unsigned char *)"Heading: South", 14);
  }
  if(globalPtr->west) {
    UARTSend((unsigned char *)"Heading: West", 13);
  }
  if(globalPtr->fromDirection == 'N') {
    UARTSend((unsigned char *)"From Direction: North", 21);
  }
  if(globalPtr->fromDirection == 'E') {
    UARTSend((unsigned char *)"From Direction: East", 20);
  }
  if(globalPtr->fromDirection == 'S') {
    UARTSend((unsigned char *)"From Direction: South", 21);
  }
  if(globalPtr->fromDirection == 'W') {
    UARTSend((unsigned char *)"From Direction: West", 20);
  }  
  if(globalPtr->trainSize) {
    UARTSend((unsigned char *)"Train Size: ", 10);
    UARTSend((unsigned char *)numCars, 1);
  }
  if(globalPtr->gridlock) {
    UARTSend((unsigned char *)"GRIDLOCK!", 9);
  }
  if(globalPtr->passengerCount) {
    UARTSend((unsigned char *)"Passengers: ", 12);
    UARTSend((unsigned char *)passcountArray, 4);
  }  
  if(globalPtr->globalCount) {
    UARTSend((unsigned char *)"AHT Time: ", 10);
    UARTSend((unsigned char *)globalCountArray, 10);
  }  
  return;
}
