#include "types.h"

typedef struct {
  CHKH header;
  void* data;
  short id;
  short used;
} writesection;

// Section Stacker
typedef struct {
  bool isStacked;
  int startid; // triglist ID
  int count;
  int blocks;
  int chunkSize;
  int totalBlocks;
} writeentry;

extern int loadedGameType;
extern int gameType;
extern int compatability;
extern bool delGameName;
extern bool trimMTXM;
extern bool delSPActions;

void loadCHKData(u8* data, u32 filesize);
void clearCHK();
void detectGameType();
u32 getOutputType();

void genDefaultListfile();
void countWAVs(MapFile* mapFiles, u32 mapFileCount);

u8* createCHKBuffer(int outputType);
u32 createMeleeCHK(u8* buffer, int mode);
u32 createUMSCHK(u8* buffer, int mode);

void initTrigIterator(TrigIterator* ti, u32 flags);
bool getNextTrig(TrigIterator* ti);
CONDITION* getNextCondition(TrigIterator* ti);
ACTION* getNextAction(TrigIterator* ti);
