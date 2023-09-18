#include "types.h"
#include "data.h"
#include "SFmpq_static.h"

UNITSDAT unitsdat;
WEAPONSDAT weaponsdat;
UPGRADESDAT upgradesdat;
TECHDATADAT techdatadat;
VCOD refVCOD;

u16* tileData[8] = {0};
int tileDataSize[8] = {0};

const char tilesetNames[8][9] = {"badlands", "platform", "install", "ashworld", "jungle", "desert", "ice", "twilight"};

bool loadDataMPQ(){
  MPQHANDLE hMPQ = NULL;
  int i;
  char path[24];
#ifdef DATA_EMBED
  char appPath[MAX_PATH];
  if(GetModuleFileName(NULL, appPath, MAX_PATH) == 0){
    strcpy(appPath, DATA_MPQ);
  }
  SFileOpenArchive(appPath, 10, 0, &hMPQ);
#else
  SFileOpenArchive(DATA_MPQ, 10, 0, &hMPQ);
#endif
  if(hMPQ == NULL) return false;
  if(!loadDatFile("arr\\units.dat", &unitsdat, sizeof(UNITSDAT)) ||
     !loadDatFile("arr\\weapons.dat", &weaponsdat, sizeof(WEAPONSDAT)) ||
     !loadDatFile("arr\\upgrades.dat", &upgradesdat, sizeof(UPGRADESDAT)) ||
     !loadDatFile("arr\\techdata.dat", &techdatadat, sizeof(TECHDATADAT)) ||
     !loadDatFile("vcod.bin", &refVCOD, sizeof(VCOD))) {
    SFileCloseArchive(hMPQ);
    return false;
  }
  for(i = 0; i < 8; i++){
    sprintf(path, "tileset\\%s.bin", tilesetNames[i]);
    tileDataSize[i] = loadMPQFile(path, (void**)&tileData[i]);
    if(tileDataSize[i] == 0) break; // error loading data
  }
  SFileCloseArchive(hMPQ);
  if(i != 8){
    unloadDataMPQ();
    return false;
  }
  return true;
}

void unloadDataMPQ(){
  int i;
  for(i = 0; i < 8; i++){
    if(tileData[i] != NULL) free(tileData[i]);
    tileData[i] = NULL;
  }
}


// allocates buffer & returns size
u32 loadMPQFile(const char filename[], void** buffer){
  MPQHANDLE hFile;
  DWORD size;
  if(!SFileOpenFile(filename, &hFile)) return 0;
  size = SFileGetFileSize(hFile, NULL);
  if(size == 0){
    *buffer = NULL;
    SFileCloseFile(hFile);
    return 0;
  }
  *buffer = malloc(size);
  SFileReadFile(hFile, *buffer, size, NULL, NULL);
  SFileCloseFile(hFile);
  return size;
}


// writes to buffer, returns true if bytes read matches size
bool loadDatFile(const char* filename, void* buffer, u32 size){
  MPQHANDLE hFile;
  DWORD getSize;
  if(!SFileOpenFile(filename, &hFile)) return false;
  getSize = SFileGetFileSize(hFile, NULL);
  if(getSize == size){
    SFileReadFile(hFile, buffer, size, &getSize, NULL);
  }
  SFileCloseFile(hFile);
  return getSize == size;
}
