#include "headerThingy.h"

void SwitchControl(void *vParameters)
{
  //FLAAAAG
  SwitchConActive = TRUE;
  
  xOLEDMessage gridlockDisplay;
  gridlockDisplay.pcMessage = "GRIDLOCK!";
  gridlockDisplay.ulX = 10;
  gridlockDisplay.ulY = 40;
  gridlockDisplay.brightness = 15;
  
  xOLEDMessage clearScreen;
  clearScreen.pcMessage = "                ";
  clearScreen.ulX = 10;
  clearScreen.ulY = 40;
  clearScreen.brightness = 15;
  
  static int randd = 0;
  
  while(1)
  {
    if (trainPresent==TRUE) {
      
      
      randd = 0;
      //CASE 1
      //train just generated, gridlock is false
      if(!gridlock && trainPresent && firstCycle) {
        //startTime is logged as whatever current globalCount value is
        firstCycle = FALSE;
        //generate a localPtr->random number between -2 and 2
        randd = randomInteger(-2, 2);
        
        //if random number is less than 0, we have GRIDLOCK!
        //note: TRAIN MUST RUN AFTER GRIDLOCK FINISHES
        if(randd < 0) {
          gridlock = TRUE;
          gridlockTime = globalCount + (-12000*randd);
          traversalTime = 6000*(trainSize) +  gridlockTime;
        }
        //if random number is greater than 0, we have a train passing through!
        else {      
          traversalTime = 6000*(trainSize) + globalCount;
        }
      }
      //CASE 2
      //gridlock is true
      if(gridlock) {
        
        if ((gridlockTime - globalCount) % 1000 == 0)
          gridlockDisplay.brightness = 15; 
        
        if ((gridlockTime - globalCount) % 1000 == 500)
          gridlockDisplay.brightness = 0;
        
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
          
          fromDirection = 'X';
          east = FALSE;
          north = FALSE;    
          west = FALSE;
          south = FALSE;
          firstCycle = TRUE;
          xQueueSend( xOLEDQueue, &clearScreen, 0 ); 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE); //turn off sound
        
          //clear trainSize and passengerCount display things
          
          clearScreen.ulX = 10;
          clearScreen.ulY = 50;
          xQueueSend( xOLEDQueue, &clearScreen, 0 );
          clearScreen.ulX = 10;
          clearScreen.ulY = 60;
          xQueueSend( xOLEDQueue, &clearScreen, 0 );
          switchConComplete = TRUE;
          
   
         
        }
      }
      
      
    }
    //DE FLAG
    SwitchConActive = FALSE;
    vTaskDelay(500);
  }
  
  
}
