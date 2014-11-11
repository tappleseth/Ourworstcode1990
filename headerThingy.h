#ifndef HEADERTHINGY_H
#define HEADERTHINGY_H

#define MAX_QUEUE_LENGTH        6
#define DELAY                   250000
#define TASK_SELECT             2
#define HIGH                    TRUE
#define LOW                     FALSE
#define SOUND_ENABLE            1

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

extern boringTom* head;
extern boringTom* current;

//FUNCTION PROTOTYPES
void Startup(void);

void addToStack(boringTom* addMe);
int getStackSize(void);
void popFromStack(void);
void TomSchedule(void);

void NuTrainCom(void* localData, void* globalData);
void CurrentTrain(void* localData, void* globalData);
void NuSwitchControl(void* localData, void* globalData);
void SerialComTask(void* localData, void* globalData);

void IntTimer0(void);
int randomInteger(int a, int b);
void Delay(int* foolioJenkins);
void pin(bool);
void IntGPIOe(void);
//END FUNCTION PROTOTYPES

//GLOBAL VARIABLE DECLARATIONS

extern unsigned int TrainState;
extern unsigned int TimerState;
extern unsigned int seed;


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
  unsigned int startTime;
  bool trainComComplete;
  bool currentTrainComplete;
  bool switchConComplete;
  unsigned char fromDirection;
} globalData;

typedef struct {
  unsigned int trainSize;
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
