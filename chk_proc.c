#include "types.h"
#include "chk.h"
#include "options.h"
#include "data.h"
#include "chk_proc.h"

// CHK Data
extern u32   chkTYPE;
extern u16   chkVER;
extern VCOD  chkVCOD;
extern u8    chkOWNR[12];
extern u16   chkERA;
extern DIM   chkDIM;
extern u8    chkSIDE[12];
extern u16   chkMTXM[65536];
extern PUNI  chkPUNI;
extern UPGR  chkUPGR;
extern PTEC  chkPTEC;
extern UNIT* chkUNIT;
extern u32*  unitlist; // chkUNIT ID
extern writeentry* unitwrite;
extern THG2* chkTHG2;
extern u8    chkMASK[65536];
extern STR   chkSTR;
extern UPRP  chkUPRP[64];
extern MRGN  chkMRGN[255];
extern TRIG* chkTRIG;
extern TRIG* chkMBRF;
extern u32*  triglist; // chkTRIG ID
extern writeentry* trigwrite;
extern u16   chkSPRP[2];
extern FORC  chkFORC;
extern UNIS  chkUNIS;
extern UPGS  chkUPGS;
extern TECS  chkTECS;
extern u8    chkCOLR[8];
extern PUPx  chkPUPx;
extern PTEx  chkPTEx;
extern UNIx  chkUNIx;
extern UPGx  chkUPGx;
extern TECx  chkTECx;
extern CRGB  chkCRGB;

extern u32 mapSize; // mapDIM.x * mapDIM.y
extern u32 mtxmSize;
extern u32 maskSize;
extern u32 trigCount, trigUnique, trigWCount, trigMax;
extern u32 mbrfCount;
extern u32 unitCount, unitUnique, unitWCount;
extern u32 thg2Count;
extern u32 forcSize;

extern bool loadedSections[SECTION_COUNT];
extern u8 usedUnits[228]; // Uses USEDUNIT_* flags to determine if a unit is preplaced, enabled, or can be built in-game
extern u8 usedWeapons[100];
extern s8 usedPlayers[8];
extern u8 playerReindex[8];
extern u8* usedStrings;
extern u16* stringReindex;
extern bool usedLocations[256];
extern u8 locationReindex[256];
extern u8 cuwpReindex[64];
extern u32 usedLocationCount;
extern bool use255Locations;
extern bool randomStartLoc;
extern bool useFORCData;

extern const ActDef Conditions[COND_COUNT];
extern const ActDef Actions[ACT_COUNT];
extern const ActDef BriefingActions[BACT_COUNT];


void procVCOD(){
  u32 i,j;
  playerStruct players[12] = {0};
  
  u32 zeroCount = 0;
  for(i = PLAYER_1; i <= PLAYER_12; i++){
    if(chkOWNR[i] == 0) zeroCount++;
    if(chkSIDE[i] == 0) zeroCount++;
  }
  
  // number of zeros between OWNR and SIDE *must* be odd
  if((zeroCount & 1) == 0){
    // search for nonzero value adjacent to zeros
    for(i = PLAYER_1; i <= PLAYER_12; i++){
      if((usedPlayers[i] <= USEDPLAYER_NEUTRAL || i > PLAYER_8) && chkSIDE[i] != 0){
        if((i > PLAYER_1 && chkSIDE[i-1] == 0) || (i < PLAYER_12 && chkSIDE[i+1] == 0)){
          chkSIDE[i] = 0;
          break;
        }
      }
    }
    
    // if previous case not found, search for a zero value and overwrite with an adjacent nonzero value
    if(i > PLAYER_12){
      for(i = PLAYER_1; i <= PLAYER_12; i++){
        if((usedPlayers[i] <= USEDPLAYER_NEUTRAL || i > PLAYER_8) && chkSIDE[i] == 0){
          if(i > PLAYER_1 && chkSIDE[i-1] != 0){
            chkSIDE[i] = chkSIDE[i-1];
            break;
          }else if(i < PLAYER_12 && chkSIDE[i+1] != 0){
            chkSIDE[i] = chkSIDE[i-1];
            break;
          }
        }
      }
    }
    
    // if previous case not found, change either the last player or earliest unused player
    if(i > PLAYER_12){
      if(chkOWNR[PLAYER_12] == 5){
        chkOWNR[PLAYER_12] = 0;
      }else if(chkSIDE[PLAYER_12] == 0){
        chkSIDE[PLAYER_12] = 1;
      }else{
        for(i = PLAYER_1; i <= PLAYER_12; i++){
          if((usedPlayers[i] <= USEDPLAYER_NEUTRAL || i > PLAYER_8) && chkSIDE[i] == 0){
            chkSIDE[i] = 1;
            break;
          }
        }
      }
    }
    
    // if previous case was not found, invert last player
    if(i > PLAYER_12){
      chkOWNR[PLAYER_12] = !chkOWNR[PLAYER_12];
    }
  }
  
  for(i = PLAYER_1; i <= PLAYER_12; i++){
    players[i].type = chkOWNR[i];
    players[i].race = chkSIDE[i];
  }
  
  u8* playerData = (u8*)(players);
  
  // compute VCOD hash
  u32 hash = 0;
  u32 xorData = 0;
  for(i = 0; i < sizeof(players); i += 4){
    xorData ^= *(u32*)(&playerData[i]);
    for(j = 0; j < 16; j++){
      switch(refVCOD.opcodes[j]){
        case 0:
          hash ^= *(u32*)(&playerData[i]);
          break;
        case 1:
          hash += *(u32*)(&playerData[i]);
          break;
        case 2:
          hash -= *(u32*)(&playerData[i]);
          break;
        
        case 3:
        case 4:
        case 5:
          hash ^= refVCOD.seeds[playerData[i+0]]; // Type
          hash ^= refVCOD.seeds[playerData[i+1]]; // Race
          hash ^= refVCOD.seeds[playerData[i+2]]; // Force (always 0)
          hash ^= refVCOD.seeds[playerData[i+3]]; // Name (always 0)
          break;
        
        case 6:
          hash = (hash >> (32 - (playerData[i+0] & 31))) | (hash << (playerData[i+0] & 31));
          hash = (hash >> (32 - (playerData[i+1] & 31))) | (hash << (playerData[i+1] & 31));
          hash = (hash >> (32 - (playerData[i+2] & 31))) | (hash << (playerData[i+2] & 31));
          hash = (hash >> (32 - (playerData[i+3] & 31))) | (hash << (playerData[i+3] & 31));
          break;
        
        case 7:
          hash = (hash << (32 - (playerData[i+0] & 31))) | (hash >> (playerData[i+0] & 31));
          hash = (hash << (32 - (playerData[i+1] & 31))) | (hash >> (playerData[i+1] & 31));
          hash = (hash << (32 - (playerData[i+2] & 31))) | (hash >> (playerData[i+2] & 31));
          hash = (hash << (32 - (playerData[i+3] & 31))) | (hash >> (playerData[i+3] & 31));
          break;
      }
    }
  }
  
  memset(&chkVCOD, 0, sizeof(VCOD));
  chkVCOD.seeds[0] = hash ^ xorData;
  chkVCOD.opcodes[15] = 3;
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

void procMRGN(){
  u32 i;
  u32 newid = 0;
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
  if(newid < 255){
    memset(&chkMRGN[newid], 0, (255 - newid) * sizeof(MRGN));
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
