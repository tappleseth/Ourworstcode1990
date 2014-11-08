

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

void NuTrainCom(void* localData, void* globalData){
  // current* ptr = (trainComData*)data;
  return;
}
//NOTE: currentTrain has its own best nest

void NuSwitchControl(void* localData, void* globalData){
  return;
}

void SerialComTask(void* localData, void* globalData){
  return;
}




