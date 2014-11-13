#ifndef HEADERTHINGY_H
#define HEADERTHINGY_H


#define MAX_QUEUE_LENGTH        6
#define DELAY                   250000
#define TASK_SELECT             2
#define HIGH                    TRUE
#define LOW                     FALSE
#define SOUND_ENABLE            0

#define PORT_DATA               (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7)  // full 8 bits of port used

//PRIMITIVE TYPE DEFINITION
typedef enum {FALSE = 0, TRUE = 1} bool;

typedef struct {
  void (*justTrainTaskThings)(void*,void*);
  void* localDataPtr;
  void* globalDataPtr;
  void* next;
  void* previous;
} boringTom;

//GLOBAL VARIABLE DECLARATIONS
extern boringTom* head;
extern boringTom* current;
extern unsigned int tempCount;
extern unsigned int frequencyCount;
extern unsigned int seed;
extern unsigned int TrainState;
extern unsigned int TimerState;
extern unsigned char globalCountArray[10];
extern unsigned char passCountArray[4];
//extern unsigned char numCars[2];

//FUNCTION PROTOTYPES
extern void Startup(void);

extern void addToStack(boringTom* addMe);
extern int getStackSize(void);
extern void popFromStack(void);
extern void TomSchedule(void);

extern void NuTrainCom(void* localData, void* globalData);
extern void CurrentTrain(void* localData, void* globalData);
extern void NuSwitchControl(void* localData, void* globalData);
extern void SerialComTask(void* localData, void* globalData);

extern void IntTimer0(void);
extern int randomInteger(int a, int b);
extern void Delay(int* foolioJenkins);
extern void pin(bool);
extern void IntGPIOe(void);
extern void IntGPIOd(void);

extern void UARTIntHandler(void);
extern void UARTSend(const unsigned char *pucBuffer, unsigned long ulCount);
//END FUNCTION PROTOTYPES






//END GLOBAL VARIABLE DECLARATIONS

//STRUCT DEFINITIONS


typedef struct {  
  bool north;
  bool east;
  bool west;
  bool south;
  bool gridlock;
  bool trainPresent;  
  unsigned char stateTom;
  unsigned int globalCount;
  unsigned int trainSize;
  unsigned int traversalTime;
  //so we can have train run after gridlock:
  unsigned int gridlockTime;
  unsigned int startTime;
  bool trainComComplete;
  bool currentTrainComplete;
  bool switchConComplete;
  unsigned char fromDirection;
  double passengerCount;
} globalData;

typedef struct {
  bool north;
  bool east;
  bool west;
  bool  gridlock;
  bool trainPresent;  
  unsigned int trainSize;
  unsigned int globalCount;
} trainComData;

typedef struct {
  bool toggleNorth;
  bool toggleSouth;
  bool toggleWest;
  bool toggleEast;
} currentTrainData;

typedef struct {
  unsigned int startTime;
} switchControlData;

typedef struct {
  bool data;
} serialCommunicationsData;

#endif
//END STRUCT DEFINITIONS
