#define MAX_QUEUE_LENGTH        6
#define DELAY                   250000
#define TASK_SELECT             2
#define HIGH                    TRUE
#define LOW                     FALSE

#define PORT_DATA  (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7)  // full 8 bits of port used

/*
THOMAS APPLESETH && BRANDON JOHANSON && JUSTIN "JUSTIN" TAFT
Professor A. Ecker
EE 472, AUT 2014
Project 2
10/23/2014

DESC: A delightful little bit of embedded programming that simulates a train switching yard. 

Random number generator code provided by professor, originally contained in "rand1.c"
*/

typedef struct {
  void (*justTrainTaskThings)(void*);
  void* trainTaskPtr;
} TCB;

typedef enum {FALSE = 0, TRUE = 1} bool;

void TrainCom(void* data);
void SwitchControl(void* data);
void NorthTrain(void* data);
void EastTrain(void* data);
void WestTrain(void* data);
void Schedule(void* data);

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
  unsigned int* globalCount;
} scheduleData;
