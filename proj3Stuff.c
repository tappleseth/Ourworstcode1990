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
  //trainComData* localPtr = (trainComData*)localData;
  int direction = 0;
  static char fromNorth[12] = "From: North";
  static char idleState[22] = "Tracks Idle         ";
  static char fromSouth[12] = "From: South";
  static char fromWest[12] = "From: West ";
  static char fromEast[12] = "From: East ";
  if (globalPtr->fromDirection == 'X'){
   
    RIT128x96x4StringDraw(idleState, 10, 30, 15);
  }
  
   //IF !trainPresent and some button was pressed, THEN generate train
  if ((TrainState == 1 || TrainState == 2 || TrainState == 4 || TrainState == 8)&&(globalPtr->trainPresent==FALSE)) {
   globalPtr->trainSize = 0;
    //godDamnit=0;
    
    //step 0: set trainPresent to true
    globalPtr->trainPresent = TRUE;
    
    //step 1: set from direction
    if (TrainState == 1){
      globalPtr->fromDirection = 'N';
      
      RIT128x96x4StringDraw(fromNorth, 10, 30, 15);
   
    } else if (TrainState == 2){
      globalPtr->fromDirection = 'S';
      
      RIT128x96x4StringDraw(fromSouth, 10, 30, 15);
    }  else if (TrainState == 4){
      globalPtr->fromDirection = 'W';
      
      RIT128x96x4StringDraw(fromWest, 10, 30, 15);
    } else {
      globalPtr->fromDirection = 'E';
     
      RIT128x96x4StringDraw(fromEast, 10, 30, 15);
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
    char numCars[2] = 48+ globalPtr->trainSize;
  //static char testPhail[] = " ";
   // step 4: print things
  static char waffleThingy[12] = "Train Size:";
  RIT128x96x4StringDraw(waffleThingy, 10, 50, 15);
  RIT128x96x4StringDraw(numCars, 80, 50, 15);
  
  //step 5: set flag to pop this mother from the stack
  globalPtr->trainComComplete = TRUE;
  
  
  //step 6: do passengerCount things
  char passCountArray[4] = "   ";
  static char passTitle[12] = "Passengers:";
  int z = 2;
  
  globalPtr->passengerCount = (double) 300.0*((frequencyCount-1000.0)/1000.0);
  int tempPassengerCount = (int) globalPtr->passengerCount;
  
  while(tempPassengerCount > 0) {
    passCountArray[z] = (tempPassengerCount%10) + 48;
    tempPassengerCount = tempPassengerCount/10;
    z--;   
  }
  passCountArray[3] = '\0';
  
  RIT128x96x4StringDraw(passTitle, 10, 60, 15);
  RIT128x96x4StringDraw(passCountArray, 80, 60, 15);
  
  } 
  return;
}
             //NOTE: currentTrain has its own best nest
void NuSwitchControl(void* localData, void* sharedData){
#if TASK_SELECT == 1 || TASK_SELECT == -1
  pin(HIGH);
#endif
  
  globalData* globalPtr = (globalData*)sharedData;
 // switchControlData* localPtr = (switchControlData*)localData;
  static int firstCycle = 0;
  static int rand = 0;
  static char brando9k[10] = "GRIDLOCK!";
  static int brightness = 15;
  
  //CASE 1
  //train just generated, gridlock is false
  if(!globalPtr->gridlock && globalPtr->trainPresent && firstCycle == 0) {
    
    
    //startTime is logged as whatever current globalCount value is
    firstCycle = 1;
    //localPtr->startTime = globalPtr->globalCount;
    //generate a random number between -2 and 2
    rand = randomInteger(-2, 2);
    
    //if random number is less than 0, we have GRIDLOCK!
    //note: TRAIN MUST RUN AFTER GRIDLOCK FINISHES
    if(rand < 0) {
      globalPtr->gridlock = TRUE;
      globalPtr->gridlockTime = globalPtr->globalCount + (-12*rand);
      globalPtr->traversalTime = 6*(globalPtr->trainSize) +  globalPtr->gridlockTime;
    }
    //if random number is greater than 0, we have a train passing through!
    else {
      
      globalPtr->traversalTime = 6*(globalPtr->trainSize) + globalPtr->globalCount;
    }
  }
  //CASE 2
  //gridlock is true
  if(globalPtr->gridlock) {
    
    if ((globalPtr->globalCount - globalPtr->gridlockTime) % 2 == 0)
      brightness = 15; 
    
    if ((globalPtr->globalCount - globalPtr->gridlockTime) % 2 == 1)
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
      //static char bullshit[] = "I do not care";
      globalPtr->fromDirection = 'X';
      //RIT128x96x4StringDraw(bullshit, 10, 40, 0); //turn off bullshit
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
  //localData* localPtr = (localData*)localData;
  
  //RIT128x96x4StringDraw("From: Russia ", 10, 60, 15);
 // UARTSend("From: Russia\r\n",14);
  for(int i=0; i<1600;i++);
  //UARTSend("With love\r\n",11);
  
  if(globalPtr->fromDirection == 'N') {
    UARTSend((unsigned char*)"From: North\r\n", 15);
    for(int i=0; i<1600;i++);
  }
  if(globalPtr->fromDirection == 'E') {
    UARTSend((unsigned char*)"From: East\r\n", 14);
    for(int i=0; i<1600;i++);
  }
  if(globalPtr->fromDirection == 'S') {
    UARTSend((unsigned char*)"From: South\r\n", 15);
    for(int i=0; i<1600;i++);
  }
  if(globalPtr->fromDirection == 'W') {
    UARTSend((unsigned char*)"From: West\r\n", 14);
    for(int i=0; i<1600;i++);
  }  
  if(globalPtr->fromDirection == 'X') {
    UARTSend((unsigned char*)"Tracks Idle\r\n", 13);
    for(int i=0; i<1600;i++);
  }  
  
  if (!globalPtr->gridlock){
    if(globalPtr->north) {
      UARTSend((unsigned char*)"To: North\r\n", 11);
      for(int i=0; i<1600;i++);
    }
    if(globalPtr->east) {
      UARTSend((unsigned char*)"To: East\r\n", 10);
      for(int i=0; i<1600;i++);
    }
    if(globalPtr->south) {
      UARTSend((unsigned char*)"To: South\r\n", 11);
      for(int i=0; i<1600;i++);
    }
    if(globalPtr->west) {
      UARTSend((unsigned char*)"To: West\r\n", 10);
      for(int i=0; i<1600;i++);
    }
  } else {
    UARTSend((unsigned char*)"GRIDLOCK!!\r\n", 12);
    for(int i=0; i<1600;i++);
  }
  
  if(globalPtr->trainSize) {
    unsigned char godDamnit;
    godDamnit = 48 + globalPtr->trainSize;
    for(int i=0; i<1600;i++);
    UARTSend("Train Size: ", 12);
    for(int i=0; i<1600;i++);
    UARTSend(&godDamnit, 1);
    for(int i=0; i<1600;i++);
    UARTSend("\r\n",2);
    for(int i=0; i<1600;i++);
  }
 if(globalPtr->passengerCount) {
   
   unsigned char passCountArray[] = "    ";
   
    
    int tempo = (int) globalPtr->passengerCount;
    
    int i = 3; 
  
  while(tempo > 0) {
    passCountArray[i] = (tempo%10) + 48;
    tempo = tempo/10;
    i--;     
  }
    UARTSend("Passengers: ", 12);
    for(int i=0; i<1600;i++);
    UARTSend(passCountArray, 4);
    for(int i=0; i<1600;i++);
    UARTSend("\r\n",2);
    for(int i=0; i<1600;i++);
  }  
  if(globalPtr->globalCount) {
    
    unsigned char globalCountArray[] = "          ";
    
    
    int justinCrazy = globalPtr->globalCount;
    
    int i = 9; 
  
  while(justinCrazy > 0) {
    globalCountArray[i] = (justinCrazy%10) + 48;
    justinCrazy = justinCrazy/10;
    i--;     
  }
    UARTSend("AHT Time: ", 10);
    for(int i=0; i<1600;i++);
    UARTSend(globalCountArray, 10);
    for(int i=0; i<1600;i++);
    UARTSend("\r\n",2);
    for(int i=0; i<1600;i++);
  }  
  UARTSend((unsigned char*)"\r\n", 2); //SPAAAAAAAAACE MAAAAAAAAAAAAAAAN AAAAASA!!! AAAAAAAAA!!! AAAAAOMGF!
  for(int i=0; i<1600;i++);
  return;
}
