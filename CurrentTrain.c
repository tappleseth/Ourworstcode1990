#include "headerThingy.h"

/* 
recently modified to account for edge case of simultaneous gridlock and train traversal 
this fixes the flickering text bug, 
which resulted from "GRIDLOCK" and "<TO direction>" overlapping
oho!
-Thomas sux
*/
void CurrentTrain(void *vParameters)
{
  //FLAAAAG!! OMFG FLAAAG
  CurrentTrainActive = TRUE;
  
  xOLEDMessage headingNorth;
  headingNorth.pcMessage = "Heading: North";
  headingNorth.ulX = 10;
  headingNorth.ulY = 40;
  headingNorth.brightness = 15;
  
  xOLEDMessage headingWest;
  headingWest.pcMessage = "Heading: West";
  headingWest.ulX = 10;
  headingWest.ulY = 40;
  headingWest.brightness = 15;
  
  xOLEDMessage headingEast;
  headingEast.pcMessage = "Heading: East";
  headingEast.ulX = 10;
  headingEast.ulY = 40;
  headingEast.brightness = 15;
  
  xOLEDMessage headingSouth;
  headingSouth.pcMessage = "Heading: South";
  headingSouth.ulX = 10;
  headingSouth.ulY = 40;
  headingSouth.brightness = 15;
  
  xOLEDMessage tracksIdle;
  tracksIdle.pcMessage = "Tracks Idle";
  tracksIdle.ulX = 10;
  tracksIdle.ulY = 40;
  tracksIdle.brightness = 15;
  
  xOLEDMessage clearSafety;
  clearSafety.pcMessage = "           ";
  clearSafety.ulX = 10;
  clearSafety.ulY = 40;
  clearSafety.brightness = 0;
  
  while(1)
  {
    if (trainPresent==TRUE){
      /*
      
      NORTH TRAIN
      
      */
      
      if(!gridlock) {
        if(north) {
          
          //static char northDisplay[15] = "Heading: North";
          
          if(toggleNorth){
            toggleNorth = TRUE;
            noiseCount = globalCount + 10000;
            flashCount = globalCount;
            headingNorth.brightness = 15;
          }
          
          if(toggleNorth) {
            //FLASH EVENTS
            //loops indefinitely (will end when train passes)
            if((globalCount - flashCount) % 3000 == 0)
              headingNorth.brightness = 15; 
            
            if((globalCount - flashCount) % 3000 == 2000)
              headingNorth.brightness = 0;
            
            xQueueSend( xOLEDQueue, &headingNorth, 0  );
            //END FLASH EVENTS
            
            //SOUND EVENTS
#if SOUND_ENABLE == 1
            //first blast: lasts 2 seconds
            if(globalCount == (noiseCount - 10000))
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
            
            //off time between first and second blast
            if(globalCount == (noiseCount - 8000))
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
            
            //wait one second, then start second blast
            if(globalCount == (noiseCount - 7000))
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
            
            //wait two seconds, stop second blast
            if(globalCount == (noiseCount - 5000))
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
            
            //wait one second, then start first short blast
            if(globalCount == (noiseCount - 4000))
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
            
            //wait one second, then stop first short blast
            if(globalCount == (noiseCount - 3000))
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
            
            //wait one second, then start second short blast
            if(globalCount == (noiseCount - 2000))
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
            
            //wait one second, end all blasts, set bigglesOn to FALSE
            if(globalCount == (noiseCount - 1000))
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
            
            if(noiseCount == globalCount) {
              noiseCount = globalCount + 10000;
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
            }      
            
#endif
          }
        }
        
        else if(east) {
          // static char eastDisplay[14] = "Heading: East";
          
          if(!toggleEast) {
            toggleEast = TRUE;
            noiseCount = globalCount + 13000;
            flashCount = globalCount;
            headingEast.brightness = 15000;
          }
          
          //*****FLASH EVENTS*****  
          
          if(toggleEast) {
            //FLASH OFF
            
            if ((globalCount - flashCount) % 2000 == 0)
              headingEast.brightness = 15; 
            
            if ((globalCount - flashCount) % 2000 == 1000)
              headingEast.brightness = 0;
            
            xQueueSend( xOLEDQueue, &headingEast, 0);
            
            //*****SOUND EVENTS*****
#if SOUND_ENABLE == 1
            
            //first blast: lasts 2 seconds
            if(globalCount == (noiseCount - 13000)) 
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
            
            //off time between first and second blast
            if(globalCount == (noiseCount - 11000)) 
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
            
            //wait one second, then start second blast
            if(globalCount == (noiseCount - 10000)) 
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
            
            //wait two seconds, stop second blast
            if(globalCount == (noiseCount - 8000)) 
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
            
            //wait one second, then start first short blast
            if(globalCount == (noiseCount - 7000)) 
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
            
            //wait one second, then stop first short blast
            if(globalCount == (noiseCount - 5000)) 
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
            
            //first blast: lasts 2 seconds
            if(globalCount == (noiseCount - 4000)) 
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
            
            //off time between first and second blast
            if(globalCount == (noiseCount - 3000)) 
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
            
            //wait one second, then start second blast
            if(globalCount == (noiseCount - 2000))
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
            
            //wait two seconds, stop second blast
            if(globalCount == (noiseCount - 1000))
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
            
            if(noiseCount == globalCount) {
              noiseCount = globalCount + 13000;
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
            }
            
#endif
          }
        }
        
        else if(west) {
          //static char westDisplay[14] = "Heading: West";
          
          if(!toggleWest) {
            toggleWest = TRUE;
            noiseCount = globalCount + 7000;
            flashCount = globalCount;
            headingWest.brightness = 15;
          }
          
          //FLASH EVENTS
          if(toggleWest) {
            if ((globalCount - flashCount) % 4000 == 0)
              headingWest.brightness = 15;
            
            if ((globalCount - flashCount) % 4000 == 2000)
              headingWest.brightness = 0;
            
            xQueueSend( xOLEDQueue, &headingWest, 0  );
            
            //SOUND EVENTS
#if SOUND_ENABLE == 1      
            
            //first blast: lasts 2 seconds
            if(globalCount == (noiseCount - 7000))
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
            
            //off time between first and second blast
            if(globalCount == (noiseCount - 5000))
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
            
            //wait one second, then start second blast
            if(globalCount == (noiseCount - 4000))
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
            
            //wait two seconds, stop second blast
            if(globalCount == (noiseCount - 3000))
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
            
            //wait one second, then start first short blast
            if(globalCount == (noiseCount - 2000))
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
            
            //wait one second, then stop first short blast
            if(globalCount == (noiseCount - 1000))
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);    
            
            if(noiseCount == globalCount) {
              noiseCount = globalCount + 7000;
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
            }
#endif
          }
        } 
        
        else {
          //static char southDisplay[15] = "Heading: South";
          
          if(!toggleSouth) {
            toggleSouth = TRUE;
            noiseCount = globalCount + 12000;
            flashCount = globalCount;
            headingSouth.brightness = 15;
          }
          
          //*****FLASH EVENTS*****  
          
          if(toggleSouth) {
            //FLASH OFF
            
            if ((globalCount - flashCount) % 2000 == 0)
              headingSouth.brightness = 15;
            
            if ((globalCount - flashCount) % 2000 == 1000)
              headingSouth.brightness = 0;
            
            xQueueSend( xOLEDQueue, &headingSouth, 0  );
            
            //*****SOUND EVENTS*****
#if SOUND_ENABLE == 1
            //first blast: lasts 2 seconds
            if(globalCount == (noiseCount - 12000)) 
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
            
            //off time between first and second blast
            if(globalCount == (noiseCount - 10000)) 
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
            
            //wait one second, then start second blast
            if(globalCount == (noiseCount - 9000)) 
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
            
            //wait two seconds, stop second blast
            if(globalCount == (noiseCount - 7000)) 
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
            
            //wait one second, then start first short blast
            if(globalCount == (noiseCount - 6000)) 
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
            
            //wait one second, then stop first short blast
            if(globalCount == (noiseCount - 5000)) 
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
            
            //first blast: lasts 2 seconds
            if(globalCount == (noiseCount - 4000)) 
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
            
            //off time between first and second blast
            if(globalCount == (noiseCount - 3000)) 
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
            
            //wait one second, then start second blast
            if(globalCount == (noiseCount - 2000))
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
            
            //wait two seconds, stop second blast
            if(globalCount == (noiseCount - 1000))
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
            
            if(noiseCount == globalCount) {
              noiseCount = globalCount + 12000;
              PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, TRUE);
            }
#endif
          }   
        }    
      }
      //NOTE TO SELF: BUUUULLLSHIIIIIT
     /* if(globalCount >= traversalTime) {
        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, FALSE);
        
        //static char clearSafety[16] = "               ";
        //RIT128x96x4StringDraw(clearSafety, 10, 40, 0); 
        xQueueSend( xOLEDQueue, &clearSafety, 0  );
        currentTrainComplete = TRUE;
      }*/
      
      
    }
    //UNFLAG?
    CurrentTrainActive = FALSE;
    vTaskDelay(1000);
  }
  
  
  
  
}

