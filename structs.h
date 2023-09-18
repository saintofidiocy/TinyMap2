#ifndef H_STRUCTS
#define H_STRUCTS
#include <stdlib.h>

#include <pshpack1.h>

// ---- CHK Sections ----
typedef struct {
  u32 name;
  u32 size;
} CHKH;

typedef struct{
  u32 classInstance;
  u16 x;
  u16 y;
  u16 unitID;
  u16 bldgRelation;
  u32 validFlags;
  u8  player;
  u8  hp;
  u8  shields;
  u8  energy;
  u32 resources;
  u16 hangar;
  u16 stateFlags;
  u32 unused;
  u32 relatedBldg;
} UNIT;

typedef struct{
  u16 id;
  u16 x;
  u16 y;
  u16 player;
  u16 flags;
} THG2;

typedef struct{
  u32 validFlags;
  u8  player;
  u8  hp;
  u8  shields;
  u8  energy;
  u32 resources;
  u16 hangar;
  u16 stateFlags;
  u32 unused;
} UPRP;

typedef struct{
  u32 x1;
  u32 y1;
  u32 x2;
  u32 y2;
  u16 string;
  u16 flags;
} MRGN;

typedef struct{
  u32 location;
  u32 player;
  u32 number;
  u16 unit;
  u8  modifier;
  u8  condition;
  u8  type;
  u8  flags;
  u16 unused;
} CONDITION;

typedef struct{
  u32 location;
  u32 text;
  u32 wav;
  u32 time;
  u32 player;
  u32 dest;
  u16 type;
  u8  action;
  u8  modifier;
  u8  flags;
  u8  unused1;
  u16 unused2;
} ACTION;

typedef struct{
  CONDITION conditions[16];
  ACTION actions[64];
  u32 flags;
  u8 players[28];
} TRIG;

typedef struct{
  u32 seeds[256];
  u8  opcodes[16];
} VCOD;

typedef struct{
  u16 w;
  u16 h;
} DIM;

typedef struct{
  u32  count;
  u16* offsets;
  u32* offsetsX;
  u8** data;
} STR;

typedef struct{
  u8  players[8];
  u16 strings[4];
  u8  flags[4];
} FORC;

typedef struct{
  u8 available[12][228];
  u8 global[228];
  u8 useGlobal[12][228];
} PUNI;

typedef struct{
  u8 playerMax[12][46];
  u8 playerLevel[12][46];
  u8 globalMax[46];
  u8 globalLevel[46];
  u8 useGlobal[12][46];
} UPGR;

typedef struct{
  u8 playerMax[12][61];
  u8 playerLevel[12][61];
  u8 globalMax[61];
  u8 globalLevel[61];
  u8 useGlobal[12][61];
} PUPx;

typedef struct{
  u8 playerEnabled[12][24];
  u8 playerResearched[12][24];
  u8 globalEnabled[24];
  u8 globalResearched[24];
  u8 useGlobal[12][24];
} PTEC;

typedef struct{
  u8 playerEnabled[12][44];
  u8 playerResearched[12][44];
  u8 globalEnabled[44];
  u8 globalResearched[44];
  u8 useGlobal[12][44];
} PTEx;

typedef struct{
  u8  useDefaults[228];
  s32 hp[228];
  u16 shields[228];
  u8  armor[228];
  u16 time[228];
  u16 minerals[228];
  u16 gas[228];
  u16 string[228];
  u16 damage[100];
  u16 bonus[100];
} UNIS;

typedef struct{
  u8  useDefaults[228];
  s32 hp[228];
  u16 shields[228];
  u8  armor[228];
  u16 time[228];
  u16 minerals[228];
  u16 gas[228];
  u16 string[228];
  u16 damage[130];
  u16 bonus[130];
} UNIx;

typedef struct{
  u8  useDefaults[46];
  u16 mineralBase[46];
  u16 mineralFactor[46];
  u16 gasBase[46];
  u16 gasFactor[46];
  u16 timeBase[46];
  u16 timeFactor[46];
} UPGS;

typedef struct{
  u8  useDefaults[61];
  u8  unused;
  u16 mineralBase[61];
  u16 mineralFactor[61];
  u16 gasBase[61];
  u16 gasFactor[61];
  u16 timeBase[61];
  u16 timeFactor[61];
} UPGx;

typedef struct{
  u8  useDefaults[24];
  u16 minerals[24];
  u16 gas[24];
  u16 time[24];
  u16 energy[24];
} TECS;

typedef struct{
  u8  useDefaults[44];
  u16 minerals[44];
  u16 gas[44];
  u16 time[44];
  u16 energy[44];
} TECx;

typedef struct{
  u8 rgb[8][3];
  u8 player[8];
} CRGB;
#define _DLGS_H // stop yelling that CRGB already exists


// ---- DAT Files ----

// Units.dat
typedef struct {
  u8  flingy[228];
  u16 subunit1[228];
  u16 subunit2[228];
  u16 infestation[96];
  u32 construction[228];
  u8  direction[228];
  u8  hasShields[228];
  u16 shields[228];
  u32 hp[228];
  u8  elevation[228];
  u8  unknown[228];
  u8  rank[228];
  u8  aicompidle[228];
  u8  aihumanidle[228];
  u8  returntoidle[228];
  u8  attackunit[228];
  u8  attackmove[228];
  u8  gndWeapon[228];
  u8  gndMaxHits[228];
  u8  airWeapon[228];
  u8  airMaxHits[228];
  u8  aiInternal[228];
  u32 abilFlags[228];
  u8  targetRange[228];
  u8  sightRange[228];
  u8  armorUpgrade[228];
  u8  unitDamageSize[228];
  u8  armor[228];
  u8  rightClickAction[228];
  u16 sndReady[106];
  u16 sndWhatStart[228];
  u16 sndWhatEnd[228];
  u16 sndPissStart[106];
  u16 sndPissEnd[106];
  u16 sndYesStart[106];
  u16 sndYesEnd[106];
  s16 placeBox[456]; // not referenced; actual struct isn't important
  u16 addonPosition[192]; // not referenced; actual struct isn't important
  u16 unitSize[912]; // not referenced; actual struct isn't important
  u16 portrait[228];
  u16 mineralCost[228];
  u16 gasCost[228];
  u16 buildTime[228];
  u16 unknown1[228];
  u8  groupFlags[228];
  u8  supplyProvided[228];
  u8  supplyRequired[228];
  u8  spaceRequired[228];
  u8  spaceProvided[228];
  u16 buildScore[228];
  u16 destroyScore[228];
  u16 mapString[228];
  u8  broodwar[228];
  u16 availFlags[228];
} UNITSDAT;

typedef struct {
  u16 label[130];
  u32 graphics[130];
  u8  unused[130];
  u16 targetFlags[130];
  u32 minRange[130];
  u32 maxRange[130];
  u8  damageUpgrade[130];
  u8  weaponType[130];
  u8  weaponBehavior[130];
  u8  removeAfter[130];
  u8  explosionType[130];
  u16 innerSplash[130];
  u16 mediumSplash[130];
  u16 outerSplash[130];
  u16 damageAmount[130];
  u16 damageBonus[130];
  u8  weaponCooldown[130];
  u8  damageFactor[130];
  u8  attackAngle[130];
  u8  launchSpin[130];
  u8  xOffset[130];
  u8  yOffet[130];
  u16 targetError[130];
  u16 icon[130];
} WEAPONSDAT;

// Upgrades.dat
typedef struct{
  u16 mineralCost[61];
  u16 mineralFactor[61];
  u16 gasCost[61];
  u16 gasFactor[61];
  u16 timeCost[61];
  u16 timeFactor[61];
  u16 unknown[61];
  u16 icon[61];
  u16 label[61];
  u8  race[61];
  u8  max[61];
  u8  broodwar[61];
} UPGRADESDAT;

// Techdata.dat
typedef struct{
  u16 mineralCost[44];
  u16 gasCost[44];
  u16 time[44];
  u16 energy[44];
  u32 unknown[44];
  u16 icon[44];
  u16 label[44];
  u8  race[44];
  u8  unused[44];
  u8  broodwar[44];
} TECHDATADAT;





// ---- WAV Headers ----

typedef struct{
  u32 ckID;
  u32 cksize;
  u32 WAVEID;
} RIFF;

typedef struct{
  u16 wFormatTag;
  u16 nChannels;
  u32 nSamplesPerSec;
  u32 nAvgBytesPerSec;
  u16 nBlockAlign;
  u16 wBitsPerSample;
} wavhdrfmt;

typedef struct{
  u32 signature;
  u8  version;
  u8  flags;
  u64 position;
  u32 serial;
  u32 sequence;
  u32 checksum;
  u8  segments;
  u8  segTable[1];
} ogghdr;



// ---- Internal Structs ----

typedef struct{
  u8* data;
  char filename[MAX_PATH];
  u32 initSize;
  u32 initCompSize;
  u32 saveSize;
  u32 saveCompSize;
  struct{
    u32 trig;
    u32 mbrf;
  } wavUsage;
  u32 mpqFlags;
  u32 comp; // Previously used compression settings
} MapFile;

typedef struct {
  u32 sectionID;
  u32 sectionName;
  u32 loadType;
  void* data;
  u32 size;
  u32* count;
} SectionDef;

typedef struct {
  u16 fields;
  u8  flagMask;
} ActDef;

typedef struct {
  TRIG* section;
  u32 flags;
  u32 trigIndex;
  u32 maxIndex;
  u32 condIndex;
  u32 actIndex;
} TrigIterator;

#include <poppack.h>
#endif
