

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
  if ((TrainState == 1 || TrainState == 2 || TrainState == 4 || TrainState == 8)&&(globalPtr.trainPresent==FALSE)) {
    //OMFG, do stuff
    //step 1: set from direction
    if (TrainState == 1){
      globalPtr.fromDirection = 'N';
    } else if (TrainState == 2){
      globalPtr.fromDirection = 'S';
    }  else if (TrainState == 4){
      globalPtr.fromDirection = 'W';
    } else {
      globalPtr.fromDirection = 'E';
    }
    
    // step 2: generate TO train direction
    direction = randomInteger(0, 3);
    if(direction == 0) {
      globalPtr.east = TRUE;
      globalPtr.north = FALSE;    
      globalPtr.west = FALSE;
      globalPtr.south = FALSE;
    }
    else if(direction == 1) {
      globalPtr.east = FALSE;
      globalPtr.north = TRUE;    
      globalPtr.west = FALSE;
      globalPtr.south = FALSE;
    }
    else if (direction == 2{
      globalPtr.east = FALSE;
      globalPtr.north = FALSE;    
      globalPtr.west = TRUE;
      globalPtr.south = FALSE;
    } else {
      globalPtr.east = FALSE;
      globalPtr.north = FALSE;    
      globalPtr.west = FALSE;
      globalPtr.south = TRUE;
    }
             
    // step 3: generate train size
    globalPtr.trainSize = randomInteger(2,9);         
    
   // step 4: print things
  static char waffleThingy[] = {"Train Size: ", globalPtr.trainSize, "\0"};
  RIT128x96x4StringDraw(waffleThingy, 10, 10, 15);
  
  //step 5: set flag to pop this mother from the stack
  globalPtr.trainComComplete = TRUE;
  }
}
  return;
}
             //NOTE: currentTrain has its own best nest
void NuSwitchControl(void* localData, void* globalData){
               return;
             }
             
             void SerialComTask(void* localData, void* globalData){
               return;
             }
             
             
             
             
             
