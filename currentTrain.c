//justin's current train madness
//produce OLED/audio alarms
//remove from task queue if no train is present

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

/* 
recently modified to account for edge case of simultaneous gridlock and train traversal 
this fixes the flickering text bug, 
which resulted from "GRIDLOCK" and "<TO direction>" overlapping
oho!
-Thomas
*/
void CurrentTrain(void* localData, void* sharedData){
  
#if TASK_SELECT == 4 || TASK_SELECT == -1
  pin(HIGH);
#endif
  
  //PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
  //produce OLED/audio alarms
  //remove from task queue if no train is present
  globalData* globalPtr = (globalData*)sharedData;    
  currentTrainData* localPtr = (currentTrainData*)localData;
  /*
  
  NORTH TRAIN
  
  */
  
  if(globalPtr->north == TRUE && !globalPtr->gridlock) {
    
    static unsigned int noiseCount = 0;
    static unsigned int northFlashCount = 0;  
    static char northDisplay[15] = "Heading: North";
    static int brightness = 15;
    
    if(globalPtr->north) {
      if(!localPtr->toggleNorth){
        localPtr->toggleNorth = TRUE;
        noiseCount = globalPtr->globalCount + 10;
        northFlashCount = globalPtr->globalCount;
        brightness = 15;
      }
      
      if(localPtr->toggleNorth) {
        //FLASH EVENTS
        //loops indefinitely (will end when train passes)
        if((globalPtr->globalCount - northFlashCount) % 3 == 0)
          brightness = 15; 
        
        if((globalPtr->globalCount - northFlashCount) % 3 == 2)
          brightness = 0;
        
        RIT128x96x4StringDraw(northDisplay, 10, 40, brightness); 
        //END FLASH EVENTS
        
        //SOUND EVENTS
#if SOUND_ENABLE == 1
        //first blast: lasts 2 seconds
        if(globalPtr->globalCount == (noiseCount - 10))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //off time between first and second blast
        if(globalPtr->globalCount == (noiseCount - 8))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        //wait one second, then start second blast
        if(globalPtr->globalCount == (noiseCount - 7))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //wait two seconds, stop second blast
        if(globalPtr->globalCount == (noiseCount - 5))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        //wait one second, then start first short blast
        if(globalPtr->globalCount == (noiseCount - 4))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //wait one second, then stop first short blast
        if(globalPtr->globalCount == (noiseCount - 3))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        //wait one second, then start second short blast
        if(globalPtr->globalCount == (noiseCount - 2))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //wait one second, end all blasts, set bigglesOn to FALSE
        if(globalPtr->globalCount == (noiseCount - 1))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        if(noiseCount == globalPtr->globalCount) {
          noiseCount = globalPtr->globalCount + 10;
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        }      
#endif
      }
    }
     
  }
  
  /*
  
  EAST TRAIN
  
  */
  
  if(globalPtr->east == TRUE && !globalPtr->gridlock) {
    
    
    static unsigned int eastNoiseCount = 0;
    static unsigned int eastFlashCount = 0;
    static char eastDisplay[14] = "Heading: East";
    static int brightness = 15;
    
    if(globalPtr->east) {
      if(!localPtr->toggleEast) {
        localPtr->toggleEast = TRUE;
        eastNoiseCount = globalPtr->globalCount + 13;
        eastFlashCount = globalPtr->globalCount;
        brightness = 15;
      }
      
      //*****FLASH EVENTS*****  
      
      if(localPtr->toggleEast) {
        //FLASH OFF
        
        if ((globalPtr->globalCount - eastFlashCount) % 2 == 0)
          brightness = 15; 
        
        if ((globalPtr->globalCount - eastFlashCount) % 2 == 1)
          brightness = 0;
        
        RIT128x96x4StringDraw(eastDisplay, 10, 40, brightness); 
        
        //*****SOUND EVENTS*****
#if SOUND_ENABLE == 1
        //first blast: lasts 2 seconds
        if(globalPtr->globalCount == (eastNoiseCount - 13)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //off time between first and second blast
        if(globalPtr->globalCount == (eastNoiseCount - 11)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        //wait one second, then start second blast
        if(globalPtr->globalCount == (eastNoiseCount - 10)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //wait two seconds, stop second blast
        if(globalPtr->globalCount == (eastNoiseCount - 8)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        //wait one second, then start first short blast
        if(globalPtr->globalCount == (eastNoiseCount - 7)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //wait one second, then stop first short blast
        if(globalPtr->globalCount == (eastNoiseCount - 5)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        //first blast: lasts 2 seconds
        if(globalPtr->globalCount == (eastNoiseCount - 4)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //off time between first and second blast
        if(globalPtr->globalCount == (eastNoiseCount - 3)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        //wait one second, then start second blast
        if(globalPtr->globalCount == (eastNoiseCount - 2))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //wait two seconds, stop second blast
        if(globalPtr->globalCount == (eastNoiseCount - 1))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        if(eastNoiseCount == globalPtr->globalCount) {
          eastNoiseCount = globalPtr->globalCount + 13;
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        }
#endif
      }   
    }
    
   /* if(globalPtr->globalCount >= globalPtr->traversalTime) {
      PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
      globalPtr->currentTrainComplete = TRUE;
      eastNoiseCount = 0;
      eastFlashCount = 0;
      brightness = 15;
      localPtr->toggleEast = FALSE;
      RIT128x96x4StringDraw(eastDisplay, 10, 40, 0);
    }*/
    
  }
  
  /*
  
  WEST TRAIN
  
  */
  
  if(globalPtr->west == TRUE && !globalPtr->gridlock) {
    
    
    static unsigned int westNoiseCount = 0;
    static unsigned int westFlashCount = 0;
    static char westDisplay[14] = "Heading: West";
    static int brightness = 15;  
    
    if(globalPtr->west) {
      if(!localPtr->toggleWest) {
        localPtr->toggleWest = TRUE;
        westNoiseCount = globalPtr->globalCount + 7;
        westFlashCount = globalPtr->globalCount;
        brightness = 15;
      }
      
      //FLASH EVENTS
      if(localPtr->toggleWest) {
        if ((globalPtr->globalCount - westFlashCount) % 4 == 0)
          brightness = 15; 
        
        if ((globalPtr->globalCount - westFlashCount) % 4 == 2)
          brightness = 0;
        
        RIT128x96x4StringDraw(westDisplay, 10, 40, brightness); 
        
        //SOUND EVENTS
#if SOUND_ENABLE == 1      
        //first blast: lasts 2 seconds
        if(globalPtr->globalCount == (westNoiseCount - 7))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //off time between first and second blast
        if(globalPtr->globalCount == (westNoiseCount - 5))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        //wait one second, then start second blast
        if(globalPtr->globalCount == (westNoiseCount - 4))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //wait two seconds, stop second blast
        if(globalPtr->globalCount == (westNoiseCount - 3))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        //wait one second, then start first short blast
        if(globalPtr->globalCount == (westNoiseCount - 2))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //wait one second, then stop first short blast
        if(globalPtr->globalCount == (westNoiseCount - 1))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);    
        
        if(westNoiseCount == globalPtr->globalCount) {
          westNoiseCount = globalPtr->globalCount + 7;
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        }
#endif
      }
    }
    
    /*if(globalPtr->globalCount >= globalPtr->traversalTime) {
      PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
      globalPtr->currentTrainComplete = TRUE;
      westNoiseCount = 0;
      westFlashCount = 0;
      brightness = 15;
      localPtr->toggleWest = FALSE;
      RIT128x96x4StringDraw(westDisplay, 10, 40, 0);
    }*/
      
  }
  
  /*
  
  SOUTH TRAIN
  
  */
  
  if(globalPtr->south == TRUE && !globalPtr->gridlock) {
    
    static unsigned int southNoiseCount = 0;
    static unsigned int southFlashCount = 0;
    static char southDisplay[15] = "Heading: South";
    static int brightness = 15;
    
    if(globalPtr->south) {
      if(!localPtr->toggleSouth) {
        localPtr->toggleSouth = TRUE;
        southNoiseCount = globalPtr->globalCount + 12;
        southFlashCount = globalPtr->globalCount;
        brightness = 15;
      }
      
      //*****FLASH EVENTS*****  
      
      if(localPtr->toggleSouth) {
        //FLASH OFF
        
        if ((globalPtr->globalCount - southFlashCount) % 2 == 0)
          brightness = 15;
        
        if ((globalPtr->globalCount - southFlashCount) % 2 == 1)
          brightness = 0;
        
        RIT128x96x4StringDraw(southDisplay, 10, 40, brightness); 
        
        //*****SOUND EVENTS*****
#if SOUND_ENABLE == 1
        //first blast: lasts 2 seconds
        if(globalPtr->globalCount == (southNoiseCount - 12)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //off time between first and second blast
        if(globalPtr->globalCount == (southNoiseCount - 10)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        //wait one second, then start second blast
        if(globalPtr->globalCount == (southNoiseCount - 9)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //wait two seconds, stop second blast
        if(globalPtr->globalCount == (southNoiseCount - 7)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        //wait one second, then start first short blast
        if(globalPtr->globalCount == (southNoiseCount - 6)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //wait one second, then stop first short blast
        if(globalPtr->globalCount == (southNoiseCount - 5)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        //first blast: lasts 2 seconds
        if(globalPtr->globalCount == (southNoiseCount - 4)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //off time between first and second blast
        if(globalPtr->globalCount == (southNoiseCount - 3)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        //wait one second, then start second blast
        if(globalPtr->globalCount == (southNoiseCount - 2))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //wait two seconds, stop second blast
        if(globalPtr->globalCount == (southNoiseCount - 1))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        if(southNoiseCount == globalPtr->globalCount) {
          southNoiseCount = globalPtr->globalCount + 12;
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        }
#endif
      }   
    }
    
    /*if(globalPtr->globalCount >= globalPtr->traversalTime) {
      PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
      globalPtr->currentTrainComplete = TRUE;
      southNoiseCount = 0;
      southFlashCount = 0;
      brightness = 15;
      localPtr->toggleSouth = FALSE;
      RIT128x96x4StringDraw(southDisplay, 10, 40, 0);
    }*/
      
  }
  //NOTE TO SELF: BUUUULLLSHIIIIIT
  if(globalPtr->globalCount >= globalPtr->traversalTime) {
      PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
      localPtr->toggleSouth = FALSE;
      localPtr->toggleWest = FALSE;
      localPtr->toggleEast = FALSE;
      localPtr->toggleNorth = FALSE;
      globalPtr->currentTrainComplete = TRUE;
      static char clearSafety[16] = "               ";
      RIT128x96x4StringDraw(clearSafety, 10, 40, 0); 
      
  }
#if TASK_SELECT == 4 || TASK_SELECT == -1
  pin(LOW);
#endif
  return;
}
