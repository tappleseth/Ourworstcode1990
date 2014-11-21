#include "headerThingy.h"
void TrainCom(void *vParameters)
{
  //FLAAAG!! FLAAAAG
  TrainComActive = TRUE;
  
  xOLEDMessage fromNorth;
  fromNorth.pcMessage =  "From: North";
  fromNorth.ulX = 10;
  fromNorth.ulY = 30;
  fromNorth.brightness = 15;
  
  xOLEDMessage fromSouth;
  fromSouth.pcMessage =  "From: South";
  fromSouth.ulX = 10;
  fromSouth.ulY = 30;
  fromSouth.brightness = 15;
  
  xOLEDMessage fromWest;
  fromWest.pcMessage =  "From: West ";
  fromWest.ulX = 10;
  fromWest.ulY = 30;
  fromWest.brightness = 15;
  
  xOLEDMessage fromEast;
  fromEast.pcMessage =  "From: East ";
  fromEast.ulX = 10;
  fromEast.ulY = 30;
  fromEast.brightness = 15;
  
  xOLEDMessage tracksIdle;  
  tracksIdle.pcMessage = "Tracks Idle         ";
  tracksIdle.ulX = 10;
  tracksIdle.ulY = 30;
  tracksIdle.brightness = 15;
  
  while(1)
  {
    
    //static int i = 0;
    
   /* if(globalCount == 5000) {
      TrainState = 1;   
    }*/
    
    // i++;
    
    if (fromDirection == 'X'){
      xQueueSend( xOLEDQueue, &tracksIdle, 0 );
    }
    
    //IF !trainPresent and some button was pressed, THEN generate train
    if ((TrainState == 1 || TrainState == 2 || TrainState == 4 || TrainState == 8)&&(!trainPresent)) {
      //step 0: set trainPresent to true
      trainPresent = TRUE;
      
      //step 1: set from direction
      if (TrainState == 1){
        fromDirection = 'N';
        xQueueSend( xOLEDQueue, &fromNorth, 0 );
        
      } else if (TrainState == 2){
        fromDirection = 'S';
        xQueueSend( xOLEDQueue, &fromSouth, 0 );
      }  else if (TrainState == 4){
        fromDirection = 'W';
        xQueueSend( xOLEDQueue, &fromWest, 0 );
      } else {
        fromDirection = 'E';
        xQueueSend( xOLEDQueue, &fromEast, 0 );
      }
      
      east = FALSE;
      north = FALSE;    
      west = FALSE;
      south = FALSE;
      
      // step 2: generate TO train direction
      direction = randomInteger(0, 3);
      
      if(direction == 0)
        east = TRUE;
      else if(direction == 1)
        north = TRUE;  
      else if(direction == 2)
        west = TRUE;
      else
        south = TRUE;
      
      // step 3: generate train size
      trainSize = randomInteger(2,9);    
      signed char numCarss[] = {48+ trainSize,'\0'};
      xOLEDMessage numCars;
      numCars.pcMessage = &numCarss;
      numCars.ulX = 80;
      numCars.ulY = 50;
      numCars.brightness = 15;
      
      xOLEDMessage TrainSize;
      TrainSize.pcMessage = "Train Size:";
      TrainSize.ulX = 10;
      TrainSize.ulY = 50;
      TrainSize.brightness = 15;
      
      // step 4: print things
      xQueueSend( xOLEDQueue, &TrainSize, 0 );
      xQueueSend( xOLEDQueue, &numCars, 0 );
      
      
      
      
      //step 6: do passengerCount things
      xOLEDMessage PassTitle;
      PassTitle.pcMessage = "Passengers:";
      PassTitle.ulX = 10;
      PassTitle.ulY = 60;
      PassTitle.brightness = 15;
      
      
      
      char passCountArray[4] = "   ";
      int z = 2;
      
      passengerCount = (double) 300.0*((frequencyCount-1000.0)/1000.0);
      int tempPassengerCount = (int) passengerCount;
      
      while(tempPassengerCount > 0) {
        passCountArray[z] = (tempPassengerCount%10) + 48;
        tempPassengerCount = tempPassengerCount/10;
        z--;   
      }
      //passCountArray[3] = '\0';
      
      xOLEDMessage Passengers;
      Passengers.pcMessage = passCountArray;
      Passengers.ulX = 80;
      Passengers.ulY = 60;
      Passengers.brightness = 15;
      
      xQueueSend( xOLEDQueue, &PassTitle, 0 );
      xQueueSend( xOLEDQueue, &Passengers, 0 );
      
      /*CHANGE THIS MOTHER FUCKKKKKKKERSSSSSSS*/
      TrainState = 0;
      
      
      
      
      //step 5: set flag to pop this mother from the stack
      trainComComplete = TRUE;
    } //ebd of if statement block semantics 
    
    TrainComActive = FALSE;
    vTaskDelay(1000);
  } //end of mosnter while(1) loop
  
}
