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

static boringTom* head;
static boringTom* current;


//FUNCTION PROTOTYPES
void Startup(void);
void Schedule(void* data);
void TrainCom(void* data);
void CurrentTrain(void*, void*);
void SwitchControl(void* data);
void SerialCommunications(void* data);

void addToStack(boringTom* addMe);
int getStackSize(void);
void popFromStack(void);

void NorthTrain(void* data);
void EastTrain(void* data);
void WestTrain(void* data);
void TomSchedule(void);
void NuTrainCom(void* localData, void* globalData);
void CurrentTrain(void* localData, void* globalData);
void NuSwitchControl(void* localData, void* globalData);
void SerialComTask(void* localData, void* globalData);
//END FUNCTION PROTOTYPES

//GLOBAL VARIABLE DECLARATIONS

static unsigned int TrainState;
static unsigned int TimerState;


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
  unsigned int* globalCount;
} scheduleData;

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
  bool north;
  bool east;
  bool west;
  bool gridlock;
  bool trainPresent;
  unsigned int trainSize;
  unsigned int globalCount;
  unsigned int traversalTime;
  unsigned int startTime;
} switchControlData;

typedef struct {
  bool data;
} serialCommunicationsData;

//END STRUCT DEFINITIONS
