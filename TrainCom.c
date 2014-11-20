#include "headerThingy.h"

void TrainCom(void){
  #if TASK_SELECT == 1 || TASK_SELECT == -1
    pin(HIGH);
  #endif
    
  static char idleState[22] = "Tracks Idle         ";
  
  static char fromNorth[12] = "From: North";
  static char fromSouth[12] = "From: South";
  static char fromWest[12] = "From: West ";
  static char fromEast[12] = "From: East ";
  
  if (fromDirection == 'X'){
    RIT128x96x4StringDraw(idleState, 10, 30, 15);
  }
  
  //IF !trainPresent and some button was pressed, THEN generate train
  if ((TrainState == 1 || TrainState == 2 || TrainState == 4 || TrainState == 8)&&(!trainPresent)) {
    //step 0: set trainPresent to true
    trainPresent = TRUE;
    
    //step 1: set from direction
    if (TrainState == 1){
      fromDirection = 'N';
      
      RIT128x96x4StringDraw(fromNorth, 10, 30, 15);
      
    } else if (TrainState == 2){
      fromDirection = 'S';
      
      RIT128x96x4StringDraw(fromSouth, 10, 30, 15);
    }  else if (TrainState == 4){
      fromDirection = 'W';
      
      RIT128x96x4StringDraw(fromWest, 10, 30, 15);
    } else {
      fromDirection = 'E';
      
      RIT128x96x4StringDraw(fromEast, 10, 30, 15);
    }
    
    east = FALSE;
    north = FALSE;    
    west = FALSE;
    south = FALSE;
    
    // step 2: generate TO train direction
    //direction = randomInteger(0, 3);
    
    if(direction == 0)
      east = TRUE;
    else if(direction == 1)
      north = TRUE;  
    else if(direction == 2)
      west = TRUE;
    else
      south = TRUE;
    
    // step 3: generate train size
    //trainSize = randomInteger(2,9);    
    char numCars[2] = {48+ trainSize,'\0'};
    //static char testPhail[] = " ";
    // step 4: print things
    static char waffleThingy[12] = "Train Size:";
    RIT128x96x4StringDraw(waffleThingy, 10, 50, 15);
    RIT128x96x4StringDraw(numCars, 80, 50, 15);
    
    //step 5: set flag to pop this mother from the stack
    trainComComplete = TRUE;
    
    
    //step 6: do passengerCount things
    char passCountArray[4] = "   ";
    static char passTitle[12] = "Passengers:";
    int z = 2;
    
    passengerCount = (double) 300.0*((frequencyCount-1000.0)/1000.0);
    int tempPassengerCount = (int) passengerCount;
    
    while(tempPassengerCount > 0) {
      passCountArray[z] = (tempPassengerCount%10) + 48;
      tempPassengerCount = tempPassengerCount/10;
      z--;   
    }
    //passCountArray[3] = '\0';
    
    RIT128x96x4StringDraw(passTitle, 10, 60, 15);
    RIT128x96x4StringDraw(passCountArray, 80, 60, 15);
  }
  
  #if TASK_SELECT == 1 || TASK_SELECT == -1
    pin(LOW);
  #endif
  return;
}
