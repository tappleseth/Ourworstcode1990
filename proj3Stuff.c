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

void addToStack(boringTom* addMe){
  head->next = addMe;
  addMe->previous = head;
  addMe->next = NULL;
  
  head = addMe;
}

void popFromStack(){
  boringTom* lynnsIdea;
  lynnsIdea = head->previous;
  head = lynnsIdea;
  head->next = NULL;
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
    //step 0: set trainPresent to true
    globalPtr->trainPresent = TRUE;
    //OMFG, do stuff
    //step 1: set from direction
    if (TrainState == 1){
      globalPtr->fromDirection = 'N';
    } else if (TrainState == 2){
      globalPtr->fromDirection = 'S';
    }  else if (TrainState == 4){
      globalPtr->fromDirection = 'W';
    } else {
      globalPtr->fromDirection = 'E';
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
    char tomLose = (char) globalPtr->trainSize;
   // step 4: print things
  static char waffleThingy[] = "Train Size: \0";
  RIT128x96x4StringDraw(waffleThingy, 10, 50, 15);
  RIT128x96x4StringDraw(&tomLose, 20, 50, 15);
  
  //step 5: set flag to pop this mother from the stack
  globalPtr->trainComComplete = TRUE;
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
  
  static int rand = 0;
  static char brando9k[] = "GRIDLOCK! \0";
  static int brightness = 15;
  
  if(!globalPtr->gridlock && globalPtr->trainPresent) {
    //startTime is logged as whatever current globalCount value is
    localPtr->startTime = globalPtr->globalCount;
    //generate a random number between -2 and 2
    rand = randomInteger(-2, 2);
    
    //if random number is less than 0, we have GRIDLOCK!
    if(rand < 0) {
      globalPtr->gridlock = TRUE;
      globalPtr->traversalTime = globalPtr->globalCount + (-70*rand);
    }
    //if random number is greater than 0, we have a train passing through!
    else {
      
      globalPtr->traversalTime = 35*(globalPtr->trainSize) + globalPtr->globalCount;
    }
  }
  //CASE 2
  //gridlock is true
  if(globalPtr->gridlock) {
    //if globalCount exceeds gridlock timer, end gridlock
    if ((globalPtr->globalCount - globalPtr->traversalTime) % 6 == 0)
      brightness = 15; 
    
    if ((globalPtr->globalCount - globalPtr->traversalTime) % 6 == 3)
      brightness = 0;
    
    RIT128x96x4StringDraw(brando9k, 10, 40, brightness); 
    
    if(globalPtr->globalCount >= globalPtr->traversalTime)
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
    }
  }
  
#if TASK_SELECT == 1 || TASK_SELECT == -1
  pin(LOW);
#endif
             }
             
             void SerialComTask(void* localData, void* globalData){
               return;
             }
             
          
             
             
             
