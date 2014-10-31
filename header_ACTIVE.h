#define MAX_QUEUE_LENGTH        6
#define DELAY                   250000
#define TASK_SELECT             2
#define HIGH                    TRUE
#define LOW                     FALSE
#define SOUND_ENABLE            0

#define PORT_DATA               (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7)  // full 8 bits of port used

//PRIMITIVE TYPE DEFINITION
typedef enum {FALSE = 0, TRUE = 1} bool;

//FUNCTION PROTOTYPES
void Startup(void);
void Schedule(void* data);
void TrainCom(void* data);
void CurrentTrain(void* data);
void SwitchControl(void* data);
void SerialCommunications(void* data);

void NorthTrain(void* data);
void EastTrain(void* data);
void WestTrain(void* data);
//END FUNCTION PROTOTYPES

//GLOBAL VARIABLE DECLARATIONS
static bool north;
static bool east;
static bool west;

static bool toggleNorth;
static bool toggleEast;
static bool toggleWest;

static bool gridlock;
static bool trainPresent;
static unsigned int trainSize;
static unsigned int globalCount;
static unsigned int traversalTime;
static unsigned int startTime;

static int seed;
//END GLOBAL VARIABLE DECLARATIONS

//STRUCT DEFINITIONS
typedef struct {
  void (*justTrainTaskThings)(void*);
  void* localDataPtr;
  //void* globalDataPtr;
} TCB;

typedef struct {  
  bool* north;
  bool* east;
  bool* west;
  
  bool* gridlock;
  bool* trainPresent;  
  unsigned int* globalCount;
  unsigned int* trainSize;
  unsigned int* traversalTime;
  unsigned int* startTime;
} globalData;

typedef struct {  
  unsigned int* globalCount;
} scheduleData;

typedef struct {
  bool* north;
  bool* east;
  bool* west;
  bool* gridlock;
  bool* trainPresent;  
  unsigned int* trainSize;
  unsigned int* globalCount;
} trainComData;

typedef struct {
  bool* data;
} currentTrainData;

typedef struct {
  bool* north;
  bool* east;
  bool* west;
  bool* gridlock;
  bool* trainPresent;
  unsigned int* trainSize;
  unsigned int* globalCount;
  unsigned int* traversalTime;
  unsigned int* startTime;
} switchControlData;

typedef struct {
  bool* data;
} serialCommunicationsData;




typedef struct {
  bool* toggleNorth;  
  bool* north;
  unsigned int* globalCount;
  unsigned int* traversalTime;
} northTrainData;

typedef struct {
  bool* toggleWest;  
  bool* west;
  unsigned int* globalCount;
  unsigned int* traversalTime;
} westTrainData;

typedef struct {
  bool* toggleEast;
  bool* east;
  unsigned int* globalCount;
  unsigned int* traversalTime;
} eastTrainData;

//END STRUCT DEFINITIONS
