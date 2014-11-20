#include "headerThingy.h"

void SwitchControl(void){
#if TASK_SELECT == 2 || TASK_SELECT == -1
  pin(HIGH);
#endif
  
 
  
  randd = 0;
  brightness = 15;
  
  xOLEDMessage gridlockDisplay;
  gridlockDisplay.pcMessage = "GRIDLOCK!";
  gridlockDisplay.ulX = 10;
  gridlockDisplay.ulY = 40;
  gridlockDisplay.brightness = 15;
  
  //CASE 1
  //train just generated, gridlock is false
  if(!gridlock && trainPresent && firstCycle) {
    //startTime is logged as whatever current globalCount value is
    firstCycle = FALSE;
    startTime = globalCount;
    //generate a localPtr->random number between -2 and 2
    randd = randomInteger(-2, 2);
    
    //if random number is less than 0, we have GRIDLOCK!
    //note: TRAIN MUST RUN AFTER GRIDLOCK FINISHES
    if(randd < 0) {
      gridlock = TRUE;
      gridlockTime = globalCount + (-12*randd);
      traversalTime = 6*(trainSize) +  gridlockTime;
    }
    //if random number is greater than 0, we have a train passing through!
    else {      
      traversalTime = 6*(trainSize) + globalCount;
    }
  }
  //CASE 2
  //gridlock is true
  if(gridlock) {
    
    if ((globalCount - gridlockTime) % 2 == 0)
      brightness = 15; 
    
    if ((globalCount - gridlockTime) % 2 == 1)
      brightness = 0;
    
    xQueueSend( xOLEDQueue, &gridlockDisplay, 0 ); 
    //if globalCount exceeds gridlock timer, end gridlock
    if(globalCount >= gridlockTime)
      gridlock = FALSE;
  }
  
  //CASE 3
  //train is present
  if(trainPresent) {
    if(globalCount >= traversalTime) {
      trainPresent = FALSE;
      switchConComplete = TRUE;
      fromDirection = 'X';
      
      firstCycle = TRUE;
      
      PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE); //turn off sound
    }
  }
  
#if TASK_SELECT == 2 || TASK_SELECT == -1
  pin(LOW);
#endif
}
