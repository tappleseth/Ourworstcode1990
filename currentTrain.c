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
void CurrentTrain(void* localData, void* sharedData){
  //produce OLED/audio alarms
  //remove from task queue if no train is present
  globalData* globalPtr = (globalData*)sharedData;    
  currentTrainData* localPtr = (currentTrainData*)localData;
  /*
  
  NORTH TRAIN
  
  */
  
  if(globalPtr->north == TRUE) {
    #if (TASK_SELECT == 2 || TASK_SELECT == -1)
    pin(HIGH);
  #endif
    
  
  
  static unsigned int noiseCount = 0;
  static unsigned int northFlashCount = 0;  
  static char northDisplay[] = "NorthTrain \0";
  static int brightness = 15;
  
  if(globalPtr->north) {
    if(!localPtr->toggleNorth){
      localPtr->toggleNorth = TRUE;
      noiseCount = globalPtr->globalCount + 60;
      northFlashCount = globalPtr->globalCount;
      brightness = 15;
    }
    
    if(localPtr->toggleNorth) {
      //FLASH EVENTS
      //loops indefinitely (will end when train passes)
      if((globalPtr->globalCount - northFlashCount) % 18 == 0)
        brightness = 15; 
      
      if((globalPtr->globalCount - northFlashCount) % 18 == 9)
        brightness = 0;
      
      RIT128x96x4StringDraw(northDisplay, 10, 40, brightness); 
      //END FLASH EVENTS
      
      //SOUND EVENTS
      #if SOUND_ENABLE == 1
        //first blast: lasts 2 seconds
        if(globalPtr->globalCount == (noiseCount - 60))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //off time between first and second blast
        if(globalPtr->globalCount == (noiseCount - 48))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        //wait one second, then start second blast
        if(globalPtr->globalCount == (noiseCount - 42))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //wait two seconds, stop second blast
        if(globalPtr->globalCount == (noiseCount - 30))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        //wait one second, then start first short blast
        if(globalPtr->globalCount == (noiseCount - 24))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //wait one second, then stop first short blast
        if(globalPtr->globalCount == (noiseCount - 18))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        //wait one second, then start second short blast
        if(globalPtr->globalCount == (noiseCount - 12))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //wait one second, end all blasts, set bigglesOn to FALSE
        if(globalPtr->globalCount == (noiseCount - 6))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        if(noiseCount == globalPtr->globalCount) {
          noiseCount = globalPtr->globalCount + 60;
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        }      
      #endif
    }
  }
  
  //placeholder: need code to STOP SOME SHIT
  if(globalPtr->globalCount >= globalPtr->traversalTime) {
    PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
    noiseCount = 0;
    northFlashCount = 0;
    brightness = 15;
    localPtr->toggleNorth = FALSE;
    globalPtr->currentTrainComplete = TRUE;
    RIT128x96x4StringDraw(northDisplay, 10, 40, 0);
  }
  
  #if TASK_SELECT == 2 || TASK_SELECT == -1
    pin(LOW); 
  #endif    
  }
  
  /*
  
  EAST TRAIN
  
  */
  
  if(globalPtr->east == TRUE) {
    #if TASK_SELECT == 4 || TASK_SELECT == -1
    pin(HIGH);
  #endif
   
  
  static unsigned int eastNoiseCount = 0;
  static unsigned int eastFlashCount = 0;
  static char eastDisplay[] = "EastTrain \0";
  static int brightness = 15;
  
  if(globalPtr->east) {
    if(!localPtr->toggleEast) {
      localPtr->toggleEast = TRUE;
      eastNoiseCount = globalPtr->globalCount + 78;
      eastFlashCount = globalPtr->globalCount;
      brightness = 15;
    }
    
    //*****FLASH EVENTS*****  
    
    if(localPtr->toggleEast) {
      //FLASH OFF
      
      if ((globalPtr->globalCount - eastFlashCount) % 12 == 0)
        brightness = 15; 
      
      if ((globalPtr->globalCount - eastFlashCount) % 12 == 6)
        brightness = 0;
      
      RIT128x96x4StringDraw(eastDisplay, 10, 40, brightness); 
      
      //*****SOUND EVENTS*****
      #if SOUND_ENABLE == 1
        //first blast: lasts 2 seconds
        if(globalPtr->globalCount == (eastNoiseCount - 78)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //off time between first and second blast
        if(globalPtr->globalCount == (eastNoiseCount - 66)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        //wait one second, then start second blast
        if(globalPtr->globalCount == (eastNoiseCount - 60)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //wait two seconds, stop second blast
        if(globalPtr->globalCount == (eastNoiseCount - 48)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        //wait one second, then start first short blast
        if(globalPtr->globalCount == (eastNoiseCount - 42)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //wait one second, then stop first short blast
        if(globalPtr->globalCount == (eastNoiseCount - 30)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        //first blast: lasts 2 seconds
        if(globalPtr->globalCount == (eastNoiseCount - 24)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //off time between first and second blast
        if(globalPtr->globalCount == (eastNoiseCount - 18)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        //wait one second, then start second blast
        if(globalPtr->globalCount == (eastNoiseCount - 12))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //wait two seconds, stop second blast
        if(globalPtr->globalCount == (eastNoiseCount - 6))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        if(eastNoiseCount == globalPtr->globalCount) {
          eastNoiseCount = globalPtr->globalCount + 78;
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        }
      #endif
    }   
  }
  
  if(globalPtr->globalCount >= globalPtr->traversalTime) {
    PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
    globalPtr->currentTrainComplete = TRUE;
    eastNoiseCount = 0;
    eastFlashCount = 0;
    brightness = 15;
    localPtr->toggleEast = FALSE;
    RIT128x96x4StringDraw(eastDisplay, 10, 40, 0);
  }

  #if TASK_SELECT == 4 || TASK_SELECT == -1
    pin(LOW);
  #endif    
  }
  
  /*
  
  WEST TRAIN
  
  */
  
  if(globalPtr->west == TRUE) {
    #if TASK_SELECT == 3 || TASK_SELECT == -1
    pin(HIGH);
  #endif

  
  static unsigned int westNoiseCount = 0;
  static unsigned int westFlashCount = 0;
  static char westDisplay[] = "WestTrain \0";
  static int brightness = 15;  
  
  if(globalPtr->west) {
    if(!localPtr->toggleWest) {
      localPtr->toggleWest = TRUE;
      westNoiseCount = globalPtr->globalCount + 42;
      westFlashCount = globalPtr->globalCount;
      brightness = 15;
    }
    
    //FLASH EVENTS
    if(localPtr->toggleWest) {
      if ((globalPtr->globalCount - westFlashCount) % 24 == 0)
        brightness = 15; 
      
      if ((globalPtr->globalCount - westFlashCount) % 24 == 12)
        brightness = 0;
      
      RIT128x96x4StringDraw(westDisplay, 10, 40, brightness); 
      
      //SOUND EVENTS
      #if SOUND_ENABLE == 1      
        //first blast: lasts 2 seconds
        if(globalPtr->globalCount == (westNoiseCount - 42))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //off time between first and second blast
        if(globalPtr->globalCount == (westNoiseCount - 30))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        //wait one second, then start second blast
        if(globalPtr->globalCount == (westNoiseCount - 24))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //wait two seconds, stop second blast
        if(globalPtr->globalCount == (westNoiseCount - 18))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        //wait one second, then start first short blast
        if(globalPtr->globalCount == (westNoiseCount - 12))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //wait one second, then stop first short blast
        if(globalPtr->globalCount == (westNoiseCount - 6))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);    
        
        if(westNoiseCount == globalPtr->globalCount) {
          westNoiseCount = globalPtr->globalCount + 42;
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        }
      #endif
    }
  }
  
  if(globalPtr->globalCount >= globalPtr->traversalTime) {
    PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
    globalPtr->currentTrainComplete = TRUE;
    westNoiseCount = 0;
    westFlashCount = 0;
    brightness = 15;
    localPtr->toggleWest = FALSE;
    RIT128x96x4StringDraw(westDisplay, 10, 40, 0);
  }
  
  #if TASK_SELECT == 3 || TASK_SELECT == -1
    pin(LOW);
  #endif    
  }
  
  /*
  
  SOUTH TRAIN
  
  */
  
  if(globalPtr->south == TRUE) {
    #if TASK_SELECT == 5 || TASK_SELECT == -1
    pin(HIGH);
  #endif
     
  static unsigned int southNoiseCount = 0;
  static unsigned int southFlashCount = 0;
  static char southDisplay[] = "SouthTrain \0";
  static int brightness = 15;
  
  if(globalPtr->south) {
    if(!localPtr->toggleSouth) {
      localPtr->toggleSouth = TRUE;
      southNoiseCount = globalPtr->globalCount + 72;
      southFlashCount = globalPtr->globalCount;
      brightness = 15;
    }
    
    //*****FLASH EVENTS*****  
    
    if(localPtr->toggleSouth) {
      //FLASH OFF
      
      if ((globalPtr->globalCount - southFlashCount) % 12 == 0)
        brightness = 15;
      
      if ((globalPtr->globalCount - southFlashCount) % 12 == 6)
        brightness = 0;
      
      RIT128x96x4StringDraw(southDisplay, 10, 40, brightness); 
      
      //*****SOUND EVENTS*****
      #if SOUND_ENABLE == 1
        //first blast: lasts 2 seconds
        if(globalPtr->globalCount == (southNoiseCount - 72)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //off time between first and second blast
        if(globalPtr->globalCount == (southNoiseCount - 60)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        //wait one second, then start second blast
        if(globalPtr->globalCount == (southNoiseCount - 54)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //wait two seconds, stop second blast
        if(globalPtr->globalCount == (southNoiseCount - 42)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        //wait one second, then start first short blast
        if(globalPtr->globalCount == (southNoiseCount - 36)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //wait one second, then stop first short blast
        if(globalPtr->globalCount == (southNoiseCount - 30)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        //first blast: lasts 2 seconds
        if(globalPtr->globalCount == (southNoiseCount - 24)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //off time between first and second blast
        if(globalPtr->globalCount == (southNoiseCount - 18)) 
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        //wait one second, then start second blast
        if(globalPtr->globalCount == (southNoiseCount - 12))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        
        //wait two seconds, stop second blast
        if(globalPtr->globalCount == (southNoiseCount - 6))
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        if(southNoiseCount == globalPtr->globalCount) {
          southNoiseCount = globalPtr->globalCount + 72;
          PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
        }
      #endif
    }   
  }
  
  if(globalPtr->globalCount >= globalPtr->traversalTime) {
    PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
    globalPtr->currentTrainComplete = TRUE;
    southNoiseCount = 0;
    southFlashCount = 0;
    brightness = 15;
    localPtr->toggleSouth = FALSE;
    RIT128x96x4StringDraw(southDisplay, 10, 40, 0);
  }

  #if TASK_SELECT == 4 || TASK_SELECT == -1
    pin(LOW);
  #endif    
  }
  return;
}
