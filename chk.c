#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "chk.h"
#include "options.h"
#include "data.h"

#include "buildops.h"

// CHK Data
u32   chkTYPE;
u16   chkVER;
VCOD  chkVCOD;
u8    chkOWNR[12];
u16   chkERA;
DIM   chkDIM;
u8    chkSIDE[12];
u16   chkMTXM[65536];
PUNI  chkPUNI;
UPGR  chkUPGR;
PTEC  chkPTEC;
UNIT* chkUNIT;
u32*  unitlist = NULL; // chkUNIT ID
writeentry* unitwrite = NULL;
THG2* chkTHG2;
u8    chkMASK[65536];
STR   chkSTR;
UPRP  chkUPRP[64];
MRGN  chkMRGN[255];
TRIG* chkTRIG;
TRIG* chkMBRF;
u32*  triglist = NULL; // chkTRIG ID
writeentry* trigwrite = NULL;
u16   chkSPRP[2];
FORC  chkFORC;
UNIS  chkUNIS;
UPGS  chkUPGS;
TECS  chkTECS;
u8    chkCOLR[8];
PUPx  chkPUPx;
PTEx  chkPTEx;
UNIx  chkUNIx;
UPGx  chkUPGx;
TECx  chkTECx;
CRGB  chkCRGB;

u32 mapSize; // mapDIM.x * mapDIM.y
u32 mtxmSize;
u32 maskSize;
u32 trigCount, trigUnique, trigWCount, trigMax;
u32 mbrfCount;
u32 unitCount, unitUnique, unitWCount;
u32 thg2Count;
u32 forcSize;


SectionDef sections[SECTION_COUNT] = {
  {SECT_TYPE_ID, SECT_TYPE, LOADSIZE_CONST, &chkTYPE, sizeof(chkTYPE), NULL},
  {SECT_VER_ID,  SECT_VER,  LOADSIZE_CONST, &chkVER,  sizeof(chkVER),  NULL},
  {SECT_VCOD_ID, SECT_VCOD, LOADSIZE_CONST, &chkVCOD, sizeof(chkVCOD), NULL},
  {SECT_OWNR_ID, SECT_OWNR, LOADSIZE_CONST,  chkOWNR, sizeof(chkOWNR), NULL},
  {SECT_ERA_ID,  SECT_ERA,  LOADSIZE_CONST, &chkERA,  sizeof(chkERA),  NULL},
  {SECT_DIM_ID,  SECT_DIM,  LOADSIZE_CONST, &chkDIM,  sizeof(chkDIM),  NULL},
  {SECT_SIDE_ID, SECT_SIDE, LOADSIZE_CONST,  chkSIDE, sizeof(chkSIDE), NULL},
  {SECT_MTXM_ID, SECT_MTXM, LOADSIZE_MAX,    chkMTXM, sizeof(chkMTXM), &mtxmSize},
  {SECT_PUNI_ID, SECT_PUNI, LOADSIZE_CONST, &chkPUNI, sizeof(chkPUNI), NULL},
  {SECT_UPGR_ID, SECT_UPGR, LOADSIZE_CONST, &chkUPGR, sizeof(chkUPGR), NULL},
  {SECT_PTEC_ID, SECT_PTEC, LOADSIZE_CONST, &chkPTEC, sizeof(chkPTEC), NULL},
  {SECT_UNIT_ID, SECT_UNIT, LOADSIZE_ENTRY, &chkUNIT, sizeof(UNIT),    &unitCount},
  {SECT_THG2_ID, SECT_THG2, LOADSIZE_ENTRY, &chkTHG2, sizeof(THG2),    &thg2Count},
  {SECT_MASK_ID, SECT_MASK, LOADSIZE_MASK,   chkMASK, sizeof(chkMASK), NULL},
  {SECT_STR_ID,  SECT_STR,  LOADSIZE_STR,   &chkSTR,  0,               NULL},
  {SECT_UPRP_ID, SECT_UPRP, LOADSIZE_CONST,  chkUPRP, sizeof(chkUPRP), NULL},
  {SECT_MRGN_ID, SECT_MRGN, LOADSIZE_MGRN,   chkMRGN, sizeof(chkMRGN), NULL},
  {SECT_TRIG_ID, SECT_TRIG, LOADSIZE_ENTRY, &chkTRIG, sizeof(TRIG),    &trigCount},
  {SECT_MBRF_ID, SECT_MBRF, LOADSIZE_ENTRY, &chkMBRF, sizeof(TRIG),    &mbrfCount},
  {SECT_SPRP_ID, SECT_SPRP, LOADSIZE_CONST,  chkSPRP, sizeof(chkSPRP), NULL},
  {SECT_FORC_ID, SECT_FORC, LOADSIZE_MAX,   &chkFORC, sizeof(chkFORC), &forcSize},
  {SECT_UNIS_ID, SECT_UNIS, LOADSIZE_CONST, &chkUNIS, sizeof(chkUNIS), NULL},
  {SECT_UPGS_ID, SECT_UPGS, LOADSIZE_CONST, &chkUPGS, sizeof(chkUPGS), NULL},
  {SECT_TECS_ID, SECT_TECS, LOADSIZE_CONST, &chkTECS, sizeof(chkTECS), NULL},
  {SECT_COLR_ID, SECT_COLR, LOADSIZE_CONST,  chkCOLR, sizeof(chkCOLR), NULL},
  {SECT_PUPx_ID, SECT_PUPx, LOADSIZE_CONST, &chkPUPx, sizeof(chkPUPx), NULL},
  {SECT_PTEx_ID, SECT_PTEx, LOADSIZE_CONST, &chkPTEx, sizeof(chkPTEx), NULL},
  {SECT_UNIx_ID, SECT_UNIx, LOADSIZE_CONST, &chkUNIx, sizeof(chkUNIx), NULL},
  {SECT_UPGx_ID, SECT_UPGx, LOADSIZE_CONST, &chkUPGx, sizeof(chkUPGx), NULL},
  {SECT_TECx_ID, SECT_TECx, LOADSIZE_CONST, &chkTECx, sizeof(chkTECx), NULL},
  {SECT_STRx_ID, SECT_STRx, LOADSIZE_STR,   &chkSTR,  0,               NULL},
  {SECT_CRGB_ID, SECT_CRGB, LOADSIZE_CONST, &chkCRGB, sizeof(chkCRGB), NULL}
};

const ActDef Conditions[COND_COUNT] = {
/*COND_NO_CONDITION               */ {0, 0},
/*COND_COUNTDOWN_TIMER            */ {CDEF_NUMBER | CDEF_MODIFIER, 0},
/*COND_COMMAND                    */ {CDEF_PLAYER | CDEF_NUMBER | CDEF_UNIT | CDEF_MODIFIER, 0},
/*COND_BRING                      */ {CDEF_LOCATION | CDEF_PLAYER | CDEF_NUMBER | CDEF_UNIT | CDEF_MODIFIER, 0},
/*COND_ACCUMULATE                 */ {CDEF_PLAYER | CDEF_NUMBER | CDEF_MODIFIER | CDEF_TYPE, 0},
/*COND_KILL                       */ {CDEF_PLAYER | CDEF_NUMBER | CDEF_UNIT | CDEF_MODIFIER, 0},
/*COND_COMMAND_THE_MOST           */ {CDEF_UNIT, 0},
/*COND_COMMAND_THE_MOST_AT        */ {CDEF_LOCATION | CDEF_UNIT, 0},
/*COND_MOST_KILLS                 */ {CDEF_UNIT, 0},
/*COND_HIGHEST_SCORE              */ {CDEF_TYPE, 0},
/*COND_MOST_RESOURCES             */ {CDEF_TYPE, 0},
/*COND_SWITCH                     */ {CDEF_MODIFIER | CDEF_TYPE, 0},
/*COND_ELAPSED_TIME               */ {CDEF_NUMBER | CDEF_MODIFIER, 0},
/*COND_MISSION_BRIEFING           */ {ADEF_IGNORE, 0},
/*COND_OPPONENTS                  */ {CDEF_PLAYER | CDEF_NUMBER | CDEF_MODIFIER, 0},
/*COND_DEATHS                     */ {CDEF_PLAYER | CDEF_NUMBER | CDEF_UNIT | CDEF_MODIFIER | CDEF_FLAGS | CDEF_UNUSED, TRIG_UNITTYPEUSED},
/*COND_COMMAND_THE_LEAST          */ {CDEF_UNIT, 0},
/*COND_COMMAND_THE_LEAST_AT       */ {CDEF_LOCATION | CDEF_UNIT, 0},
/*COND_LEAST_KILLS                */ {CDEF_UNIT, 0},
/*COND_LOWEST_SCORE               */ {CDEF_TYPE, 0},
/*COND_LEAST_RESOURCES            */ {CDEF_TYPE, 0},
/*COND_SCORE                      */ {CDEF_PLAYER | CDEF_NUMBER | CDEF_MODIFIER | CDEF_TYPE, 0},
/*COND_ALWAYS                     */ {ADEF_IGNORE, 0},
/*COND_NEVER                      */ {ADEF_IGNORE, 0},
};

const ActDef Actions[ACT_COUNT] = {
/*ACT_NO_ACTION                   */ {0, 0},
/*ACT_VICTORY                     */ {0, 0},
/*ACT_DEFEAT                      */ {0, 0},
/*ACT_PRESERVE_TRIGGER            */ {ADEF_VALID_IGNORE, 0},
/*ACT_WAIT                        */ {ADEF_TIME, 0},
/*ACT_PAUSE_GAME                  */ {ADEF_SP_ACT, 0},
/*ACT_UNPAUSE_GAME                */ {ADEF_SP_ACT, 0},
/*ACT_TRANSMISSION                */ {ADEF_LOCATION | ADEF_TEXT | ADEF_WAV | ADEF_DEST | ADEF_TYPE | ADEF_MODIFIER | ADEF_FLAGS, TRIG_ALWAYSDISPLAY},
/*ACT_PLAY_WAV                    */ {ADEF_WAV, 0},
/*ACT_DISPLAY_TEXT_MESSAGE        */ {ADEF_TEXT | ADEF_FLAGS, TRIG_ALWAYSDISPLAY},
/*ACT_CENTER_VIEW                 */ {ADEF_LOCATION, 0},
/*ACT_CREATE_UNITS_WITH_PROPERTIES*/ {ADEF_LOCATION | ADEF_PLAYER | ADEF_DEST | ADEF_TYPE | ADEF_MODIFIER, 0},
/*ACT_SET_MISSION_OBJECTIVES      */ {ADEF_TEXT, 0},
/*ACT_SET_SWITCH                  */ {ADEF_DEST | ADEF_MODIFIER, 0},
/*ACT_SET_COUNTDOWN_TIMER         */ {ADEF_MODIFIER | ADEF_TIME, 0},
/*ACT_RUN_AI_SCRIPT               */ {ADEF_DEST, 0},
/*ACT_RUN_AI_SCRIPT_AT            */ {ADEF_LOCATION | ADEF_DEST, 0},
/*ACT_LEADERBOARD_CONTROL         */ {ADEF_TEXT | ADEF_TYPE, 0},
/*ACT_LEADERBOARD_CONTROL_AT      */ {ADEF_LOCATION | ADEF_TEXT | ADEF_TYPE, 0},
/*ACT_LEADERBOARD_RESOURCES       */ {ADEF_TEXT | ADEF_TYPE, 0},
/*ACT_LEADERBOARD_KILLS           */ {ADEF_TEXT | ADEF_TYPE, 0},
/*ACT_LEADERBOARD_POINTS          */ {ADEF_TEXT | ADEF_TYPE, 0},
/*ACT_KILL_UNIT                   */ {ADEF_PLAYER | ADEF_TYPE, 0},
/*ACT_KILL_UNITS_AT               */ {ADEF_LOCATION | ADEF_PLAYER | ADEF_TYPE | ADEF_MODIFIER, 0},
/*ACT_REMOVE_UNIT                 */ {ADEF_PLAYER | ADEF_TYPE, 0},
/*ACT_REMOVE_UNITS_AT             */ {ADEF_LOCATION | ADEF_PLAYER | ADEF_TYPE | ADEF_MODIFIER, 0},
/*ACT_SET_RESOURCES               */ {ADEF_PLAYER | ADEF_DEST | ADEF_TYPE | ADEF_MODIFIER, 0},
/*ACT_SET_SCORE                   */ {ADEF_PLAYER | ADEF_DEST | ADEF_TYPE | ADEF_MODIFIER, 0},
/*ACT_MINIMAP_PING                */ {ADEF_LOCATION, 0},
/*ACT_TALKING_PORTRAIT            */ {ADEF_TIME | ADEF_TYPE, 0},
/*ACT_MUTE_UNIT_SPEECH            */ {0, 0},
/*ACT_UNMUTE_UNIT_SPEECH          */ {0, 0},
/*ACT_LEADERBOARD_COMPUTER_PLAYERS*/ {ADEF_MODIFIER, 0},
/*ACT_LEADERBOARD_GOAL_CONTROL    */ {ADEF_TEXT | ADEF_DEST | ADEF_TYPE, 0},
/*ACT_LEADERBOARD_GOAL_CONTROL_AT */ {ADEF_LOCATION | ADEF_TEXT | ADEF_DEST | ADEF_TYPE, 0},
/*ACT_LEADERBOARD_GOAL_RESOURCES  */ {ADEF_TEXT | ADEF_DEST | ADEF_TYPE, 0},
/*ACT_LEADERBOARD_GOAL_KILLS      */ {ADEF_TEXT | ADEF_DEST | ADEF_TYPE, 0},
/*ACT_LEADERBOARD_GOAL_POINTS     */ {ADEF_TEXT | ADEF_DEST | ADEF_TYPE, 0},
/*ACT_MOVE_LOCATION               */ {ADEF_LOCATION | ADEF_PLAYER | ADEF_DEST_LOC | ADEF_TYPE, 0},
/*ACT_MOVE_UNITS                  */ {ADEF_LOCATION | ADEF_PLAYER | ADEF_DEST_LOC | ADEF_TYPE | ADEF_MODIFIER, 0},
/*ACT_LEADERBOARD_GREED           */ {ADEF_DEST, 0},
/*ACT_SET_NEXT_SCENARIO           */ {ADEF_TEXT | ADEF_SP_ACT, 0},
/*ACT_SET_DOODAD_STATE            */ {ADEF_LOCATION | ADEF_PLAYER | ADEF_TYPE | ADEF_MODIFIER, 0},
/*ACT_SET_INVINCIBILITY           */ {ADEF_LOCATION | ADEF_PLAYER | ADEF_TYPE | ADEF_MODIFIER, 0},
/*ACT_CREATE_UNITS                */ {ADEF_LOCATION | ADEF_PLAYER | ADEF_TYPE | ADEF_MODIFIER, 0},
/*ACT_SET_DEATHS                  */ {ADEF_PLAYER | ADEF_DEST | ADEF_TYPE | ADEF_MODIFIER | ADEF_FLAGS | ADEF_UNUSED, TRIG_UNITTYPEUSED},
/*ACT_ORDER                       */ {ADEF_LOCATION | ADEF_PLAYER | ADEF_DEST_LOC | ADEF_TYPE | ADEF_MODIFIER, 0},
/*ACT_COMMENT                     */ {ADEF_IGNORE, 0},
/*ACT_GIVE_UNITS_TO_PLAYER        */ {ADEF_LOCATION | ADEF_PLAYER | ADEF_DEST | ADEF_TYPE | ADEF_MODIFIER, 0},
/*ACT_MODIFY_UNIT_HIT_POINTS      */ {ADEF_LOCATION | ADEF_PLAYER | ADEF_DEST | ADEF_TYPE | ADEF_MODIFIER, 0},
/*ACT_MODIFY_UNIT_ENERGY          */ {ADEF_LOCATION | ADEF_PLAYER | ADEF_DEST | ADEF_TYPE | ADEF_MODIFIER, 0},
/*ACT_MODIFY_UNIT_SHIELD_POINTS   */ {ADEF_LOCATION | ADEF_PLAYER | ADEF_DEST | ADEF_TYPE | ADEF_MODIFIER, 0},
/*ACT_MODIFY_UNIT_RESOURCE_AMOUNT */ {ADEF_LOCATION | ADEF_PLAYER | ADEF_DEST | ADEF_MODIFIER, 0},
/*ACT_MODIFY_UNIT_HANGAR_COUNT    */ {ADEF_LOCATION | ADEF_PLAYER | ADEF_DEST | ADEF_TYPE | ADEF_MODIFIER, 0},
/*ACT_PAUSE_TIMER                 */ {0, 0},
/*ACT_UNPAUSE_TIMER               */ {0, 0},
/*ACT_DRAW                        */ {0, 0},
/*ACT_SET_ALLIANCE_STATUS         */ {ADEF_PLAYER | ADEF_TYPE, 0},
/*ACT_DISABLE_DEBUG_MODE          */ {ADEF_SP_ACT, 0},
/*ACT_ENABLE_DEBUG_MODE           */ {ADEF_SP_ACT, 0},
};

const ActDef BriefingActions[BACT_COUNT] = {
/*BACT_NO_ACTION                  */ {0, 0},
/*BACT_WAIT                       */ {ADEF_TIME, 0},
/*BACT_PLAY_WAV                   */ {ADEF_WAV | ADEF_TIME, 0},
/*BACT_TEXT_MESSAGE               */ {ADEF_TEXT | ADEF_TIME, 0},
/*BACT_MISSION_OBJECTIVES         */ {ADEF_TEXT, 0},
/*BACT_SHOW_PORTRAIT              */ {ADEF_PLAYER | ADEF_TYPE, 0},
/*BACT_HIDE_PORTRAIT              */ {ADEF_PLAYER, 0},
/*BACT_DISPLAY_SPEAKING_PORTRAIT  */ {ADEF_TIME | ADEF_PLAYER, 0},
/*BACT_TRANSMISSION               */ {ADEF_TEXT | ADEF_WAV | ADEF_TIME | ADEF_PLAYER | ADEF_MODIFIER, 0},
/*BACT_SKIP_TUTORIAL              */ {ADEF_SP_ACT, 0},
};

bool loadedSections[SECTION_COUNT] = {0};
u8 usedUnits[228] = {0}; // Uses USEDUNIT_* flags to determine if a unit is preplaced, enabled, or can be built in-game
u8 usedWeapons[100] = {0};
s8 usedPlayers[8] = {0};
u8 playerReindex[8] = {0};
u8* usedStrings = NULL;
u16* stringReindex = NULL;
bool usedLocations[256] = {0};
u8 locationReindex[256] = {0};
u8 cuwpReindex[64] = {0};
u32 usedLocationCount = 0;
bool use255Locations = false;
bool randomStartLoc = false;
bool useFORCData = false;

int loadedGameType;
int gameType;
int compatability;
bool delGameName;
bool trimMTXM;
bool delSPActions;

writesection writeSectionData[SECTION_COUNT];
u32 writeSectionCount = 0;

void addSection(u32 name, u32 size, void* data);
u32 compileCHK(u8* buffer);
void writeSection(u8* buffer, u32* pointer, u32 name, u32 size, void* data);
void writeSTRSection(u8* buffer, u32* pointer);
void writeTRIGSection(u8* buffer, u32* pointer);
bool trigcmp(TRIG* a, TRIG* b);
void calcChunking(int count, int *blocks, int *chunkSize);

void initTrigIterator(TrigIterator* ti, u32 flags);
bool getNextTrig(TrigIterator* ti);
CONDITION* getNextCondition(TrigIterator* ti);
ACTION* getNextAction(TrigIterator* ti);

void procVCOD();
void procMTXM();
void procPUNI();
void procUPGR();
void procPUPx();
void procPTEC();
void procPTEx();
void procUNIT(bool melee);
void procTHG2(bool melee);
void procMASK();
void procSTR(bool melee);
void procUPRP();
void procMRGN(bool count);
void procTRIG();
void procMBRF();
void procSPRP();
void procFORC();
void procUNIS();
void procUNIx();
void procUPGS();
void procUPGx();
void procTECS();
void procTECx();
void procCRGB();

void testBuildable();



bool loadSection(u8* data, u32 filesize, SectionDef* section);

void loadCHKData(u8* data, u32 filesize){
  u32 i;
  
  clearCHK();
  
  for(i = 0; i < SECTION_COUNT; i++){
    loadedSections[sections[i].sectionID] = loadSection(data, filesize, &sections[i]);
  }
}

bool loadSection(u8* data, u32 filesize, SectionDef* section){
  u32 fptr = 0;
  CHKH head;
  bool found = false;
  
  while((fptr + sizeof(CHKH)) <= filesize){
    head = *(CHKH*)(data + fptr);
    fptr += sizeof(CHKH);
    if((fptr + head.size) > filesize){
      // read past end of file
      break;
    }
    if(head.name == section->sectionName) {
      switch(section->loadType) {
        case LOADSIZE_CONST:
          if(head.size == section->size){
            memcpy(section->data, data+fptr, head.size);
            found = true;
          }
          break;
        
        case LOADSIZE_MAX:
          if(head.size <= section->size){
            if(section->count != NULL){
              if(head.name == SECT_MTXM){
                *(section->count) = (head.size + 1) / 2;
              }else{
                *(section->count) = head.size;
              }
            }
            if(head.name == SECT_FORC) memset(section->data, 0, section->size);
            memcpy(section->data, data+fptr, head.size);
            found = true;
          }
          break;
        
        case LOADSIZE_ENTRY:
          if((head.size % section->size) == 0){
            // a bit of pointer hell
            u8* innerPtr = *(u8**)(section->data);
            innerPtr = realloc(innerPtr, *(section->count) * section->size + head.size);
            memcpy(&innerPtr[*(section->count) * section->size], data+fptr, head.size);
            *(section->count) += head.size / section->size;
            *(u8**)(section->data) = innerPtr;
            found = true;
          }
          break;
        
        case LOADSIZE_MGRN:
          if(head.size == 64*sizeof(MRGN) || head.size == 255*sizeof(MRGN)){
            use255Locations = (head.size == 255*sizeof(MRGN));
            memcpy(section->data, data+fptr, head.size);
            found = true;
          }
          break;
        
        case LOADSIZE_MASK:
          if(head.size > section->size){
            // trim excess data
            memcpy(section->data, data+fptr, section->size);
          }else if(head.size != 0){
            memcpy(section->data, data+fptr, head.size);
          }
          found = true;
          break;
        
        case LOADSIZE_STR:
        {
          u32 i;
          u8* strData;
          u32 strSize;
          u32 strOffs;
          u32 dataSize;
          if(head.name == SECT_STRx){
            dataSize = sizeof(u32);
          }else{
            dataSize = sizeof(u16);
          }
          // Clear any previously loaded strings
          if(chkSTR.data != NULL){
            for(i = 0; i < chkSTR.count; i++){
              if(chkSTR.data[i] != NULL) free(chkSTR.data[i]);
            }
            free(chkSTR.data);
            chkSTR.data = NULL;
          }
          if(chkSTR.offsets != NULL) free(chkSTR.offsets);
          chkSTR.offsets = NULL;
          if(chkSTR.offsetsX != NULL) free(chkSTR.offsetsX);
          chkSTR.offsetsX = NULL;
          if(usedStrings != NULL) free(usedStrings);
          usedStrings = NULL;
          if(stringReindex != NULL) free(stringReindex);
          stringReindex = NULL;
          //
          chkSTR.count = 0;
          memcpy(&chkSTR.count, data+fptr, dataSize);
          chkSTR.data = malloc(chkSTR.count * sizeof(u8*));
          if(dataSize == sizeof(u16)){
            chkSTR.offsets = malloc(chkSTR.count * sizeof(u16));
            memcpy(chkSTR.offsets, data + fptr + sizeof(u16), chkSTR.count * sizeof(u16));
          }else{
            chkSTR.offsetsX = malloc(chkSTR.count * sizeof(u32));
            memcpy(chkSTR.offsetsX, data + fptr + sizeof(u32), chkSTR.count * sizeof(u32));
          }
          strData = malloc(head.size - (chkSTR.count + 1)*dataSize + 1);
          strData[head.size - (chkSTR.count + 1)*dataSize] = 0; // ensure the last string is null-terminated
          memcpy(strData, data + fptr + (chkSTR.count + 1)*dataSize, head.size - (chkSTR.count + 1)*dataSize);
          for(i = 0; i < chkSTR.count; i++){
            chkSTR.data[i] = NULL;
            if(dataSize == sizeof(u16)){
              strOffs = chkSTR.offsets[i];
            }else{
              strOffs = chkSTR.offsetsX[i];
            }
            if(strOffs <= head.size){
              strOffs -= (chkSTR.count + 1)*dataSize;
              strSize = strlen(strData + strOffs);
              if(strSize != 0){
                chkSTR.data[i] = malloc(strSize + 1);
                strcpy(chkSTR.data[i], strData + strOffs);
              }
            }
          }
          usedStrings = calloc(chkSTR.count + 1, sizeof(u8));
          stringReindex = calloc(chkSTR.count + 1, sizeof(u32));
          free(strData);
          found = true;
          break;
        }
      }
    }
    fptr += head.size;
  }
  
  return found;
}

void clearCHK(){
  u32 i;
  chkTYPE = 0;
  chkVER = 0;
  memset(&chkVCOD, 0, sizeof(VCOD));
  memset(chkOWNR, 0, sizeof(chkOWNR));
  chkERA = 0;
  memset(&chkDIM, 0, sizeof(DIM));
  memset(chkSIDE, 0, sizeof(chkSIDE));
  memset(chkMTXM, 0, sizeof(chkMTXM));
  memset(&chkPUNI, 0, sizeof(PUNI));
  memset(&chkUPGR, 0, sizeof(UPGR));
  memset(&chkPTEC, 0, sizeof(PTEC));
  if(chkUNIT != NULL) free(chkUNIT);
  chkUNIT = NULL;
  if(unitlist != NULL) free(unitlist);
  unitlist = NULL;
  if(unitwrite != NULL) free(unitwrite);
  unitwrite = NULL;
  if(chkTHG2 != NULL) free(chkTHG2);
  chkTHG2 = NULL;
  memset(chkMASK, 0xFF, sizeof(chkMASK));
  for(i = 0; i < chkSTR.count; i++){
    if(chkSTR.data[i] != NULL) free(chkSTR.data[i]);
  }
  chkSTR.count = 0;
  if(chkSTR.data) free(chkSTR.data);
  chkSTR.data = NULL;
  if(chkSTR.offsets != NULL) free(chkSTR.offsets);
  chkSTR.offsets = NULL;
  if(chkSTR.offsetsX != NULL) free(chkSTR.offsetsX);
  chkSTR.offsetsX = NULL;
  memset(chkUPRP, 0, sizeof(chkUPRP));
  memset(chkMRGN, 0, sizeof(chkMRGN));
  if(chkTRIG != NULL) free(chkTRIG);
  chkTRIG = NULL;
  if(triglist != NULL) free(triglist);
  triglist = NULL;
  if(trigwrite != NULL) free(trigwrite);
  trigwrite = NULL;
  if(chkMBRF != NULL) free(chkMBRF);
  chkMBRF = NULL;
  memset(chkSPRP, 0, sizeof(chkSPRP));
  memset(&chkFORC, 0, sizeof(FORC));
  memset(&chkUNIS, 0, sizeof(UNIS));
  memset(&chkUPGS, 0, sizeof(UPGS));
  memset(&chkTECS, 0, sizeof(TECS));
  memset(chkCOLR, 0, sizeof(chkCOLR));
  memset(&chkPUPx, 0, sizeof(PUPx));
  memset(&chkPTEx, 0, sizeof(PTEx));
  memset(&chkUNIx, 0, sizeof(UNIx));
  memset(&chkUPGx, 0, sizeof(UPGx));
  memset(&chkTECx, 0, sizeof(TECx));
  memset(&chkCRGB, 0, sizeof(CRGB));
  mapSize = 0;
  mtxmSize = 0;
  maskSize = 0;
  trigCount = 0;
  trigUnique = 0;
  trigWCount = 0;
  trigMax = 0;
  mbrfCount = 0;
  unitCount = 0;
  unitUnique = 0;
  unitWCount = 0;
  thg2Count = 0;
  forcSize = 0;
  memset(loadedSections, 0, sizeof(loadedSections));
  memset(usedUnits, 0, sizeof(usedUnits));
  memset(usedWeapons, 0, sizeof(usedWeapons));
  memset(usedPlayers, 0, sizeof(usedPlayers));
  memset(playerReindex, 0, sizeof(playerReindex));
  if(usedStrings != NULL) free(usedStrings);
  usedStrings = NULL;
  if(stringReindex != NULL) free(stringReindex);
  stringReindex = NULL;
  memset(usedLocations, 0, sizeof(usedLocations));
  memset(locationReindex, 0, sizeof(locationReindex));
  memset(cuwpReindex, 0, sizeof(cuwpReindex));
  usedLocationCount = 0; 
  use255Locations = false;
  randomStartLoc = false;
  useFORCData = false;
}

void detectGameType(){
  loadedGameType = chkVER;
  
  if(optAutoDetect){
    u32 units = 0;
    u32 i;
    for(i = 0; i < unitCount; i++){
      if(chkUNIT[i].player <= PLAYER_8 && chkUNIT[i].unitID != UNIT_START_LOCATION){
        units++;
      }
    }
    
    if(usedLocationCount == 0) procMRGN(true);
    if(units == 0 // No units other than Start Locs and Neutral units
       && usedLocationCount <= 1 // No locations other than Anywhere
       && trigCount <= 3 // No triggers other than possible default triggers
       /*&& mapSTR.count <= 7*/) // Forces, "Anywhere", Map Name, Map Description -- Doesn't work this way, didn't count actual strings (mapSTR.count defaults to 1024)
    { // Probably Melee
      gameType = GAMETYPE_MELEE;
      if(loadedGameType == VER_BROODWAR || loadedGameType == VER_BROODWAR_RM){
        for(i = PLAYER_1; i <= PLAYER_8; i++){
          if(chkCOLR[i] != i){
            // MODE_MELEE_EXTENDED -- Uses custom player colors
            compatability = COMPAT_BROODWAR;
            return;
          }
        }
      }
      // MODE_MELEE_SIMPLE
      compatability = COMPAT_STARCRAFT;
    }else{ // UMS
      gameType = GAMETYPE_UMS;
      switch(loadedGameType){
        case VER_STARCRAFT:
          // MODE_UMS_STARCRAFT
          compatability = COMPAT_STARCRAFT;
          break;
        case VER_HYBRID:
        case VER_HYBRID_RM:
          // MODE_UMS_EXTENDED
          compatability = COMPAT_HYBRID;
          break;
        case VER_BROODWAR:
        case VER_BROODWAR_RM:
          // MODE_UMS_BROODWAR
          compatability = COMPAT_BROODWAR;
          break;
      }
    }
  }
}

unsigned int getOutputType(){
  if(gameType == GAMETYPE_UMS){
    if(usedLocationCount == 0) procMRGN(true); // Count locations
    switch(compatability){
      case COMPAT_STARCRAFT:
        switch(loadedGameType){
          case VER_STARCRAFT:
            return MODE_UMS_SIMPLESC;
          
          case VER_HYBRID:
            if(usedLocationCount < 64){
              return MODE_UMS_SIMPLESC;
            }else{
              return MODE_UMS_STARCRAFT;
            }
          
          case VER_HYBRID_RM:
            return MODE_UMS_EXTENDED | MODE_REMASTERED;
        }
        break;
      
      case COMPAT_HYBRID:
        switch(loadedGameType){
          case VER_STARCRAFT:
            return MODE_UMS_SIMPLESC;
          
          case VER_HYBRID:
            if(usedLocationCount < 64){
              return MODE_UMS_SIMPLEEX;
            }else{
              return MODE_UMS_EXTENDED;
            }
          
          case VER_HYBRID_RM:
            return MODE_UMS_EXTENDED | MODE_REMASTERED;
        }
        break;
      
      case COMPAT_BROODWAR:
        switch(loadedGameType){
          case VER_HYBRID:
          case VER_BROODWAR:
            return MODE_UMS_BROODWAR;
          
          case VER_HYBRID_RM:
          case VER_BROODWAR_RM:
            return MODE_UMS_BROODWAR | MODE_REMASTERED;
        }
    }
  }else{ // GAMETYPE_MELEE
    switch(compatability){
      case COMPAT_STARCRAFT:
      case COMPAT_HYBRID:
        return MODE_MELEE_SIMPLE;
      
      case COMPAT_BROODWAR:
        if(loadedGameType == VER_BROODWAR || loadedGameType == VER_BROODWAR_RM){
          return MODE_MELEE_EXTENDED;
        }
    }
  }
  return 0; // No valid combination available
}

void genDefaultListfile(){
  u32 i;
  u8 wavIDs[chkSTR.count];
  TrigIterator ti;
  ACTION* a;
  
  FILE* f = fopen(defaultListfile, "w");
  if(f == NULL) return;
  fputs(SCENARIO_CHK "\n", f);
  
  memset(wavIDs, 0, chkSTR.count);
  
  initTrigIterator(&ti, TI_TRIGGERS | TI_ALLOW_DISABLED | TI_AUTO_NEXT_TRIG);
  while(a = getNextAction(&ti)){
    if(!(Actions[a->action].fields & ADEF_WAV)) continue;
    if(a->wav != 0 && a->wav < chkSTR.count){
      wavIDs[a->wav-1] = true;
    }
  }
  
  initTrigIterator(&ti, TI_BRIEFINGS | TI_ALLOW_DISABLED | TI_AUTO_NEXT_TRIG);
  while(a = getNextAction(&ti)){
    if(!(BriefingActions[a->action].fields & ADEF_WAV)) continue;
    if(a->wav != 0 && a->wav < chkSTR.count){
      wavIDs[a->wav-1] = true;
    }
  }
  
  for(i = 0; i < chkSTR.count; i++){
    if(wavIDs[i]){
      fputs(chkSTR.data[i], f);
      fputc('\n', f);
    }
  }
  
  fclose(f);
}

void countWAVs(MapFile* mapFiles, u32 mapFileCount){
  u32 i,j;
  s32 wavIDs[chkSTR.count];
  TrigIterator ti;
  ACTION* a;
  
  // get string IDs corresponding to MPQ files
  for(i = 0; i < chkSTR.count; i++){
    wavIDs[i] = -1;
    for(j = 0; j < mapFileCount; j++){
      if(!strcmpi(chkSTR.data[i], mapFiles[j].filename)){
        wavIDs[i] = j;
        break;
      }
    }
  }
  
  // count trigger WAVs
  initTrigIterator(&ti, TI_TRIGGERS | TI_VALIDATE_CONDITIONS | TI_VALIDATE_PLAYERS | TI_AUTO_NEXT_TRIG);
  while(a = getNextAction(&ti)){
    if(!(Actions[a->action].fields & ADEF_WAV)) continue;
    if(a->wav != 0 && a->wav < chkSTR.count && wavIDs[a->wav-1] > -1){
      mapFiles[wavIDs[a->wav-1]].wavUsage.trig++;
    }
  }
  
  // count briefing WAVs
  initTrigIterator(&ti, TI_BRIEFINGS | TI_VALIDATE_CONDITIONS | TI_VALIDATE_PLAYERS | TI_AUTO_NEXT_TRIG);
  while(a = getNextAction(&ti)){
    if(!(BriefingActions[a->action].fields & ADEF_WAV)) continue;
    if(a->wav != 0 && a->wav < chkSTR.count && wavIDs[a->wav-1] > -1){
      mapFiles[wavIDs[a->wav-1]].wavUsage.mbrf++;
    }
  }
}


// Allocates a buffer at least large enough for the compressed CHK
u8* createCHKBuffer(int outputType){
  u32 size;
  int i;
  
  // Base CHK size
  size = sizeof(chkVER) + sizeof(chkVCOD) + sizeof(chkOWNR) + sizeof(chkERA) + sizeof(DIM) + sizeof(chkSIDE) + sizeof(chkSPRP)
       + mtxmSize*sizeof(u16) + unitCount*sizeof(UNIT) + thg2Count*sizeof(THG2) + useFORCData*sizeof(FORC)
       + sizeof(CHKH)*12; // 12 sections headers
  
  // STR data size
  if(loadedSections[SECT_STRx_ID]){
    size += (chkSTR.count+1)*sizeof(u32);
  }else{
    size += (chkSTR.count+1)*sizeof(u16);
  }
  for(i = 0; i < chkSTR.count; i++){
    if(chkSTR.data[i] != NULL){
      size += strlen(chkSTR.data[i]) + 1;
    }
  }
  
  switch(outputType){
    case MODE_MELEE_SIMPLE:
      // Is base CHK
      break;
    
    case MODE_MELEE_EXTENDED:
      // Base CHK + COLR
      size += sizeof(chkCOLR) + sizeof(CHKH);
      break;
    
    case MODE_UMS_STARCRAFT:
      size += (255-64)*sizeof(MRGN); // 255 locations instead of 64 -- add the difference
    case MODE_UMS_SIMPLESC:
      size += sizeof(PUNI) + sizeof(UPGR) + sizeof(PTEC) + sizeof(chkUPRP) + sizeof(UNIS) + sizeof(UPGS) + sizeof(TECS)
            + mapSize + 64*sizeof(MRGN) + trigCount*sizeof(TRIG) + mbrfCount*sizeof(TRIG)
            + sizeof(CHKH)*11;
      break;
    
    case MODE_UMS_EXTENDED:
      size += (255-64)*sizeof(MRGN); // 255 locations instead of 64 -- add the difference
    case MODE_UMS_SIMPLEEX:
      size += sizeof(PUNI) + sizeof(UPGR) + sizeof(PTEC) + sizeof(chkUPRP) + sizeof(UNIS) + sizeof(UPGS) + sizeof(TECS)
            + sizeof(PUPx) + sizeof(PTEx) + sizeof(UNIx) + sizeof(UPGx) + sizeof(TECx)
            + mapSize + 64*sizeof(MRGN) + trigCount*sizeof(TRIG) + mbrfCount*sizeof(TRIG)
            + sizeof(CHKH)*16;
      break;
    
    case MODE_UMS_BROODWAR:
      size += sizeof(PUNI) + sizeof(chkUPRP) + sizeof(chkCOLR) + sizeof(PUPx) + sizeof(PTEx) + sizeof(UNIx) + sizeof(UPGx) + sizeof(TECx)
            + mapSize + 255*sizeof(MRGN) + trigCount*sizeof(TRIG) + mbrfCount*sizeof(TRIG)
            + sizeof(CHKH)*12;
      break;
    
    default: // Everything
      size += sizeof(PUNI) + sizeof(UPGR) + sizeof(PTEC) + sizeof(chkUPRP) + sizeof(UNIS) + sizeof(UPGS) + sizeof(TECS)
            + sizeof(chkCOLR) + sizeof(PUPx) + sizeof(PTEx) + sizeof(UNIx) + sizeof(UPGx) + sizeof(TECx)
            + mapSize + 255*sizeof(MRGN) + trigCount*sizeof(TRIG) + mbrfCount*sizeof(TRIG)
            + sizeof(CHKH)*17;
  }
  
  return malloc(size);
}

u32 createMeleeCHK(u8* buffer, int outputType){
  u32 i,j;
  
  if(outputType == MODE_MELEE_SIMPLE) chkVER = VER_STARCRAFT;
  if(outputType == MODE_MELEE_EXTENDED){
    if(loadedGameType != VER_HYBRID_RM && loadedGameType != VER_BROODWAR_RM){
      chkVER = VER_BROODWAR;
    }else{
      chkVER = VER_BROODWAR_RM;
    }
  }
  
  chkERA &= 7;
  
  for(i = PLAYER_1; i <= PLAYER_8; i++){
    usedPlayers[i] = USEDPLAYER_UNUSED;
    playerReindex[i] = i;
  }
  for(i = 0; i < unitCount; i++){
    if(chkUNIT[i].unitID == UNIT_START_LOCATION && chkUNIT[i].player <= PLAYER_8){
      usedPlayers[chkUNIT[i].player] = chkUNIT[i].player;
    }
  }
  for(i = j = PLAYER_1; i <= PLAYER_8; i++){ // ???? was this originally intended as playerReindex?
    if(usedPlayers[i] != USEDPLAYER_UNUSED){
      usedPlayers[j] = usedPlayers[i];
      j++;
    }
  }
  for(i = PLAYER_1; i <= PLAYER_8; i++){
    chkSIDE[i] = SIDE_USER_SELECTABLE;
    if(usedPlayers[i] == USEDPLAYER_UNUSED){
      chkOWNR[i] = OWNR_INACTIVE;
    }else{
      chkOWNR[i] = OWNR_COMPUTER;
      playerReindex[usedPlayers[i]] = i;
    }
  }
  for(i = PLAYER_9; i <= PLAYER_12; i++){
    chkSIDE[i] = SIDE_USER_SELECTABLE;
    chkOWNR[i] = OWNR_INACTIVE;
  }
  useFORCData = false;
  
  procVCOD();
  procMTXM();
  procUNIT(true);
  procTHG2(true);
  procSTR(true);
  procSPRP();
  
  randomStartLoc = true;
  if(loadedSections[SECT_CRGB_ID]) procCRGB();
  
  // old stuff for VCOD stacking
  /*writeSectionCount = 0;
  addSection(SECT_VER, sizeof(chkVER), &chkVER);
  addSection(SECT_VCOD, sizeof(VCOD), &chkVCOD);
  addSection(SECT_OWNR, sizeof(chkOWNR), chkOWNR);
  addSection(SECT_ERA, sizeof(chkERA), &chkERA);
  addSection(SECT_DIM, sizeof(DIM), &chkDIM);
  addSection(SECT_SIDE, sizeof(chkSIDE), chkSIDE);
  addSection(SECT_MTXM, mtxmSize*sizeof(u16), chkMTXM);
  addSection(SECT_UNIT, unitCount*sizeof(UNIT), chkUNIT);
  addSection(SECT_THG2, thg2Count*sizeof(THG2), chkTHG2);
  addSection(SECT_STR, 0, NULL); // Special case -- not stored in memory in the CHK format
  addSection(SECT_SPRP, sizeof(chkSPRP), chkSPRP);
  addSection(SECT_FORC, 0, NULL); // Section has to exist, but doesn't have to contain anything
  if(outputType == MODE_MELEE_EXTENDED){
    addSection(SECT_COLR, sizeof(chkCOLR), chkCOLR);
  }
  return compileCHK(buffer);*/
  
  u32 pointer = 0;
  writeSection(buffer, &pointer, SECT_VER, sizeof(chkVER), &chkVER);
  writeSection(buffer, &pointer, SECT_VCOD, sizeof(VCOD), &chkVCOD);
  writeSection(buffer, &pointer, SECT_OWNR, sizeof(chkOWNR), chkOWNR);
  writeSection(buffer, &pointer, SECT_ERA, sizeof(chkERA), &chkERA);
  writeSection(buffer, &pointer, SECT_DIM, sizeof(DIM), &chkDIM);
  writeSection(buffer, &pointer, SECT_SIDE, sizeof(chkSIDE), chkSIDE);
  writeSection(buffer, &pointer, SECT_MTXM, mtxmSize*sizeof(u16), chkMTXM);
  writeSection(buffer, &pointer, SECT_UNIT, unitCount*sizeof(UNIT), chkUNIT);
  writeSection(buffer, &pointer, SECT_THG2, thg2Count*sizeof(THG2), chkTHG2);
  writeSTRSection(buffer, &pointer);
  writeSection(buffer, &pointer, SECT_SPRP, sizeof(chkSPRP), chkSPRP);
  writeSection(buffer, &pointer, SECT_FORC, 0, NULL);
  if(outputType == MODE_MELEE_EXTENDED){
    writeSection(buffer, &pointer, SECT_COLR, sizeof(chkCOLR), chkCOLR);
  }
  if(loadedSections[SECT_CRGB_ID]){
    writeSection(buffer, &pointer, SECT_CRGB, sizeof(CRGB), &chkCRGB);
  }
  return pointer;
}

u32 createUMSCHK(u8* buffer, int outputType){
  u32 i,j;
  
  switch(outputType){
    case MODE_UMS_SIMPLESC:
    case MODE_UMS_SIMPLEEX:
      chkVER = VER_STARCRAFT;
      use255Locations = false;
      break;
    
    case MODE_UMS_STARCRAFT:
    case MODE_UMS_EXTENDED:
      if(loadedGameType != VER_HYBRID_RM && loadedGameType != VER_BROODWAR_RM){
        chkVER = VER_HYBRID;
      }else{
        chkVER = VER_HYBRID_RM;
      }
      use255Locations = true;
      break;
    
    case MODE_UMS_BROODWAR:
      if(loadedGameType != VER_HYBRID_RM && loadedGameType != VER_BROODWAR_RM){
        chkVER = VER_BROODWAR;
      }else{
        chkVER = VER_BROODWAR_RM;
      }
      use255Locations = true;
      break;
  }
  useFORCData = true;
 
  chkERA &= 7;
  
  for(i = PLAYER_1; i <= PLAYER_8; i++){
    switch(chkOWNR[i]){
      case OWNR_INACTIVE:
        usedPlayers[i] = USEDPLAYER_UNUSED;
        break;
      case OWNR_COMPUTER:
        usedPlayers[i] = USEDPLAYER_HUMAN;
        break;
      case OWNR_HUMAN:
        usedPlayers[i] = USEDPLAYER_COMPUTER;
        break;
      default:
        usedPlayers[i] = USEDPLAYER_NEUTRAL;
    }
    if(usedPlayers[i] == USEDPLAYER_UNUSED || usedPlayers[i] == USEDPLAYER_NEUTRAL){
      chkSIDE[i] = SIDE_ZERG;
    }
  }
  for(i = PLAYER_9; i <= PLAYER_12; i++){
    chkSIDE[i] = SIDE_ZERG;
    chkOWNR[i] = OWNR_INACTIVE;
  }
  
  randomStartLoc = false;
  
  procMTXM();
  procPUNI(); // UsedUnits Check Enabled
  procUNIT(false); // UsedUnits Check Preplaced
  procTHG2(false); // UsedUnits Check Preplaced
  procMASK();
  procUPRP();
  procFORC();
  if((outputType & (MODE_UMS_SIMPLESC | MODE_UMS_SIMPLEEX | MODE_UMS_STARCRAFT | MODE_UMS_EXTENDED)) != 0){
    procPTEC(); // UsedUnits Check Enabled
  }
  if((outputType & (MODE_UMS_SIMPLEEX | MODE_UMS_BROODWAR | MODE_UMS_EXTENDED)) != 0){
    procPTEx(); // UsedUnits Check Enabled
  }
  procSTR(false); // UsedUnits Check Units used in Triggers & testBuildable
  procMRGN(false);
  procTRIG();
  procMBRF();
  procSPRP();
  procVCOD();
  
  if((outputType & (MODE_UMS_SIMPLESC | MODE_UMS_SIMPLEEX | MODE_UMS_STARCRAFT | MODE_UMS_EXTENDED)) != 0){
    procUPGR();
    //procPTEC();
    procUNIS();
    procUPGS();
    procTECS();
  }
  if((outputType & (MODE_UMS_SIMPLEEX | MODE_UMS_BROODWAR | MODE_UMS_EXTENDED)) != 0){
    procPUPx();
    //procPTEx();
    procUNIx();
    procUPGx();
    procTECx();
  }
  for(i = 0; i < 4; i++){
    chkFORC.strings[i] = stringReindex[chkFORC.strings[i]];
  }
  
  if(loadedSections[SECT_CRGB_ID]) procCRGB();
  
  /* old functions
  writeSectionCount = 0;
  addSection(SECT_VER, sizeof(chkVER), &chkVER);
  addSection(SECT_VCOD, sizeof(VCOD), &chkVCOD);
  addSection(SECT_OWNR, sizeof(chkOWNR), chkOWNR);
  addSection(SECT_ERA, sizeof(chkERA), &chkERA);
  addSection(SECT_DIM, sizeof(DIM), &chkDIM);
  addSection(SECT_SIDE, sizeof(chkSIDE), chkSIDE);
  addSection(SECT_MTXM, mtxmSize*sizeof(u16), chkMTXM);
  addSection(SECT_PUNI, sizeof(PUNI), &chkPUNI);
  if((outputType & (MODE_UMS_SIMPLESC | MODE_UMS_SIMPLEEX | MODE_UMS_STARCRAFT | MODE_UMS_EXTENDED)) != 0){
    addSection(SECT_UPGR, sizeof(UPGR), &chkUPGR);
    addSection(SECT_PTEC, sizeof(PTEC), &chkPTEC);
  }
  addSection(SECT_UNIT, unitCount*sizeof(UNIT), chkUNIT);
  addSection(SECT_THG2, thg2Count*sizeof(THG2), chkTHG2);
  addSection(SECT_MASK, maskSize, chkMASK);
  addSection(SECT_STR, 0, NULL); // Special case -- not stored in memory in the CHK format
  addSection(SECT_UPRP, sizeof(UPRP), chkUPRP);
  if(use255Locations){
    addSection(SECT_MRGN, 255*sizeof(MRGN), chkMRGN);
  }else{
    addSection(SECT_MRGN, 64*sizeof(MRGN), chkMRGN);
  }
  addSection(SECT_TRIG, trigCount*sizeof(TRIG), chkTRIG);
  //addSection(SECT_TRIG, 0, NULL); // Special case -- Section Stacker
  addSection(SECT_MBRF, mbrfCount*sizeof(TRIG), chkMBRF);
  addSection(SECT_SPRP, sizeof(chkSPRP), chkSPRP);
  addSection(SECT_FORC, sizeof(FORC), &chkFORC);
  if((outputType & (MODE_UMS_SIMPLESC | MODE_UMS_SIMPLEEX | MODE_UMS_STARCRAFT | MODE_UMS_EXTENDED)) != 0){
    addSection(SECT_UNIS, sizeof(UNIS), &chkUNIS);
    addSection(SECT_UPGS, sizeof(UPGS), &chkUPGS);
    addSection(SECT_TECS, sizeof(TECS), &chkTECS);
  }
  if(outputType == MODE_UMS_BROODWAR){
    if(randomStartLoc == false){ // Only clear colors when they won't be visible
      for(i = PLAYER_1; i <= PLAYER_8; i++){
        if(usedPlayers[i] == USEDPLAYER_UNUSED){
          chkCOLR[i] = 0;
        }
      }
    }
    addSection(SECT_COLR, sizeof(chkCOLR), chkCOLR);
  }
  if((outputType & (MODE_UMS_SIMPLEEX | MODE_UMS_BROODWAR | MODE_UMS_EXTENDED)) != 0){
    addSection(SECT_PUPx, sizeof(PUPx), &chkPUPx);
    addSection(SECT_PTEx, sizeof(PTEx), &chkPTEx);
    addSection(SECT_UNIx, sizeof(UNIx), &chkUNIx);
    addSection(SECT_UPGx, sizeof(UPGx), &chkUPGx);
    addSection(SECT_TECx, sizeof(TECx), &chkTECx);
  }
  return compileCHK(bufferOut);
  */
  
  u32 pointer = 0;
  
  writeSection(buffer, &pointer, SECT_VER, sizeof(chkVER), &chkVER);
  writeSection(buffer, &pointer, SECT_VCOD, sizeof(VCOD), &chkVCOD);
  writeSection(buffer, &pointer, SECT_OWNR, sizeof(chkOWNR), chkOWNR);
  writeSection(buffer, &pointer, SECT_ERA, sizeof(chkERA), &chkERA);
  writeSection(buffer, &pointer, SECT_DIM, sizeof(DIM), &chkDIM);
  writeSection(buffer, &pointer, SECT_SIDE, sizeof(chkSIDE), chkSIDE);
  writeSection(buffer, &pointer, SECT_MTXM, mtxmSize*sizeof(u16), chkMTXM);
  writeSection(buffer, &pointer, SECT_PUNI, sizeof(PUNI), &chkPUNI);
  if((outputType & (MODE_UMS_SIMPLESC | MODE_UMS_SIMPLEEX | MODE_UMS_STARCRAFT | MODE_UMS_EXTENDED)) != 0){
    writeSection(buffer, &pointer, SECT_UPGR, sizeof(UPGR), &chkUPGR);
    writeSection(buffer, &pointer, SECT_PTEC, sizeof(PTEC), &chkPTEC);
  }
  writeSection(buffer, &pointer, SECT_UNIT, unitCount*sizeof(UNIT), chkUNIT);
  writeSection(buffer, &pointer, SECT_THG2, thg2Count*sizeof(THG2), chkTHG2);
  writeSection(buffer, &pointer, SECT_MASK, maskSize, chkMASK);
  writeSTRSection(buffer, &pointer);
  writeSection(buffer, &pointer, SECT_UPRP, sizeof(chkUPRP), chkUPRP);
  if(use255Locations){
    writeSection(buffer, &pointer, SECT_MRGN, 255*sizeof(MRGN), chkMRGN);
  }else{
    writeSection(buffer, &pointer, SECT_MRGN, 64*sizeof(MRGN), chkMRGN);
  }
  
  //writeSection(buffer, &pointer, SECT_TRIG, trigCount*sizeof(TRIG), chkTRIG);
  writeTRIGSection(buffer, &pointer); // section stacker
  
  writeSection(buffer, &pointer, SECT_MBRF, mbrfCount*sizeof(TRIG), chkMBRF);
  writeSection(buffer, &pointer, SECT_SPRP, sizeof(chkSPRP), chkSPRP);
  writeSection(buffer, &pointer, SECT_FORC, forcSize, &chkFORC);
  if((outputType & (MODE_UMS_SIMPLESC | MODE_UMS_SIMPLEEX | MODE_UMS_STARCRAFT | MODE_UMS_EXTENDED)) != 0){
    writeSection(buffer, &pointer, SECT_UNIS, sizeof(UNIS), &chkUNIS);
    writeSection(buffer, &pointer, SECT_UPGS, sizeof(UPGS), &chkUPGS);
    writeSection(buffer, &pointer, SECT_TECS, sizeof(TECS), &chkTECS);
  }
  if(outputType == MODE_UMS_BROODWAR){
    if(randomStartLoc == false){ // Only clear colors when they won't be visible
      for(i = PLAYER_1; i <= PLAYER_8; i++){
        if(usedPlayers[i] == USEDPLAYER_UNUSED){
          chkCOLR[i] = 0;
        }
      }
    }
    writeSection(buffer, &pointer, SECT_COLR, sizeof(chkCOLR), chkCOLR);
  }
  if(loadedSections[SECT_CRGB_ID]){
    writeSection(buffer, &pointer, SECT_CRGB, sizeof(CRGB), &chkCRGB);
  }
  if((outputType & (MODE_UMS_SIMPLEEX | MODE_UMS_BROODWAR | MODE_UMS_EXTENDED)) != 0){
    writeSection(buffer, &pointer, SECT_PUPx, sizeof(PUPx), &chkPUPx);
    writeSection(buffer, &pointer, SECT_PTEx, sizeof(PTEx), &chkPTEx);
    writeSection(buffer, &pointer, SECT_UNIx, sizeof(UNIx), &chkUNIx);
    writeSection(buffer, &pointer, SECT_UPGx, sizeof(UPGx), &chkUPGx);
    writeSection(buffer, &pointer, SECT_TECx, sizeof(TECx), &chkTECx);
  }
  return pointer;
}

/*void addSection(u32 name, u32 size, void* data){
  writeSectionData[writeSectionCount].header.name = name;
  writeSectionData[writeSectionCount].header.size = size;
  writeSectionData[writeSectionCount].data = data;
  writeSectionData[writeSectionCount].id = 0; // Was used for VCOD stacking, but it didn't actually lower the map size -- I may try again at some point
  writeSectionData[writeSectionCount].used = 0; // VCOD stacking
  writeSectionCount++;
}*/

/*u32 compileCHK(u8* buffer){
  u32 i,j,ptr;
  
  // Get size of CHK and allocate buffer
  u8* chk = NULL;
  u32 chkSize = 0;
  for(i = 0; i < writeSectionCount; i++){
    switch(writeSectionData[i].header.name){
      case SECT_STR:
        chkSize += (chkSTR.count+1)*sizeof(u16) + sizeof(CHKH);
        for(j=0; j<chkSTR.count; j++){
          chkSize += strlen(chkSTR.data[j]);
        }
        break;
      case SECT_TRIG:
        chkSize += sizeof(CHKH);
        for(j=0; j<trigWCount; j++){
          if(trigwrite[j].isStacked){
            chkSize += trigwrite[j].blocks * sizeof(TRIG);
          }else{
            chkSize += trigwrite[j].count * sizeof(TRIG);
          }
        }
        break;
      default:
        chkSize += writeSectionData[i].header.size + sizeof(CHKH);
    }
  }
  chk = malloc(chkSize);
  
  // Copy sections to buffer
  ptr = 0;
  for(i = 0; i < writeSectionCount; i++){
    switch(writeSectionData[i].header.name){
      case SECT_STR:
        writeSTRSection(chk, &ptr);
        break;
      case SECT_TRIG:
        writeTRIGSection(chk, &ptr);
        break;
      default:
        writeSection(chk, &ptr, &writeSectionData[i].header, writeSectionData[i].data);
    }
  }
  
  return 0;
}*/


void writeSection(u8* buffer, u32* pointer, u32 name, u32 size, void* data){
  CHKH chkh = {name, size};
  memcpy(buffer + *pointer, &chkh, sizeof(CHKH));
  if(data != NULL && (s32)(size) > 0){
    memcpy(buffer + *pointer + sizeof(CHKH), data, size);
  }
  *pointer += size + sizeof(CHKH);
}

void writeSTRSection(u8* buffer, u32* pointer){
  u32 i, dataSize;
  CHKH chkh;
  
  if(loadedSections[SECT_STRx_ID]){
    chkh.name = SECT_STRx;
    dataSize = sizeof(u32);
  }else{
    chkh.name = SECT_STR;
    dataSize = sizeof(u16);
  }
  
  if(chkSTR.count > 0){
    chkh.size = (chkSTR.count + 1) * dataSize;
    for(i = 0; i < chkSTR.count; i++){
      chkh.size += strlen(chkSTR.data[i]) + 1;
    }
    memcpy(buffer + *pointer, &chkh, sizeof(CHKH));
    *pointer += sizeof(CHKH);
    memcpy(buffer + *pointer, &(chkSTR.count), dataSize);
    *pointer += dataSize;
    if(loadedSections[SECT_STRx_ID]){
      memcpy(buffer + *pointer, chkSTR.offsetsX, chkSTR.count * dataSize);
    }else{
      memcpy(buffer + *pointer, chkSTR.offsets, chkSTR.count * dataSize);
    }
    *pointer += chkSTR.count * dataSize;
    for(i = 0; i < chkSTR.count; i++){
      strcpy(buffer + *pointer, chkSTR.data[i]);
      *pointer += strlen(chkSTR.data[i]) + 1;
    }
  }else{
    // No strings -- minimum size
    chkh.size = 1;
    writeSection(buffer, pointer, chkh.name, chkh.size, &chkSTR);
  }
}

// Some Section Stacker magic
void writeTRIGSection(u8* buffer, u32* pointer){
  CHKH chkh = {SECT_TRIG, 0};
  u32 i,j,count,jumplbl,total;
  char c,s[3];
  
  CONDITION tmpcond;
  TRIG* tmptrgs = malloc(trigMax * sizeof(TRIG));
  
  if(trigwrite[0].isStacked){
    chkh.size = trigwrite[0].chunkSize;
  }else{
    chkh.size = trigwrite[0].count;
    if(trigwrite[1].isStacked){
      chkh.size += trigwrite[1].chunkSize;
    }
  }
  chkh.size *= sizeof(TRIG);
  
  memcpy(buffer+*pointer, &chkh, sizeof(CHKH));
  *pointer += sizeof(CHKH);
  
  for(i = 0; i < trigWCount; i++){
    if(trigwrite[i].isStacked){
      
      for(count = 0; count < trigwrite[i].blocks; count++){
        memcpy(&tmptrgs[count], &chkTRIG[triglist[trigwrite[i].startid]], sizeof(TRIG));
      }
      count = 0;
      total = trigwrite[i].count;
      do{
        total -= trigwrite[i].chunkSize;
        jumplbl = SECT_JUMP;
        switch(tmptrgs[count+trigwrite[i].chunkSize].conditions[0].condition){
          case COND_COMMAND:
          case COND_BRING:
          case COND_ACCUMULATE:
          case COND_KILL:
          case COND_OPPONENTS:
          case COND_DEATHS:
          case COND_SCORE:
            //puts("Invalid first condition ...");
            // Reorder !
            s[0] = 0;
            s[1] = 0;
            s[2] = 0;
            for(j = 1; j < 16; j++){
              c = tmptrgs[count+trigwrite[i].chunkSize].conditions[j].condition;
              if(c == COND_NO_CONDITION && !s[1]){ // End of condition list
                s[0] = j;
                j = 16; // Didn't find first-order matches
                break; // No more conditions to check
              }
              if(c == COND_COUNTDOWN_TIMER || c == COND_SWITCH || c == COND_ELAPSED_TIME) break;
              if(!s[1] && ( (c >= COND_MOST_KILLS && c <= COND_MOST_RESOURCES) || (c >= COND_LEAST_KILLS && c <= COND_LEAST_RESOURCES))) s[1] = j; // Most/Least Kills/Resources, Highest/Lowest Score
              if(!s[2] && (c == COND_COMMAND_THE_MOST_AT || c == COND_COMMAND_THE_LEAST_AT || c == COND_MOST_KILLS || c == COND_LEAST_KILLS)) s[2] = j;
            }
            if(j == 16 && s[1]) j = s[1]; // Found second-order match?
            if(j == 16 && s[2]) j = s[2]; // Found third-order match?
            if(j < 16){ // Found a swap partner, yay!
              memcpy(&tmpcond, &tmptrgs[count+trigwrite[i].chunkSize].conditions[j], sizeof(CONDITION));
              memcpy(&tmptrgs[count+trigwrite[i].chunkSize].conditions[j], &tmptrgs[count+trigwrite[i].chunkSize].conditions[0], sizeof(CONDITION));
              memcpy(&tmptrgs[count+trigwrite[i].chunkSize].conditions[0], &tmpcond, sizeof(CONDITION));
            }else{ // No swap partner. :(
              if(s[0]){ // An empty slot exists!
                for(j = s[0]; j > 0; j--){ // Insert condition at beginning of list !
                  memcpy(&tmptrgs[count+trigwrite[i].chunkSize].conditions[j], &tmptrgs[count+trigwrite[i].chunkSize].conditions[j-1], sizeof(CONDITION));
                }
                memset(&tmptrgs[count+trigwrite[i].chunkSize].conditions[0], 0, sizeof(CONDITION)); // Empty new slot
                tmptrgs[count+trigwrite[i].chunkSize].conditions[0].condition = COND_ALWAYS;
              }else{ // Can't stack ! It simply cannot be done !
                // What do I doooooooo
                puts("Invalid first condition ...");
              }
            }
            c = tmptrgs[count+trigwrite[i].chunkSize].conditions[j].condition;
            if(c == COND_COMMAND_THE_MOST_AT || c == COND_COMMAND_THE_LEAST_AT) tmptrgs[count+trigwrite[i].chunkSize].conditions[0].location = SECT_JUMP;
          case COND_COMMAND_THE_MOST_AT: // Command the Most At
          case COND_COMMAND_THE_LEAST_AT: // Command the Least At
            jumplbl = tmptrgs[count+trigwrite[i].chunkSize].conditions[0].location;
          default:
            if(total > trigwrite[i].chunkSize){
              chkh.size = trigwrite[i].chunkSize * sizeof(TRIG);
              memcpy(&tmptrgs[count].players[20], &chkh, sizeof(CHKH));
              tmptrgs[count+trigwrite[i].chunkSize].conditions[0].location = jumplbl;
              tmptrgs[count+trigwrite[i].chunkSize].conditions[0].player = -((trigwrite[i].chunkSize - 1) * sizeof(TRIG) + 16);
            }else{
              chkh.size = total;
              if(trigwrite[i+1].isStacked){
                chkh.size += trigwrite[i+1].chunkSize;
              }else{
                chkh.size += trigwrite[i+1].count;
                if(trigwrite[i+2].isStacked){
                  chkh.size += trigwrite[i+2].chunkSize;
                }
              }
              chkh.size *= sizeof(TRIG);
              memcpy(&tmptrgs[count+trigwrite[i].chunkSize-total].players[20], &chkh, sizeof(CHKH));
              tmptrgs[count+trigwrite[i].chunkSize].conditions[0].location = jumplbl;
              tmptrgs[count+trigwrite[i].chunkSize].conditions[0].player = -((total - 1) * sizeof(TRIG) + 16);
            }
            //if(jumplbl != SECT_JUMP){
            //  tmptrgs[count+trigwrite[i].chunkSize].conditions[0].location = jumplbl;
            //}
        }
        count++;
      } while(total > trigwrite[i].chunkSize);
      memcpy(buffer+*pointer, tmptrgs, trigwrite[i].blocks * sizeof(TRIG));
      *pointer += trigwrite[i].blocks * sizeof(TRIG);
      trigMax += trigwrite[i].blocks;
    }else{
      for(count = trigwrite[i].startid; count < trigwrite[i].startid + trigwrite[i].count; count++){
        memcpy(buffer+*pointer, &chkTRIG[triglist[count]], sizeof(TRIG));
        *pointer += sizeof(TRIG);
        trigMax++;
      }
    }
  }
  free(tmptrgs);
}

// true = same
// false = different
bool trigcmp(TRIG* a, TRIG* b){
  u32 i;
  for(i = 0; i < 16; i++){
    if(a->conditions[i].condition != b->conditions[i].condition) return false;
    if(a->conditions[i].condition == COND_NO_CONDITION) break;
    if(a->conditions[i].location != b->conditions[i].location) return false;
    if(a->conditions[i].number != b->conditions[i].number) return false;
    if(a->conditions[i].unit != b->conditions[i].unit) return false;
    if(a->conditions[i].modifier != b->conditions[i].modifier) return false;
    if(a->conditions[i].type != b->conditions[i].type) return false;
    if(a->conditions[i].flags != b->conditions[i].flags) return false;
  }
  for(i = 0; i < 64; i++){
    if(a->actions[i].action != b->actions[i].action) return false;
    if(a->actions[i].action == ACT_NO_ACTION) break;
    if(a->actions[i].location != b->actions[i].location) return false;
    if(a->actions[i].text != b->actions[i].text) return false;
    if(a->actions[i].wav != b->actions[i].wav) return false;
    if(a->actions[i].time != b->actions[i].time) return false;
    if(a->actions[i].player != b->actions[i].player) return false;
    if(a->actions[i].dest != b->actions[i].dest) return false;
    if(a->actions[i].type != b->actions[i].type) return false;
    if(a->actions[i].modifier != b->actions[i].modifier) return false;
    if(a->actions[i].flags != b->actions[i].flags) return false;
  }
  if(a->flags != b->flags) return false;
  for(i = 0; i < 28; i++){
    if(a->players[i] != b->players[i]) return false;
  }
  return true;
}

// count -- Total number of items read by SC
// blocks -- Number of items written to CHK
// chunkSize -- Maximum number of items read at a time
void calcChunking(int count, int *blocks, int *chunkSize){
  int totalBlocks, b,c;
  for(b = 1; b <= count; b++){
    for(c = count; c >= 1; c--){
      totalBlocks = (b - c + 1) * c;
      if(count > (totalBlocks - c) && count <= totalBlocks){
        *blocks = b;
        *chunkSize = c;
        return;
      }
    }
  }
  *blocks = count;
  *chunkSize = 1;
  return;
}

// initializes iterator struct
void initTrigIterator(TrigIterator* ti, u32 flags){
  if(flags & TI_BRIEFINGS){
    ti->section = chkMBRF;
    ti->maxIndex = mbrfCount;
  }else{
    ti->section = chkTRIG;
    ti->maxIndex = trigCount;
  }
  ti->flags = flags & ~TI__HAS_VALID_TRIG;
  ti->trigIndex = 0;
}

// advances iterator to the next valid trigger block. returns true if one exists or false if there are no more triggers
bool getNextTrig(TrigIterator* ti){
  u32 i,j;
  bool used;
  
  if(ti->flags & TI__HAS_VALID_TRIG){
    // validate next ID
    i = ti->trigIndex + 1;
  }else{
    // validate current ID
    i = ti->trigIndex;
  }
  ti->flags &= ~TI__HAS_VALID_TRIG;
  
  for( ; i < ti->maxIndex; i++){
    if(ti->flags & TI_VALIDATE_CONDITIONS){
      if(ti->flags & TI_BRIEFINGS){
        // only used if first condition is Mission Briefing
        if(ti->section[i].conditions[0].condition != COND_MISSION_BRIEFING) continue;
      }else{
        used = true; // used unless "never"
        for(j = 0; j < 16; j++){
          if(ti->section[i].conditions[j].condition == COND_NO_CONDITION) break; // stop looking
          if((ti->section[i].conditions[j].condition == COND_NEVER || ti->section[i].conditions[j].condition == COND_MISSION_BRIEFING) && (ti->section[i].conditions[j].flags & TRIG_DISABLED) != 0){
            used = false;
            break;
          }
        }
        if(!used) continue;
      }
    }
    if(ti->flags & TI_VALIDATE_ACTIONS){
      used = false; // not used unless a useful action exists
      for(j = 0; j < 64; j++){
        if(ti->section[i].actions[j].action == ACT_NO_ACTION) break; // stop looking
        if((ti->section[i].actions[j].flags & TRIG_DISABLED) && !(ti->flags & TI_ALLOW_DISABLED)) continue;
        if(ti->flags & TI_BRIEFINGS){
          if(ti->section[i].actions[j].action >= BACT_COUNT) continue;
          if(delSPActions && (BriefingActions[ti->section[i].actions[j].action].fields & ADEF_SP_ACT)) continue;
        }else{
          if(ti->section[i].actions[j].action >= ACT_COUNT) continue;
          if(Actions[ti->section[i].actions[j].action].fields & ADEF_VALID_IGNORE) continue;
          if(delSPActions && (Actions[ti->section[i].actions[j].action].fields & ADEF_SP_ACT)) continue;
        }
        used = true;
        break;
      }
      if(!used) continue;
    }
    if(ti->flags & TI_VALIDATE_PLAYERS){
      used = false;
      for(j = PLAYER_1; j <= PLAYER_FORCE_4; j++){
        if(j == PLAYER_9) j = PLAYER_ALL_PLAYERS; // skip unused players
        // check used players?
        if(ti->section[i].players[j] != 0){
          used = true;
          break;
        }
      }
      if(!used) continue;
    }
    
    // all validation checks passed
    ti->trigIndex = i;
    ti->condIndex = 0;
    ti->actIndex = 0;
    ti->flags |= TI__HAS_VALID_TRIG;
    return true;
  }
  ti->trigIndex = i;
  return false;
}

// returns pointer next valid condition or NULL if none exist
CONDITION* getNextCondition(TrigIterator* ti){
  u32 i;
  // return if there are no conditions
  if(!(ti->flags & TI__HAS_VALID_TRIG) || ti->condIndex == 16) return NULL;
  
  for(i = ti->condIndex; i < 16; i++){
    if(ti->section[ti->trigIndex].conditions[i].condition == COND_NO_CONDITION) break;
    if((ti->section[ti->trigIndex].conditions[i].flags & TRIG_DISABLED) && !(ti->flags & TI_ALLOW_DISABLED)) continue;
    if(ti->section[ti->trigIndex].conditions[i].condition >= COND_COUNT) continue;
    if(Conditions[ti->section[ti->trigIndex].conditions[i].condition].fields & ADEF_IGNORE) continue;
    ti->condIndex = i+1;
    return &(ti->section[ti->trigIndex].conditions[i]);
  }
  ti->condIndex = 16;
  return NULL;
}

// returns pointer to next valid action or NULL if none exist. Internally advances to next trigger block if TI_AUTO_NEXT_TRIG is set (for iterating over all actions without regard to ownership)
ACTION* getNextAction(TrigIterator* ti){
  u32 i;
  do {
    // return if there are no actions, or find next trig if enabled
    if(!(ti->flags & TI__HAS_VALID_TRIG) || ti->actIndex == 64){
      if(ti->flags & TI_AUTO_NEXT_TRIG){
        if(getNextTrig(ti) == false){
          return NULL;
        }
      }else{
        return NULL;
      }
    }
    
    for(i = ti->actIndex; i < 64; i++){
      if(ti->section[ti->trigIndex].actions[i].action == ACT_NO_ACTION) break;
      if((ti->section[ti->trigIndex].actions[i].flags & TRIG_DISABLED) && !(ti->flags & TI_ALLOW_DISABLED)) continue;
      if(ti->flags & TI_BRIEFINGS){
        if(ti->section[ti->trigIndex].actions[i].action >= BACT_COUNT) continue;
        if(BriefingActions[ti->section[ti->trigIndex].actions[i].action].fields & ADEF_PROC_IGNORE) continue;
        if(delSPActions && (BriefingActions[ti->section[ti->trigIndex].actions[i].action].fields & ADEF_SP_ACT)) continue;
      }else{
        if(ti->section[ti->trigIndex].actions[i].action >= ACT_COUNT) continue;
        if(Actions[ti->section[ti->trigIndex].actions[i].action].fields & ADEF_PROC_IGNORE) continue;
        if(delSPActions && (Actions[ti->section[ti->trigIndex].actions[i].action].fields & ADEF_SP_ACT)) continue;
      }
      ti->actIndex = i+1;
      return &(ti->section[ti->trigIndex].actions[i]);
    }
    ti->actIndex = 64;
  } while(ti->flags & TI_AUTO_NEXT_TRIG); // keep looping if enabled
  return NULL;
}



// ~~~ Whee ! ~~~ //


void procVCOD(){
  u32 i,j;
  bool clear = false;
  
  // Clear unused seed values
  for(i = 0; i < 256; i++){
    if(i > 0) clear = true;
    for(j = PLAYER_1; j <= PLAYER_12; j++){
      if(i == chkOWNR[j]) clear = false;
      if(i == chkSIDE[j]) clear = false;
      if(useFORCData && j <= PLAYER_8 && i == chkFORC.players[j]) clear = false;
    }
    if(clear) chkVCOD.seeds[i] = 0;
  }
  
  // Replace synonymous opcodes
  for(i = 0; i < 16; i++){
    if(chkVCOD.opcodes[i] >= 3 && chkVCOD.opcodes[i] <= 5){
      chkVCOD.opcodes[i] = 3;
    }
  }
}

void procMTXM(){
  u32 i;
  
  // Trim bottom row, if enabled
  if(trimMTXM && mtxmSize > (mapSize - chkDIM.w - 5)){
    mtxmSize = mapSize - chkDIM.w - 5;
  }
  
  // Reindex tiles based on tileData tables
  // broken in remaster
  /*for(i = 0; i < mtxmSize; i++){
    if(chkMTXM[i] < tileDataSize[chkERA]){
      chkMTXM[i] = tileData[chkERA][chkMTXM[i]];
    }
  }*/
  
  // Remove trailing null tiles
  while(chkMTXM[mtxmSize-1] == 0) mtxmSize--;
}

void procPUNI(){
  u32 i,j,index;
  
  for(i = 0; i < 228; i++){
    index = i; // logical unit ID
    // ranges of unused, hero, and otherwise special units -- allows for far, far fewer switch cases
    if(index >= UNIT_CIVILIAN          && index <= UNIT_TANK_SIEGE_T ||
       index >= UNIT_INFESTED_KERRIGAN && index <= UNIT_YGGDRASIL ||
       index >= UNIT_DARK_TEMPLAR_HERO && index <= UNIT_GANTRITHOR ||
       index >= UNIT_DANIMOTH          && index <= UNIT_DISRUPTION_WEB ||
       index >= UNIT_NORAD_II_CRASHED  && index <= UNIT_INFESTED_COMMAND_CENTER ||
       index >= UNIT_MATURE_CHRYSALIS  && index <= UNIT_UNUSED_ZERG_2 ||
       index >= UNIT_KHAYDARIN_FORMATION){
      index = UNIT_GOLIATH_TURR;
    }
    switch(index){
      case UNIT_GOLIATH_TURR:
      case UNIT_TANK_TURR:
      case UNIT_MONTAG:
      case UNIT_SPIDER_MINE:
      case UNIT_SCANNER_SWEEP:
      case UNIT_LARVA:
      case UNIT_EGG:
      case UNIT_BROODLING:
      case UNIT_TORRASQUE:
      case UNIT_MATRIARCH:
      case UNIT_COCOON:
      case UNIT_DARK_ARCHON:
      case UNIT_ARCHON:
      case UNIT_STARBASE:
      case UNIT_REPAIR_BAY:
      case UNIT_UNUSED_ZERG_1:
      case UNIT_OVERMIND_SHELL:
      case UNIT_OVERMIND:
      case UNIT_UNUSED_PROTOSS_1:
      case UNIT_UNUSED_PROTOSS_2:
      case UNIT_STASIS_CELL:
        // Unit cannot be built, so zero everything
        chkPUNI.global[i] = 0;
        for(j = PLAYER_1; j <= PLAYER_12; j++){
          chkPUNI.available[j][i] = 0;
          chkPUNI.useGlobal[j][i] = 0;
        }
        break;
      default:
        // Unit can be built, so check
        if(chkPUNI.global[i] != 0){
          chkPUNI.global[i] = 1;
          usedUnits[i] |= USEDUNIT_ENABLED;
        }
        for(j = PLAYER_1; j <= PLAYER_8; j++){
          if(usedPlayers[j] == USEDPLAYER_UNUSED){
            // Zero unused players
            chkPUNI.available[j][i] = 0;
            chkPUNI.useGlobal[j][i] = 0;
          }else{
            if(chkPUNI.useGlobal[j][i] != 0){
              chkPUNI.available[j][i] = 0;
              chkPUNI.useGlobal[j][i] = 1;
            }else{
              if(chkPUNI.available[j][i]){
                // Should unit availability be checked player-by-player?
                usedUnits[i] |= USEDUNIT_ENABLED;
              }
              if(chkPUNI.available[j][i] == chkPUNI.global[i]){
                chkPUNI.available[j][i] = 0;
                chkPUNI.useGlobal[j][i] = 1;
              }
            }
          }
        }
        // Zero P9-12
        for( ; j <= PLAYER_12; j++){
          chkPUNI.available[j][i] = 0;
          chkPUNI.useGlobal[j][i] = 0;
        }
        break;
    }
  }
}

void procUPGR(){
  u32 i,j;
  for(i = 0; i < 46; i++){
    for(j = PLAYER_1; j <= PLAYER_12; j++){
      if(i == UPGR_BURST_LASER|| i == UPGR_UNUSED_45 || usedPlayers[j] == USEDPLAYER_UNUSED){
        chkUPGR.playerMax[j][i] = 0;
        chkUPGR.playerLevel[j][i] = 0;
        chkUPGR.useGlobal[j][i] = 0;
      }else{
        if(chkUPGR.useGlobal[j][i] != 0 || (chkUPGR.playerMax[j][i] == chkUPGR.globalMax[i] && chkUPGR.playerLevel[j][i] == chkUPGR.globalLevel[i])){
          chkUPGR.playerMax[j][i] = 0;
          chkUPGR.playerLevel[j][i] = 0;
          chkUPGR.useGlobal[j][i] = 1;
        }else{
          if(chkUPGR.playerLevel[j][i] >= chkUPGR.playerMax[j][i]){
            chkUPGR.playerMax[j][i] = 0;
          }
        }
      }
    }
    if(i == UPGR_BURST_LASER || i == UPGR_UNUSED_45){
      chkUPGR.globalLevel[i] = 0;
      chkUPGR.globalMax[i] = 0;
    }else{
      if(chkUPGR.globalLevel[i] >= chkUPGR.globalMax[i]){
        chkUPGR.globalMax[i] = 0;
      }
    }
  }
}

void procPUPx(){
  u32 i,j;
  for(i = 0; i < 61; i++){
    switch(i){
      case UPGR_BURST_LASER:
      case UPGR_UNUSED_45:
      case UPGR_UNUSED_46:
      case UPGR_UNUSED_48:
      case UPGR_UNUSED_50:
      case UPGR_UNUSED_55:
      case UPGR_UNUSED_56:
      case UPGR_UNUSED_57:
      case UPGR_UNUSED_58:
      case UPGR_UNUSED_59:
        for(j = PLAYER_1; j <= PLAYER_12; j++){
          chkPUPx.playerMax[j][i] = 0;
          chkPUPx.playerLevel[j][i] = 0;
          chkPUPx.useGlobal[j][i] = 0;
        }
        chkPUPx.globalLevel[i] = 0;
        chkPUPx.globalMax[i] = 0;
        break;
      case UPGR_UPGRADE_60:
        for(j = PLAYER_1; j <= PLAYER_12; j++){
          chkPUPx.playerMax[j][i] = 0;
          if(j <= PLAYER_8 && usedPlayers[j] == USEDPLAYER_UNUSED){
            chkPUPx.playerLevel[j][i] = 0;
            chkPUPx.useGlobal[j][i] = 0;
          }else{
            if(chkPUPx.useGlobal[j][i] != 0 || chkPUPx.playerLevel[j][i] == chkPUPx.globalLevel[i]){
              chkPUPx.playerLevel[j][i] = 0;
              chkPUPx.useGlobal[j][i] = 1;
            }
          }
        }
        chkPUPx.globalMax[UPGR_UPGRADE_60] = 0;
        break;
      default:
        for(j = PLAYER_1; j <= PLAYER_12; j++){
          if(j <= PLAYER_8 && usedPlayers[j] == USEDPLAYER_UNUSED){
            chkPUPx.playerMax[j][i] = 0;
            chkPUPx.playerLevel[j][i] = 0;
            chkPUPx.useGlobal[j][i] = 0;
          }else{
            if(chkPUPx.useGlobal[j][i] != 0 || (chkPUPx.playerMax[j][i] == chkPUPx.globalMax[i] && chkPUPx.playerLevel[j][i] == chkPUPx.globalLevel[i])){
              chkPUPx.playerMax[j][i] = 0;
              chkPUPx.playerLevel[j][i] = 0;
              chkPUPx.useGlobal[j][i] = 1;
            }else{
              if(chkPUPx.playerLevel[j][i] >= chkPUPx.playerMax[j][i]){
                chkPUPx.playerMax[j][i] = 0;
              }
            }
          }
        }
        if(chkPUPx.globalLevel[i] >= chkPUPx.globalMax[i]){
          chkPUPx.globalMax[i] = 0;
        }
    }
  }
}

void procPTEC(){
  u32 i,j;
  bool enabled;
  for(i = 0; i <  24; i++){
    enabled = false;
    if(chkPTEC.globalEnabled[i] > 1) chkPTEC.globalEnabled[i] = 1;
    if(chkPTEC.globalResearched[i] > 1) chkPTEC.globalResearched[i] = 1;
    for(j = PLAYER_1; j <= PLAYER_12; j++){
      if(j <= PLAYER_8 && usedPlayers[j] == USEDPLAYER_UNUSED){
        chkPTEC.playerEnabled[j][i] = 0;
        chkPTEC.playerResearched[j][i] = 0;
        chkPTEC.useGlobal[j][i] = 0;
      }else{
        if(chkPTEC.playerEnabled[j][i] > 1) chkPTEC.playerEnabled[j][i] = 1;
        if(chkPTEC.playerResearched[j][i] > 1) chkPTEC.playerResearched[j][i] = 1;
        if(chkPTEC.useGlobal[j][i] != 0 || (chkPTEC.playerEnabled[j][i] == chkPTEC.globalEnabled[i] && chkPTEC.playerResearched[j][i] == chkPTEC.globalResearched[i])){
          chkPTEC.playerEnabled[j][i] = 0;
          chkPTEC.playerResearched[j][i] = 0;
          chkPTEC.useGlobal[j][i] = 1;
        }
        if(chkPTEC.playerEnabled[j][i] > 1) enabled = true;
      }
    }
    if(chkPTEC.globalEnabled[i] > 1) enabled = true;
    if(enabled){
      switch(i){
        case TECH_SPIDER_MINES:
          usedUnits[UNIT_SPIDER_MINE] |= USEDUNIT_ENABLED;
          break;
        case TECH_TANK_SIEGE_MODE:
          usedUnits[UNIT_TANK_SIEGE] |= USEDUNIT_ENABLED;
          usedUnits[UNIT_DUKE_SIEGE] |= USEDUNIT_ENABLED;
          break;
        case TECH_INFESTATION:
          usedUnits[UNIT_INFESTED_COMMAND_CENTER] |= USEDUNIT_ENABLED;
          break;
        case TECH_SPAWN_BROODLINGS:
          usedUnits[UNIT_BROODLING] |= USEDUNIT_ENABLED;
          break;
        case TECH_ARCHON_WARP:
          usedUnits[UNIT_ARCHON] |= USEDUNIT_ENABLED;
          break;
      }
    }
  }
}

void procPTEx(){
  u32 i,j;
  bool enabled;
  for(i = 0; i < 44; i++){
    enabled = false;
    if(i == TECH_UNUSED_26 || i == TECH_UNUSED_33 || i >= TECH_UNUSED_35){
      chkPTEx.globalEnabled[i] = 0;
      chkPTEx.globalResearched[i] = 0;
      for(j = PLAYER_1; j <= PLAYER_12; j++){
        chkPTEx.playerEnabled[j][i] = 0;
        chkPTEx.playerResearched[j][i] = 0;
        chkPTEx.useGlobal[j][i] = 0;
      }
    }else{
      if(chkPTEx.globalEnabled[i] > 1) chkPTEx.globalEnabled[i] = 1;
      if(chkPTEx.globalResearched[i] > 1) chkPTEx.globalResearched[i] = 1;
      for(j = PLAYER_1; j <= PLAYER_12; j++){
        if(j <= PLAYER_8 && usedPlayers[j] == USEDPLAYER_UNUSED){
          chkPTEx.playerEnabled[j][i] = 0;
          chkPTEx.playerResearched[j][i] = 0;
          chkPTEx.useGlobal[j][i] = 0;
        }else{
          if(chkPTEx.playerEnabled[j][i] > 1) chkPTEx.playerEnabled[j][i] = 1;
          if(chkPTEx.playerResearched[j][i] > 1) chkPTEx.playerResearched[j][i] = 1;
          if(chkPTEx.useGlobal[j][i] != 0 || (chkPTEx.playerEnabled[j][i] == chkPTEx.globalEnabled[i] && chkPTEx.playerResearched[j][i] == chkPTEx.globalResearched[i])){
            chkPTEx.playerEnabled[j][i] = 0;
            chkPTEx.playerResearched[j][i] = 0;
            chkPTEx.useGlobal[j][i] = 1;
          }
          if(chkPTEx.playerEnabled[j][i] > 1) enabled = true;
        }
      }
      if(chkPTEx.globalEnabled[i] > 1) enabled = true;
      if(enabled){
        switch(i){
          case TECH_SPIDER_MINES:
            usedUnits[UNIT_SPIDER_MINE] |= USEDUNIT_ENABLED;
            break;
          case TECH_TANK_SIEGE_MODE:
            usedUnits[UNIT_TANK_SIEGE] |= USEDUNIT_ENABLED;
            usedUnits[UNIT_DUKE_SIEGE] |= USEDUNIT_ENABLED;
            break;
          case TECH_INFESTATION:
            usedUnits[UNIT_INFESTED_COMMAND_CENTER] |= USEDUNIT_ENABLED;
            break;
          case TECH_SPAWN_BROODLINGS:
            usedUnits[UNIT_BROODLING] |= USEDUNIT_ENABLED;
            break;
          case TECH_ARCHON_WARP:
            usedUnits[UNIT_ARCHON] |= USEDUNIT_ENABLED;
            break;
          case TECH_DARK_ARCHON_MELD:
            usedUnits[UNIT_DARK_ARCHON] |= USEDUNIT_ENABLED;
            break;
          case TECH_LURKER_ASPECT:
            usedUnits[UNIT_LURKER] |= USEDUNIT_ENABLED;
            break;
        }
      }
    }
  }
}

void procUNIT(bool melee){
  u32 i, newid = 0, propMask, connections = 0;
  for(i = 0; i < unitCount; i++){
    if(melee){ // Melee mode -- only allow P12 neutral units and start locations
      if(chkUNIT[i].player <= PLAYER_8){
        chkUNIT[i].player = playerReindex[chkUNIT[i].player];
      }
      switch(chkUNIT[i].unitID){
        case UNIT_RHYNADON:
        case UNIT_BENGALAAS:
        case UNIT_SCANTID:
        case UNIT_KAKARU:
        case UNIT_RAGNASAUR:
        case UNIT_URSADON:
          if(chkUNIT[i].player != PLAYER_12) break;
          chkUNIT[newid].classInstance = 0;
          chkUNIT[newid].x = chkUNIT[i].x;
          chkUNIT[newid].y = chkUNIT[i].y;
          chkUNIT[newid].unitID = chkUNIT[i].unitID;
          chkUNIT[newid].bldgRelation = 0;
          chkUNIT[newid].stateFlags = chkUNIT[i].stateFlags & (UNIT_PROP_HALLUCINATED | UNIT_PROP_INVINCIBLE);
          chkUNIT[newid].validFlags = chkUNIT[i].validFlags & chkUNIT[newid].stateFlags;
          chkUNIT[newid].player = PLAYER_12;
          if(chkUNIT[newid].hp == 100){
            chkUNIT[newid].hp = 0;
          }else{
            chkUNIT[newid].validFlags |= UNIT_PROP_HP;
          }
          chkUNIT[newid].shields = 0;
          chkUNIT[newid].energy = 0;
          chkUNIT[newid].resources = 0;
          chkUNIT[newid].hangar = 0;
          chkUNIT[newid].unused = 0;
          chkUNIT[newid].relatedBldg = 0;
          newid++;
          break;
        case UNIT_MINERAL_FIELD_1:
        case UNIT_MINERAL_FIELD_2:
        case UNIT_MINERAL_FIELD_3:
        case UNIT_VESPENE_GEYSER:
          if(chkUNIT[i].player != PLAYER_12) break;
          chkUNIT[newid].classInstance = 0;
          chkUNIT[newid].x = chkUNIT[i].x;
          chkUNIT[newid].y = chkUNIT[i].y;
          chkUNIT[newid].unitID = chkUNIT[i].unitID;
          chkUNIT[newid].bldgRelation = 0;
          chkUNIT[newid].stateFlags = chkUNIT[i].stateFlags & UNIT_PROP_HALLUCINATED;
          chkUNIT[newid].validFlags = chkUNIT[i].validFlags & chkUNIT[newid].stateFlags;
          chkUNIT[newid].player = PLAYER_12;
          if(chkUNIT[newid].hp == 100){
            chkUNIT[newid].hp = 0;
          }else{
            chkUNIT[newid].validFlags |= UNIT_PROP_HP;
          }
          chkUNIT[newid].shields = 0;
          chkUNIT[newid].energy = 0;
          if(chkUNIT[i].resources > 0){
            chkUNIT[newid].validFlags |= UNIT_PROP_RESOURCES;
          }
          chkUNIT[newid].hangar = 0;
          chkUNIT[newid].unused = 0;
          chkUNIT[newid].relatedBldg = 0;
          newid++;
          break;
        case UNIT_START_LOCATION:
          if(chkUNIT[i].player > PLAYER_8) break;
          chkUNIT[newid].classInstance = 0;
          chkUNIT[newid].x = chkUNIT[i].x;
          chkUNIT[newid].y = chkUNIT[i].y;
          chkUNIT[newid].unitID = UNIT_START_LOCATION;
          chkUNIT[newid].bldgRelation = 0;
          chkUNIT[newid].stateFlags = 0;
          chkUNIT[newid].validFlags = 0;
          chkUNIT[newid].hp = 0;
          chkUNIT[newid].shields = 0;
          chkUNIT[newid].energy = 0;
          chkUNIT[newid].resources = 0;
          chkUNIT[newid].hangar = 0;
          chkUNIT[newid].unused = 0;
          chkUNIT[newid].relatedBldg = 0;
          newid++;
          break;
      }
      unitCount = newid;
    }else{ // Use Map Settings mode
/*
TO DO:
Remove all duplicate Start Locations. -- Keep last one (store coords, add them to the end?)
Remove Start Locations for players beyond P8
Null unit "serial" and other misc information if it is not connected with anything(addon, nydus). -- or connection is not valid
 Addons 0x0200 106+107, 106+108, 113+120, 114+115, 116+117, 116+118
 Nydus  0x0400 134+134
*/
      propMask = UNIT_PROP_HALLUCINATED;
      if(chkUNIT[i].unitID < 228){ // Valid unit -- for others, who knows.
        usedUnits[chkUNIT[i].unitID] |= USEDUNIT_PREPLACED;
        
        // Check if building is attached to anything
        if((chkUNIT[i].bldgRelation & UNIT_REL_NYDUS) != 0){
          if(chkUNIT[i].unitID != UNIT_NYDUS_CANAL) chkUNIT[i].bldgRelation = 0;
        }else if((chkUNIT[i].bldgRelation & UNIT_REL_ADDON) != 0){
          if(chkUNIT[i].unitID <= UNIT_DISRUPTION_WEB ||  // Not a building
            (chkUNIT[i].unitID >= UNIT_SUPPLY_DEPOT && chkUNIT[i].unitID <= UNIT_ACADEMY) || // Not an addon or addon building
            (chkUNIT[i].unitID >= UNIT_STARBASE && chkUNIT[i].unitID != UNIT_MACHINE_SHOP)){ // Not an addon or addon building
            chkUNIT[i].bldgRelation = 0;
          }
        }else{
          chkUNIT[i].bldgRelation = 0;
        }
        if(chkUNIT[i].bldgRelation == 0){
          chkUNIT[i].classInstance = 0;
          chkUNIT[i].relatedBldg = 0;
        }
        
        // Get valid properties for the unit type
        if((unitsdat.abilFlags[chkUNIT[i].unitID] & UNITSDAT_ABIL_CLOAKABLE) != 0){
          propMask |= UNIT_PROP_CLOAK;
        }
        if((unitsdat.abilFlags[chkUNIT[i].unitID] & UNITSDAT_ABIL_BURROWABLE) != 0){
          propMask |= UNIT_PROP_BURROW;
        }
        if((unitsdat.abilFlags[chkUNIT[i].unitID] & UNITSDAT_ABIL_FLYINGBUILDING) != 0){
          propMask |= UNIT_PROP_TRANSIT;
        }
        if((unitsdat.abilFlags[chkUNIT[i].unitID] & UNITSDAT_ABIL_INVINCIBLE) == 0){
          propMask |= UNIT_PROP_INVINCIBLE;
        }
        if(chkUNIT[i].hp != 100){
          propMask |= UNIT_PROP_HP;
        }
        if(chkUNIT[i].shields != 100 && unitsdat.hasShields[chkUNIT[i].unitID] != 0){
          propMask |= UNIT_PROP_SHIELDS;
        }
        if(/*chkUNIT[i].energy != 100 && */(unitsdat.abilFlags[chkUNIT[i].unitID] & UNITSDAT_ABIL_SPELLCASTER) != 0){ // Setting to 0% makes unit start at 50 energy instead of 100% -- thanks MOOSE.
          propMask |= UNIT_PROP_ENERGY;
        }
        if(chkUNIT[i].resources != 0 && (unitsdat.abilFlags[chkUNIT[i].unitID] & UNITSDAT_ABIL_RESOURCECONTAINER) != 0){
          propMask |= UNIT_PROP_RESOURCES;
        }
        if(chkUNIT[i].hangar != 0 && chkUNIT[i].unitID == UNIT_CARRIER || (chkUNIT[i].unitID >= UNIT_WARBRINGER && chkUNIT[i].unitID <= UNIT_REAVER)){
          propMask |= UNIT_PROP_HANGAR;
        }
        
        // Clear any unused properties -- for HP and things that are used, clearing will set to default (100%)
        if(!(propMask & UNIT_PROP_HP)) chkUNIT[i].hp = 0;
        if(!(propMask & UNIT_PROP_SHIELDS)) chkUNIT[i].shields = 0;
        if(!(propMask & UNIT_PROP_ENERGY)) chkUNIT[i].energy = 0;
        if(!(propMask & UNIT_PROP_RESOURCES)) chkUNIT[i].resources = 0;
        if(!(propMask & UNIT_PROP_HANGAR)) chkUNIT[i].hangar = 0;
        
        chkUNIT[i].stateFlags &= propMask; // Only allow used & valid flags
        propMask &= 0xFFFF0000 | chkUNIT[i].stateFlags; // Get only flags that are used ...
        chkUNIT[i].validFlags &= propMask; // ... and only mark those as valid
        chkUNIT[i].unused = 0;
      }
    }
  }
}

void procTHG2(bool melee){
  u32 i, newid = 0;
  for(i = 0; i < thg2Count; i++){
    if(melee){ // Melee mode -- Only allow pure sprites and P12 unit sprites
      if((chkTHG2[i].flags & THG2_FLAG_SPRITE) == THG2_FLAG_SPRITE){
        chkTHG2[newid].id = chkTHG2[i].id;
        chkTHG2[newid].x = chkTHG2[i].x;
        chkTHG2[newid].y = chkTHG2[i].y;
        chkTHG2[newid].player = 0;
        chkTHG2[newid].flags = THG2_FLAG_SPRITE;
        newid++;
      }else{
        if(chkTHG2[i].player == PLAYER_12){
          chkTHG2[newid].id = chkTHG2[i].id;
          chkTHG2[newid].x = chkTHG2[i].x;
          chkTHG2[newid].y = chkTHG2[i].y;
          chkTHG2[newid].player = PLAYER_12;
          chkTHG2[newid].flags &= THG2_FLAG_DISABLED;
          newid++;
        }
      }
      thg2Count = newid;
    }else{ // UMS mode
      if((chkTHG2[i].flags & THG2_FLAG_SPRITE) == THG2_FLAG_SPRITE){
        if(chkTHG2[i].id < SPRITE_SCOURGE || (chkTHG2[i].id > SPRITE_EGG_SPAWN && chkTHG2[i].id < SPRITE_LURKER_EGG)){ // doodad or effect sprite -- no player colors
          chkTHG2[i].player = 0;
        }
        chkTHG2[i].flags = THG2_FLAG_SPRITE;
      }else{
        if(chkTHG2[i].id < 228){ // is a valid unit ID
          usedUnits[chkTHG2[i].id] |= USEDUNIT_PREPLACED;
        }
        chkTHG2[i].player &= 0xFF; // zero high byte -- player is u8
        chkTHG2[i].flags &= THG2_FLAG_DISABLED;
      }
    }
  }
}

void procMASK(){
  int i;
  u8 mask = 0;
  
  // Generate fog mask for non-human players
  // Count down, lshifting each time -- bit7 = player 8, bit0 = player 1
  for(i = PLAYER_8; i >= PLAYER_1; i--){
    if(chkOWNR[i] != OWNR_HUMAN){
      mask |= 1;
    }
    mask <<= 1;
  }
  
  // Apply mask to every tile...
  for(i = 0; i < mapSize; i++){
    chkMASK[i] |= mask;
  }
  // ... and remove any that are now unused
  for(maskSize = mapSize; maskSize > 0 && chkMASK[maskSize-1] == 0xFF; maskSize--);
}

void procSTR(bool melee){
  u32 i,j;
  u32 newSTRCount = 0;
  STR newSTR;
  TrigIterator ti;
  ACTION* a;
  
  memset(usedStrings, 0, chkSTR.count + 1);
  memset(stringReindex, 0, (chkSTR.count + 1) * sizeof(u32));
  
  if(melee){
    if(delGameName){
      chkSPRP[0] = 0;
    }else{
      usedStrings[chkSPRP[0]] = 1;
    }
    usedStrings[chkSPRP[1]] = 1;
  }else{
    //
    initTrigIterator(&ti, TI_TRIGGERS | TI_VALIDATE_ALL | TI_AUTO_NEXT_TRIG);
    while(a = getNextAction(&ti)){
      if(Actions[a->action].fields & ADEF_TEXT){
        usedStrings[a->text] |= 2;
      }
      if(Actions[a->action].fields & ADEF_WAV){
        usedStrings[a->wav] |= 2;
      }
      if(a->action == ACT_CREATE_UNITS || a->action == ACT_CREATE_UNITS_WITH_PROPERTIES){
        if(a->type < 228){
          usedUnits[a->type] |= USEDUNIT_PREPLACED;
        }
      }
    }
    
    // Check used units - in str??
    testBuildable();
    
    //
    initTrigIterator(&ti, TI_BRIEFINGS | TI_VALIDATE_ALL | TI_AUTO_NEXT_TRIG);
    while(a = getNextAction(&ti)){
      if(BriefingActions[a->action].fields & ADEF_TEXT){
        usedStrings[a->text] |= 4;
      }
      if(BriefingActions[a->action].fields & ADEF_WAV){
        usedStrings[a->wav] |= 4;
      }
    }
    //
    if(delGameName){
      chkSPRP[0] = 0;
    }else{
      usedStrings[chkSPRP[0]] |= 1;
    }
    usedStrings[chkSPRP[1]] |= 1;
    for(i = 0; i < 4; i++) usedStrings[chkFORC.strings[i]] |= 1;
    for(i = 0; i < 228; i++){
     if(chkUNIS.useDefaults[i] == 0 && usedUnits[i]) usedStrings[chkUNIS.string[i]] |= 8;
     if(chkUNIx.useDefaults[i] == 0 && usedUnits[i]) usedStrings[chkUNIx.string[i]] |= 8;
    }
  }
  
  // strings are 1-based with 0 being no string
  for(i = 0; i < chkSTR.count; i++){
    if(usedStrings[i+1] != 0 && stringReindex[i+1] == 0){
      newSTRCount++;
      stringReindex[i+1] = newSTRCount;
      for(j = i+1; j < chkSTR.count; j++){
        if(chkSTR.data[j] != NULL && (strcmp(chkSTR.data[i], chkSTR.data[j]) == 0)) stringReindex[j+1] = newSTRCount;
      }
    }
  }
  
  newSTR.count = newSTRCount;
  if(loadedSections[SECT_STRx_ID]){
    newSTR.offsetsX = malloc(newSTRCount*sizeof(u32));
    newSTR.offsets = NULL;
  }else{
    newSTR.offsets = malloc(newSTRCount*sizeof(u16));
    newSTR.offsetsX = NULL;
  }
  newSTR.data = malloc(newSTRCount*sizeof(u32*));
  newSTRCount++;
  if(loadedSections[SECT_STRx_ID]){
    newSTRCount *= sizeof(u32);
  }else{
    newSTRCount *= sizeof(u16);
  }
  
  // allocate new string table
  for(i = 0; i < newSTR.count; i++){
    for(j = i; j < chkSTR.count; j++){
      if(stringReindex[j+1] == i+1){
        if(loadedSections[SECT_STRx_ID]){
          newSTR.offsetsX[i] = newSTRCount;
        }else{
          newSTR.offsets[i] = newSTRCount;
        }
        newSTR.data[i] = malloc(strlen(chkSTR.data[j])+1);
        strcpy(newSTR.data[i], chkSTR.data[j]);
        newSTRCount += strlen(newSTR.data[i])+1;
        break;
      }
    }
  }
  
  // free old table
  for(i = 0; i < chkSTR.count; i++) free(chkSTR.data[i]);
  free(chkSTR.data);
  if(chkSTR.offsets != NULL) free(chkSTR.offsets);
  if(chkSTR.offsetsX != NULL) free(chkSTR.offsetsX);
  
  // assign STR new data
  chkSTR.count = newSTR.count;
  chkSTR.offsets = newSTR.offsets;
  chkSTR.offsetsX = newSTR.offsetsX;
  chkSTR.data = newSTR.data;
}

void procUPRP(){
  u32 i,newid;
  // UNFINISHED, APPARENTLY.
  for(i = 0; i < 64; i++){
    chkUPRP[i].validFlags &= 0x003F001F;
    chkUPRP[i].player = 0;
    chkUPRP[i].stateFlags &= 0x001F;
    chkUPRP[i].unused = 0;
  }
}

void procMRGN(bool count){
  u32 i,j,newid;
  if(count){ // why is this in the proc function ?????
    TrigIterator ti;
    CONDITION* c;
    ACTION* a;
    initTrigIterator(&ti, TI_TRIGGERS); // no validation ??
    while(getNextTrig(&ti)){
      while(c = getNextCondition(&ti)){
        if(Conditions[c->condition].fields & CDEF_LOCATION){
          if(c->location && c->location < 256){
            usedLocations[c->location] = true;
          }
        }
      }
      while(a = getNextAction(&ti)){
        if(Actions[a->action].fields & ADEF_LOCATION){
          if(a->location && a->location < 256){
            usedLocations[a->location] = true;
          }
        }
        if(Actions[a->action].fields & ADEF_DEST_LOC){
          if(a->dest && a->dest < 256){
            usedLocations[a->dest] = true;
          }
        }
      }
    }
    usedLocationCount = 0;
    for(i = 1; i < 256; i++){
     if(usedLocations[i]) usedLocationCount++;
    }
  }else{ // Compress section
    newid = 0;
    for(i = 0; i < 255; i++){
      if(usedLocations[i+1]){
        chkMRGN[newid].x1 = chkMRGN[i].x1;
        chkMRGN[newid].y1 = chkMRGN[i].y1;
        chkMRGN[newid].x2 = chkMRGN[i].x2;
        chkMRGN[newid].y2 = chkMRGN[i].y2;
        chkMRGN[newid].string = 0;
        chkMRGN[newid].flags = chkMRGN[i].flags & MRGN_FLAG_MASK;
        newid++;
        locationReindex[i+1] = newid;
      }
    }
    // Zero the rest
    for( ; newid < 255; newid++){
      chkMRGN[newid].x1 = 0;
      chkMRGN[newid].y1 = 0;
      chkMRGN[newid].x2 = 0;
      chkMRGN[newid].y2 = 0;
      chkMRGN[newid].string = 0;
      chkMRGN[newid].flags = 0;
    }
  }
}

void procTRIG(){
/*
==>"TRIG", "MBRF"
Remove all triggers that will never execute. Including triggers with an AtLeast value greater than an AtMost value.
Remove similar conditions, like AtLeast 0.
Change AtLeast/AtMost conditions of the same value to Exactly.
Check LeaderBoard text strings. If the string is the same as the default string, make it 0x00.
Change "Current Player" to "Player 1" for all of Player 1(ONLY player 1)'s triggers.
Null out trigger execution for unused and invalid players and groups.
Reference all WAV files from current strings rather than their own individual file names. For example; Play WAV "Force 1", would play the wav file called "Force 1".
*/
  u32 i,j,newj, trigFlags;
  CONDITION newCondition;
  ACTION newAction;
  TrigIterator ti;
  CONDITION* c;
  ACTION* a;
  
  if(triglist != NULL) free(triglist);
  triglist = malloc(trigCount * sizeof(u32));
  trigUnique = 0;
  
  u32 newid = 0;
  u32 newCount = 0;
  initTrigIterator(&ti, TI_TRIGGERS | TI_VALIDATE_ALL);
  while(getNextTrig(&ti)){
    newj = 0;
    while(c = getNextCondition(&ti)){
      memset(&newCondition, 0, sizeof(CONDITION));
      newCondition.condition = c->condition;
      if(Conditions[c->condition].fields & CDEF_LOCATION){
        newCondition.location = locationReindex[c->location];
      }
      if(Conditions[c->condition].fields & CDEF_PLAYER){
        newCondition.player = c->player;
      }
      if(Conditions[c->condition].fields & CDEF_NUMBER){
        newCondition.number = c->number;
      }
      if(Conditions[c->condition].fields & CDEF_UNIT){
        newCondition.unit = c->unit;
      }
      if(Conditions[c->condition].fields & CDEF_MODIFIER){
        newCondition.modifier = c->modifier;
      }
      if(Conditions[c->condition].fields & CDEF_TYPE){
        newCondition.type = c->type;
      }
      if(Conditions[c->condition].fields & CDEF_FLAGS){
        newCondition.flags = c->flags & Conditions[c->condition].flagMask;
      }
      if(Conditions[c->condition].fields & CDEF_UNUSED){
        if(c->unused == TRIG_MASKED_EUD){
          newCondition.unused = TRIG_MASKED_EUD;
        }else{
          newCondition.flags = 0;
          newCondition.unused = 0;
        }
      }
      memcpy(&chkTRIG[newid].conditions[newj], &newCondition, sizeof(CONDITION));
      newj++;
    }
    // clear remaining condition slots
    if(newj < 16){
      memset(&chkTRIG[newid].conditions[newj], 0, (16 - newj) * sizeof(CONDITION));
    }
    
    newj = 0;
    trigFlags = chkTRIG[ti.trigIndex].flags & TRIG_USED_FLAGS_MASK;
    while(a = getNextAction(&ti)){
      if(a->action == ACT_PRESERVE_TRIGGER){
        trigFlags |= TRIG_FLAG_PRESERVE;
        continue; // No need to write this action
      }
      memset(&newAction, 0, sizeof(ACTION));
      newAction.action = a->action;
      if(Actions[a->action].fields & ADEF_LOCATION){
        newAction.location = locationReindex[a->location];
      }
      if(Actions[a->action].fields & ADEF_TEXT){
        newAction.text = stringReindex[a->text];
      }
      if(Actions[a->action].fields & ADEF_WAV){
        newAction.wav = stringReindex[a->wav];
      }
      if(Actions[a->action].fields & ADEF_TIME){
        newAction.time = a->time;
      }
      if(Actions[a->action].fields & ADEF_PLAYER){
        newAction.player = a->player;
      }
      if(Actions[a->action].fields & ADEF_DEST){
        newAction.dest = a->dest;
      }
      if(Actions[a->action].fields & ADEF_DEST_LOC){
        newAction.dest = locationReindex[a->dest];
      }
      if(Actions[a->action].fields & ADEF_TYPE){
        newAction.type = a->type;
      }
      if(Actions[a->action].fields & ADEF_MODIFIER){
        newAction.modifier = a->modifier;
      }
      if(Actions[a->action].fields & ADEF_FLAGS){
        newAction.flags = a->flags & Actions[a->action].flagMask;
      }
      if(Actions[a->action].fields & ADEF_UNUSED){
        if(a->unused2 == TRIG_MASKED_EUD){
          newAction.unused2 = TRIG_MASKED_EUD;
        }else{
          newAction.flags = 0;
          newAction.unused2 = 0;
        }
      }
      if(newAction.action == ACT_CREATE_UNITS){
        newAction.action = ACT_CREATE_UNITS_WITH_PROPERTIES;
      }
      if(newAction.action == ACT_CREATE_UNITS_WITH_PROPERTIES){
        if(newAction.modifier == 1) newAction.modifier = 0; // create 0 == create 1
      }
      memcpy(&chkTRIG[newid].actions[newj], &newAction, sizeof(ACTION));
      newj++;
    }
    // clear remaining action slots
    if(newj < 64){
      memset(&chkTRIG[newid].actions[newj], 0, (64 - newj) * sizeof(ACTION));
    }
    
    chkTRIG[newid].flags = trigFlags;
    for(j = PLAYER_1; j <= PLAYER_ID_COUNT; j++){
      if(j <= PLAYER_8 || (j >= PLAYER_ALL_PLAYERS && j <= PLAYER_FORCE_4)){
        if(chkTRIG[ti.trigIndex].players[j] != 0){
          chkTRIG[newid].players[j] = 1;
        }else{
          chkTRIG[newid].players[j] = 0;
        }
      }else{
        chkTRIG[newid].players[j] = 0;
      }
    }
    
    // SECTION STACKER CMP
    for(j = 0; j < newid; j++){
      if(trigcmp(&chkTRIG[newid], &chkTRIG[j])){
        triglist[newCount] = j;
        break;
      }
    }
    if(j == newid){
      triglist[newCount] = newid;
      newid++;
    }
    
    newCount++;
  }
  
  trigCount = newCount;
  trigUnique = newid;
  
  trigWCount = 0;
  trigwrite = calloc(trigCount, sizeof(writeentry));
  
  int count, blocks, chsize, lastid, total, last;
  //TRIG* tmptrgs = NULL;
  //writelist = calloc(trigcount, sizeof(writeentry));
  lastid = 0;
  last = -1;
  count = 0;
  trigMax = 0;
  
  for(i = 0; i < trigCount; i++){
    if(last != triglist[i]){
      if(last != -1){
        if(count > 1){
          calcChunking(count, &blocks, &chsize);
          if(blocks < count){
            trigWCount++;
            trigwrite[trigWCount].isStacked = true;
            trigwrite[trigWCount].startid = lastid;
            trigwrite[trigWCount].count = count;
            trigwrite[trigWCount].blocks = blocks;
            trigwrite[trigWCount].chunkSize = chsize;
            if(blocks > trigMax) trigMax = blocks;
            trigWCount++;
            trigwrite[trigWCount].startid = i;
          }else{
            trigwrite[trigWCount].count += count;
          }
        }else{
          trigwrite[trigWCount].count++;
        }
      }
      count = 1;
      lastid = i;
      last = triglist[i];
    }else{
      count++;
    }
  }
  
  if(count > 1){
    calcChunking(count, &blocks, &chsize);
    if(blocks < count){
      trigWCount++;
      trigwrite[trigWCount].isStacked = true;
      trigwrite[trigWCount].startid = i;
      trigwrite[trigWCount].count = count;
      trigwrite[trigWCount].blocks = blocks;
      trigwrite[trigWCount].chunkSize = chsize;
      if(blocks > trigMax) trigMax = blocks;
      trigWCount++;
      trigwrite[trigWCount].startid = i;
    }else{
      trigwrite[trigWCount].count += count;
    }
  }else{
    trigwrite[trigWCount].count++;
  }
  trigWCount++;
}

void procMBRF(){
  u32 i;
  ACTION newAction;
  TrigIterator ti;
  ACTION* a;
  
  u32 newid = 0;
  initTrigIterator(&ti, TI_BRIEFINGS | TI_VALIDATE_ALL);
  while(getNextTrig(&ti)){
    memset(&chkMBRF[newid].conditions, 0, sizeof(CONDITION)*16);
    chkMBRF[newid].conditions[0].condition = COND_MISSION_BRIEFING;
    
    i = 0;
    while(a = getNextAction(&ti)){
      memset(&newAction, 0, sizeof(ACTION));
      newAction.action = a->action;
      if(BriefingActions[a->action].fields & ADEF_TEXT){
        newAction.text = stringReindex[a->text];
      }
      if(BriefingActions[a->action].fields & ADEF_WAV){
        newAction.wav = stringReindex[a->wav];
      }
      if(BriefingActions[a->action].fields & ADEF_TIME){
        newAction.time = a->time;
      }
      if(BriefingActions[a->action].fields & ADEF_PLAYER){
        newAction.player = a->player;
      }
      if(BriefingActions[a->action].fields & ADEF_TYPE){
        newAction.type = a->type;
      }
      if(BriefingActions[a->action].fields & ADEF_MODIFIER){
        newAction.modifier = a->modifier;
      }
      memcpy(&chkMBRF[newid].actions[i], &newAction, sizeof(ACTION));
      i++;
    }
    // clear remaining action slots
    if(i < 64){
      memset(&chkMBRF[newid].actions[i], 0, (64 - i) * sizeof(ACTION));
    }
    
    chkMBRF[newid].flags = 0;
    for(i = PLAYER_1; i <= PLAYER_ID_COUNT; i++){
      if(i <= PLAYER_8 || (i >= PLAYER_ALL_PLAYERS && i <= PLAYER_FORCE_4)){
        if(chkMBRF[ti.trigIndex].players[i] != 0){
          chkMBRF[newid].players[i] = 1;
        }else{
          chkMBRF[newid].players[i] = 0;
        }
      }else{
        chkMBRF[newid].players[i] = 0;
      }
    }
    newid++;
  }
  mbrfCount = newid;
}

void procSPRP(){
  chkSPRP[0] = stringReindex[chkSPRP[0]];
  chkSPRP[1] = stringReindex[chkSPRP[1]];
}

void procFORC(){
  unsigned char forcUsed[4] = {0,0,0,0};
  char forcName[] = "Force 0";
  u32 i;
  for(i = PLAYER_1; i <= PLAYER_8; i++){
    if(chkFORC.players[i] < 4){ // player in a valid force
      switch(usedPlayers[i]){
        case USEDPLAYER_HUMAN:
          forcUsed[chkFORC.players[i]] |= FORCUSED_HUMAN;
          break;
        case USEDPLAYER_COMPUTER:
          forcUsed[chkFORC.players[i]] |= FORCUSED_COMPUTER;
          break;
        default:
          forcUsed[chkFORC.players[i]] |= FORCUSED_OTHER;
          break;
      }
    }else{
      chkFORC.players[i] = 5; // Set invalid forces to 5 -- reduces VCOD seed values since 5 is used elsewhere
    }
  }
  
  for(i = 0; i < 4; i++){
    if(!(forcUsed[i] & (FORCUSED_HUMAN | FORCUSED_COMPUTER))){ // Not human or computer
      chkFORC.strings[i] = 0; // Force is not visible
    }
    if(forcUsed[i] != 0 && (chkFORC.flags[i] & FORC_RANDOM_START_LOCATIONS)){
      // Can't remove extra player colors
      randomStartLoc = true;
    }
    if(chkFORC.strings[i] > 0){
      forcName[6] = '1' + i; // "Force %d"
      if(strcmp(forcName, chkSTR.data[chkFORC.strings[i]-1]) == 0){
        // Use default string
        chkFORC.strings[i] = 0;
      }
    }
    if(!(forcUsed[i] & FORCUSED_HUMAN)){ // No humans -- computers auto-ally
      chkFORC.flags[i] &= FORC_RANDOM_START_LOCATIONS;
    }else{
      chkFORC.flags[i] &= FORC_FLAGS_MASK;
      if(!(chkFORC.flags[i] & FORC_ALLIES)){
        // Can't allied vic if not allied
        chkFORC.flags[i] &= ~FORC_ALLIED_VICTORY;
      }
    }
  }
  
  // trim trailing 0's
  u8* rawFORC = (u8*)(&chkFORC);
  for(forcSize = sizeof(FORC); forcSize > 0 && rawFORC[forcSize-1] == 0; forcSize--);
}

void procUNIS(){
/*
Null all bonuses if their respective upgrade will always be 0.
*/
  u32 i,j;
  bool used;
  for(i = 0; i < 228; i++){
    switch(i){
      case UNIT_GOLIATH_TURR:
      case UNIT_TANK_TURR:
      case UNIT_SCHEZAR_TURR:
      case UNIT_DUKE_TANK_TURR:
      case UNIT_DUKE_SIEGE_TURR:
      case UNIT_TANK_SIEGE_T:
      case UNIT_SCANNER_SWEEP:
      case UNIT_DISRUPTION_WEB:
      case UNIT_STARBASE:
      case UNIT_REPAIR_BAY:
      case UNIT_UNUSED_ZERG_1:
      case UNIT_UNUSED_ZERG_2:
      case UNIT_UNUSED_PROTOSS_1:
      case UNIT_UNUSED_PROTOSS_2:
      case UNIT_CAVE:
      case UNIT_CAVE_IN:
      case UNIT_CANTINA:
      case UNIT_MINING_PLATFORM:
      case UNIT_INDEPENDENT_COMMAND_CENTER:
      case UNIT_INDEPENDENT_JUMP_GATE:
      case UNIT_RUINS:
      case UNIT_UNUSED_CRYSTAL_FORMATION:
      case UNIT_ZERG_MARKER:
      case UNIT_TERRAN_MARKER:
      case UNIT_PROTOSS_MARKER:
      case UNIT_DARK_SWARM:
        chkUNIS.useDefaults[i] = 1;
        break;
      case UNIT_NUCLEAR_MISSILE:
      case UNIT_SCARAB:
        chkUNIS.hp[i] = 0;
        chkUNIS.shields[i] = 0;
        break;
    }
    if(usedUnits[i] == 0) chkUNIS.useDefaults[i] = 1;
    if(chkUNIS.useDefaults[i] != 0){
      chkUNIS.useDefaults[i] = 1;
      chkUNIS.hp[i] = 0;
      chkUNIS.shields[i] = 0;
      chkUNIS.armor[i] = 0;
      chkUNIS.time[i] = 0;
      chkUNIS.minerals[i] = 0;
      chkUNIS.gas[i] = 0;
      chkUNIS.string[i] = 0;
    }else{
      if(unitsdat.hasShields[i] == 0) chkUNIS.shields[i] = 0;
      if(unitsdat.abilFlags[i] & UNITSDAT_ABIL_INVINCIBLE) chkUNIS.armor[i] = 0;
      
      // Zero costs of units that cannot be built or repaired
      if((!(unitsdat.availFlags[i] & UNITSDAT_AVAIL_PLAYERSETTINGS) && // Does not have settings
         (!(unitsdat.abilFlags[i] & UNITSDAT_ABIL_MECHANICAL) || !(unitsdat.groupFlags[i] & UNITSDAT_GROUP_TERRAN))) || // Cannot be repaired
         i == UNIT_REPAIR_BAY){ // Because it can be repaired
        switch(i){
          case UNIT_DARK_ARCHON:
          case UNIT_ARCHON:
            // Uses time
            chkUNIS.minerals[i] = 0;
            chkUNIS.gas[i] = 0;
          case UNIT_NUCLEAR_MISSILE:
          case UNIT_INTERCEPTOR:
          case UNIT_SCARAB:
            // Uses all 3 stats
            break;
          default:
            chkUNIS.minerals[i] = 0;
            chkUNIS.gas[i] = 0;
            chkUNIS.time[i] = 0;
        }
      }
      chkUNIS.string[i] = stringReindex[chkUNIS.string[i]];
    }
  }
  for(i = 0; i < 100; i++){
    used = false;
    // Find that a unit that uses this weapon
    for(j = 0; j < 228; j++){
      if(chkUNIS.useDefaults[j] == 0){
        if( (unitsdat.gndWeapon[j] == i || unitsdat.airWeapon[j] == i) || // Unit has weapon
            unitsdat.subunit1[j] != 228 && (unitsdat.gndWeapon[unitsdat.subunit1[j]] == i || unitsdat.airWeapon[unitsdat.subunit1[j]] == i) ){ // Subunit has weapon
          used = true;
          break;
        }
      }
    }
    // Isn't used, zero it
    if(!used){
      chkUNIS.damage[i] = 0;
      chkUNIS.bonus[i] = 0;
    }
  }
}

void procUNIx(){
/*
Null all bonuses if their respective upgrade will always be 0.
*/
  u32 i,j;
  bool used;
  for(i = 0; i < 228; i++){
    switch(i){
      case UNIT_GOLIATH_TURR:
      case UNIT_TANK_TURR:
      case UNIT_SCHEZAR_TURR:
      case UNIT_DUKE_TANK_TURR:
      case UNIT_DUKE_SIEGE_TURR:
      case UNIT_TANK_SIEGE_T:
      case UNIT_SCANNER_SWEEP:
      case UNIT_DISRUPTION_WEB:
      case UNIT_STARBASE:
      case UNIT_REPAIR_BAY:
      case UNIT_UNUSED_ZERG_1:
      case UNIT_UNUSED_ZERG_2:
      case UNIT_UNUSED_PROTOSS_1:
      case UNIT_UNUSED_PROTOSS_2:
      case UNIT_CAVE:
      case UNIT_CAVE_IN:
      case UNIT_CANTINA:
      case UNIT_MINING_PLATFORM:
      case UNIT_INDEPENDENT_COMMAND_CENTER:
      case UNIT_INDEPENDENT_JUMP_GATE:
      case UNIT_RUINS:
      case UNIT_UNUSED_CRYSTAL_FORMATION:
      case UNIT_ZERG_MARKER:
      case UNIT_TERRAN_MARKER:
      case UNIT_PROTOSS_MARKER:
      case UNIT_DARK_SWARM:
        chkUNIx.useDefaults[i] = 1;
        break;
      case UNIT_NUCLEAR_MISSILE:
      case UNIT_SCARAB:
        chkUNIx.hp[i] = 0;
        chkUNIx.shields[i] = 0;
        break;
    }
    if(usedUnits[i] == 0) chkUNIx.useDefaults[i] = 1;
    if(chkUNIx.useDefaults[i] != 0){
      chkUNIx.useDefaults[i] = 1;
      chkUNIx.hp[i] = 0;
      chkUNIx.shields[i] = 0;
      chkUNIx.armor[i] = 0;
      chkUNIx.time[i] = 0;
      chkUNIx.minerals[i] = 0;
      chkUNIx.gas[i] = 0;
      chkUNIx.string[i] = 0;
    }else{
      if(unitsdat.hasShields[i] == 0) chkUNIx.shields[i] = 0;
      if((unitsdat.abilFlags[i] & UNITSDAT_ABIL_INVINCIBLE) != 0) chkUNIx.armor[i] = 0;
      // Zero costs of units that cannot be built or repaired
      if((!(unitsdat.availFlags[i] & UNITSDAT_AVAIL_PLAYERSETTINGS) && // Does not have settings
         (!(unitsdat.abilFlags[i] & UNITSDAT_ABIL_MECHANICAL) || !(unitsdat.groupFlags[i] & UNITSDAT_GROUP_TERRAN))) || // Cannot be repaired
         i == UNIT_REPAIR_BAY){ // Because it can be repaired
        switch(i){
         case UNIT_DARK_ARCHON:
         case UNIT_ARCHON:
           // Uses time
           chkUNIx.minerals[i] = 0;
           chkUNIx.gas[i] = 0;
         case UNIT_NUCLEAR_MISSILE:
         case UNIT_INTERCEPTOR:
         case UNIT_SCARAB:
           // Uses all 3 stats
           break;
         default:
           chkUNIx.minerals[i] = 0;
           chkUNIx.gas[i] = 0;
           chkUNIx.time[i] = 0;
        }
      }
      chkUNIx.string[i] = stringReindex[chkUNIx.string[i]];
    }
  }
  for(i = 0; i < 130; i++){
    used = false;
    for(j = 0; j < 228; j++){
      if(chkUNIx.useDefaults[j] == 0){
        if( (unitsdat.gndWeapon[j] == i || unitsdat.airWeapon[j] == i) || // Unit has weapon
            unitsdat.subunit1[j] != 228 && (unitsdat.gndWeapon[unitsdat.subunit1[j]] == i || unitsdat.airWeapon[unitsdat.subunit1[j]] == i) ){ // Subunit has weapon
          used = true;
          break;
        }
      }
    }
    // Isn't used, zero it
    if(!used){
      chkUNIx.damage[i] = 0;
      chkUNIx.bonus[i] = 0;
    }
  }
}

void procUPGS(){
/*
Null all upgrade factors if the upgrade can only be achieved a single time.
Null all settings for upgrades that start at their max.
*/
  int i;
  for(i = 0; i < 46; i++){
    if(i == UPGR_BURST_LASER || i == UPGR_UNUSED_45) chkUPGS.useDefaults[i] = 0;
    if(chkUPGS.useDefaults[i] != 0){
      chkUPGS.useDefaults[i] = 1;
      chkUPGS.mineralBase[i] = 0;
      chkUPGS.mineralFactor[i] = 0;
      chkUPGS.gasBase[i] = 0;
      chkUPGS.gasFactor[i] = 0;
      chkUPGS.timeBase[i] = 0;
      chkUPGS.timeFactor[i] = 0;
    }else{
      // :-)
    }
  }
}

void procUPGx(){
/*
Null all upgrade factors if the upgrade can only be achieved a single time.
Null all settings for upgrades that start at their max.
*/
  u32 i;
  for(i = 0; i < 61; i++){
    if(i == UPGR_BURST_LASER || i == UPGR_UNUSED_45 || i == UPGR_UNUSED_46 || 
       i == UPGR_UNUSED_48 || i == UPGR_UNUSED_50 ||
       (i >= UPGR_UNUSED_55 && i <= UPGR_UNUSED_59)){
      chkUPGx.useDefaults[i] = 0;
    }
    if(chkUPGx.useDefaults[i] != 0){
      chkUPGx.useDefaults[i] = 1;
      chkUPGx.mineralBase[i] = 0;
      chkUPGx.mineralFactor[i] = 0;
      chkUPGx.gasBase[i] = 0;
      chkUPGx.gasFactor[i] = 0;
      chkUPGx.timeBase[i] = 0;
      chkUPGx.timeFactor[i] = 0;
    }else{
      //
    }
  }
  chkUPGx.unused = 0;
}

void procTECS(){
/*
Null all mineral/gas/time costs for technologies if they are defaultly researched.
*/
  u32 i;
  for(i = 0; i < 24; i++){
    if(chkTECS.useDefaults[i] != 0){
      chkTECS.useDefaults[i] = 1;
      chkTECS.minerals[i] = 0;
      chkTECS.gas[i] = 0;
      chkTECS.time[i] = 0;
      chkTECS.energy[i] = 0;
    }else{
      if(techdatadat.unused[i] == 1){ // Defaultly researched when enabled
        chkTECS.minerals[i] = 0;
        chkTECS.gas[i] = 0;
        chkTECS.time[i] = 0;
      }
      if(i == TECH_SPIDER_STIM_PACKS || /*i == TECH_SPIDER_MINES ||*/ i == TECH_TANK_SIEGE_MODE ||
         i == TECH_SPIDER_BURROWING || i == TECH_INFESTATION || i == TECH_ARCHON_WARP){
        chkTECS.energy[i] = 0; // Don't use energy
      }
    }
  }
}

void procTECx(){
/*
Null all mineral/gas/time costs for technologies if they are defaultly researched.
*/
  u32 i;
  for(i = 0; i < 44; i++){
    if(i == TECH_UNUSED_26 || i == TECH_UNUSED_33 || i >= TECH_UNUSED_35) chkTECx.useDefaults[i] = 1;
    if(chkTECx.useDefaults[i] != 0){
      chkTECx.useDefaults[i] = 1;
      chkTECx.minerals[i] = 0;
      chkTECx.gas[i] = 0;
      chkTECx.time[i] = 0;
      chkTECx.energy[i] = 0;
    }else{
      if(techdatadat.unused[i] == 1){
        chkTECx.minerals[i] = 0;
        chkTECx.gas[i] = 0;
        chkTECx.time[i] = 0;
      }
      if(i == TECH_SPIDER_STIM_PACKS || /*i == TECH_SPIDER_MINES ||*/ i == TECH_TANK_SIEGE_MODE ||
         i == TECH_SPIDER_BURROWING || i == TECH_INFESTATION || i == TECH_ARCHON_WARP ||
         i == TECH_DARK_ARCHON_MELD || i == TECH_LURKER_ASPECT){
        chkTECx.energy[i] = 0;
      }
    }
  }
}

void procCRGB(){
  u32 i;
  for(i = PLAYER_1; i <= PLAYER_8; i++){
    if(randomStartLoc == false && usedPlayers[i] == USEDPLAYER_UNUSED){
      chkCRGB.rgb[i][0] = 0;
      chkCRGB.rgb[i][1] = 0;
      chkCRGB.rgb[i][2] = 0;
      chkCRGB.player[i] = 0;
    }else{
      switch(chkCRGB.player[i]){
        case CRGB_RGB:
          // do nothing
          break;
        
        case CRGB_COLOR_ID:
          chkCRGB.rgb[i][0] = 0;
          chkCRGB.rgb[i][1] = 0;
          if(chkCRGB.rgb[i][2] > 21) chkCRGB.rgb[i][1] = i;
          break;
        
        default: // non-RGB, non-Color ID
          chkCRGB.rgb[i][0] = 0;
          chkCRGB.rgb[i][1] = 0;
          chkCRGB.rgb[i][2] = 0;
          break;
      }
    }
  }
}


// Do after PUNI, UNIT, THG2, TRIG
// Called from STR
// Do before UNIS, UNIx
void testBuildable(){
  u32 i, unit, self;
  bool ret, val;
  for(i = 0; buildops[i] != UU_EOF; i++){
    ret = true;
    self = buildops[i];
    do{
      i++;
      // get unit
      unit = buildops[i++];
      if(unit == UU_SELF) unit = self;
      
      // check property
      val = (usedUnits[unit] & buildops[i++]) != 0;
      
      // check op
      if(buildops[i] == UU_AND){
        ret &= val;
        // AND- return true if previous checks are true
      }else{
        ret &= val;
        // OR- if previous checks are true, unit is buildable & exit
        if(ret){
          usedUnits[self] |= USEDUNIT_BUILDABLE;
        }
        // Or start anew
        ret = true;
      }
      // If it's end of the line, exit
    } while(buildops[i] != UU_END);
  }
}

