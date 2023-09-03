#include "types.h"
#include "map.h"
#include "data.h"
#include "chk.h"
#include "wav.h"
#include "interface.h"
#include "options.h"
#include "SFmpq_static.h"
#include <stdio.h>

char openFilename[MAX_PATH];
char saveFilename[MAX_PATH];
u32 openSize;
u32 saveSize;
MPQHANDLE hOpenMPQ;
MapFile* mapFiles;
u32 mapFileCount;

bool mapLoadCHK(const char* filename);
bool mapLoadMPQ(const char* filename);
bool loadMapFile(MapFile* file);
void initFiles();

char* statsInit();
void statsPrint(char* stats, MapFile* file);
float getRatio(float oldSize, float newSize);

bool openMap(const char* filename, int filetype){
  if(filetype == 1){
    if(!mapLoadMPQ(filename)) return false;
  }else{
    if(!mapLoadCHK(filename)) return false;
  }
  strcpy(openFilename, filename);
  if(windowIsOpen()){
    showFileSize(openSize);
    changeWndState(WND_STATE_MAIN);
  }
  initFiles();
  listFiles();
  detectGameType();
  setCompatability();
  return true;
}


char* saveMap(const char* filename, u32 outType){
  u32 i, r;
  u32 compCalc;
  u8* chkBuffer;
  u8* tmpBuffer;
  MPQHANDLE hMPQ, hFile;
  
  if(!strcmpi(openFilename, filename)){
    dispError("As a safety precaution, you cannot overwrite the original file.", false);
    return NULL;
  }
  
  #ifdef SAVE_DBGLOG
  FILE* fDbg = fopen("dbg.log", "wb");
  if(fDbg == NULL){
    MessageBox(NULL, "Can't save dbg log?", NULL, 0);
  }
  fprintf(fDbg, "File: %s\n\nPreparing data...\n\n", filename);
  fflush(fDbg);
  #endif
  u32 fileCount = 0;
  u32 newCHKSize = 0;
  for(i = 0; i < mapFileCount; i++){
    #ifdef SAVE_DBGLOG
    fprintf(fDbg, "Preparing File %d: %s\n", i, mapFiles[i].filename);
    fflush(fDbg);
    #endif
    if(!(mapFiles[i].mpqFlags & MPQ_FLAGS_DO_NOT_ADD)){
      fileCount++;
      if(mapFiles[i].mpqFlags & MPQ_FLAGS_IS_CHK){
    #ifdef SAVE_DBGLOG
    fputs("Type: CHK\n", fDbg);
    fflush(fDbg);
    #endif
        compCalc = outType | (0x80 * delGameName) | (0x100 * trimMTXM) | (0x200 * delSPActions);
    #ifdef SAVE_DBGLOG
    fprintf(fDbg, "CompCalc = %08X\n", compCalc);
    fflush(fDbg);
    #endif
        if(mapFiles[i].comp > 0 && compCalc != mapFiles[i].comp){ // Has the file already been compressed, and with different settings?
    #ifdef SAVE_DBGLOG
    fputs("Reloading data...\n", fDbg);
    fflush(fDbg);
    #endif
          // Yes, so reload  -- save to different memory?
          free(mapFiles[i].data);
          loadMapFile(&mapFiles[i]); // Error checking
          loadCHKData(mapFiles[i].data, mapFiles[i].initSize); // Re-parse
          mapFiles[i].comp = 0;
        }
        if(mapFiles[i].comp == 0){ // File has not been compressed
    #ifdef SAVE_DBGLOG
    fputs("Compressing CHK...\n", fDbg);
    fflush(fDbg);
    #endif
          chkBuffer = createCHKBuffer(outType);
          if(outType <= MODE_MELEE_EXTENDED){
            newCHKSize = createMeleeCHK(chkBuffer, outType);
          }else{
            newCHKSize = createUMSCHK(chkBuffer, outType);
          }
    #ifdef SAVE_DBGLOG
    fprintf(fDbg, "CHK Size = %d\n", newCHKSize);
    fflush(fDbg);
    #endif
          mapFiles[i].saveSize = newCHKSize;
          free(mapFiles[i].data);
          mapFiles[i].data = chkBuffer;
          mapFiles[i].comp = compCalc;
        }
      }else if(mapFiles[i].mpqFlags & MPQ_FLAGS_IS_WAV){
    #ifdef SAVE_DBGLOG
    fputs("Type: WAV ", fDbg);
    fflush(fDbg);
    #endif
        if(mapFiles[i].mpqFlags & MPQ_FLAGS_WAV_COMP){
    #ifdef SAVE_DBGLOG
    fputs("(Compressed)\n", fDbg);
    fflush(fDbg);
    #endif
          if(mapFiles[i].comp == 0 || (mapFiles[i].comp & MPQ_FLAGS_DO_NOT_ADD) != 0){
    #ifdef SAVE_DBGLOG
    fputs("Compressing WAV...\n", fDbg);
    fflush(fDbg);
    #endif
            tmpBuffer = compWAV(mapFiles[i].data);
            if(tmpBuffer == NULL){
              MessageBox(0, "Debug: Invalid WAV or Error During Reading -- Did not compress\ncompWAV", mapFiles[i].filename, 0);
    #ifdef SAVE_DBGLOG
    fputs("Did not compress.\n", fDbg);
    fflush(fDbg);
    #endif
              mapFiles[i].saveSize = mapFiles[i].initSize;
              mapFiles[i].comp = 0;
            }else{
              free(mapFiles[i].data);
              mapFiles[i].data = tmpBuffer;
              mapFiles[i].saveSize= ((RIFF*)tmpBuffer)->cksize + 8;
              mapFiles[i].comp = WAV_MONO;
            }
          }
        }else{
    #ifdef SAVE_DBGLOG
    fputs("(Uncompressed)\n", fDbg);
    fflush(fDbg);
    #endif
          if(mapFiles[i].comp > 0 && (mapFiles[i].comp & MPQ_FLAGS_DO_NOT_ADD) == 0){
    #ifdef SAVE_DBGLOG
    fputs("Reloading data...\n", fDbg);
    fflush(fDbg);
    #endif
            free(mapFiles[i].data);
            loadMapFile(&mapFiles[i]);
            mapFiles[i].comp = 0;
          }
          if(mapFiles[i].comp == 0){
    #ifdef SAVE_DBGLOG
    fputs("Getting WAV Data...\n", fDbg);
    fflush(fDbg);
    #endif
            mapFiles[i].saveSize = mapFiles[i].initSize;
            mapFiles[i].comp = getWAVChannels(mapFiles[i].data, mapFiles[i].initSize);
            if(mapFiles[i].comp == 0){
              MessageBox(0, "Debug: Invalid WAV or Error During Reading -- Could not get nChannels\ngetWAVChannels", mapFiles[i].filename, 0);
            }
          }
        }
    #ifdef SAVE_DBGLOG
    fprintf(fDbg, "Channels: %d\nFile Size = %d\n", mapFiles[i].comp, mapFiles[i].saveSize);
    fflush(fDbg);
    #endif
      }else{
        mapFiles[i].saveSize = mapFiles[i].initSize;
    #ifdef SAVE_DBGLOG
    fprintf(fDbg, "Type: File\nFile Size = %d\n", mapFiles[i].saveSize);
    fflush(fDbg);
    #endif
      }
    }
  #ifdef SAVE_DBGLOG
  fputs("Done preparing file\n\n", fDbg);
  fflush(fDbg);
  #endif
  }
  
  #ifdef SAVE_DBGLOG
  fputs("Done preparing data\n\n\n\n", fDbg);
  fprintf(fDbg, "\tMpqOpenArchiveForUpdateEx(%s, %08X, %d, %d);\n", filename, MOAU_CREATE_ALWAYS, fileCount, optMPQBlockSize);
  fflush(fDbg);
  #endif
  if((GetFileAttributes(filename) & FILE_ATTRIBUTE_DIRECTORY) == 0) remove(filename);
  hMPQ = MpqOpenArchiveForUpdateEx(filename, MOAU_CREATE_ALWAYS, fileCount, optMPQBlockSize);
  #ifdef SAVE_DBGLOG
  fprintf(fDbg, "Created archive (r=%08x)\n", hMPQ);
  fflush(fDbg);
  #endif
  for(i = 0; i < mapFileCount; i++){
  #ifdef SAVE_DBGLOG
  fprintf(fDbg, "Loading MPQ File %d: %s\n", i, mapFiles[i].filename);
  fflush(fDbg);
  #endif
    if(!(mapFiles[i].mpqFlags & MPQ_FLAGS_DO_NOT_ADD)){
      if(mapFiles[i].mpqFlags & MPQ_FLAGS_IS_WAV){
        if(mapFiles[i].mpqFlags & MPQ_FLAGS_COMP_MASK){
          switch(mapFiles[i].comp & ~MPQ_FLAGS_DO_NOT_ADD){
            case WAV_MONO:
  #ifdef SAVE_DBGLOG
  fprintf(fDbg, "Adding Mono WAV, Comp Level: %d\nSize: %d\n", mapFiles[i].mpqFlags & MPQ_FLAGS_COMP_MASK, mapFiles[i].saveSize);
  fprintf(fDbg, "\tMpqAddFileFromBufferEx(%08x, %08x, %d, %s, %08X, %d, %d);\n", hMPQ, mapFiles[i].data, mapFiles[i].saveSize, mapFiles[i].filename, MAFA_COMPRESS, MAFA_COMPRESS_WAVE2, (mapFiles[i].mpqFlags & MPQ_FLAGS_COMP_MASK)-1);
  fflush(fDbg);
  #endif
              r = MpqAddFileFromBufferEx(hMPQ, mapFiles[i].data, mapFiles[i].saveSize, mapFiles[i].filename, MAFA_COMPRESS, MAFA_COMPRESS_WAVE2, (mapFiles[i].mpqFlags & MPQ_FLAGS_COMP_MASK)-1);
              break;
            case WAV_STEREO:
  #ifdef SAVE_DBGLOG
  fprintf(fDbg, "Adding Stereo WAV, Comp Level: %d\nSize: %d\n", mapFiles[i].mpqFlags & MPQ_FLAGS_COMP_MASK, mapFiles[i].saveSize);
  fprintf(fDbg, "\tMpqAddFileFromBufferEx(%08x, %08x, %d, %s, %08X, %d, %d);\n", hMPQ, mapFiles[i].data, mapFiles[i].saveSize, mapFiles[i].filename, MAFA_COMPRESS, MAFA_COMPRESS_WAVE, (mapFiles[i].mpqFlags & MPQ_FLAGS_COMP_MASK)-1);
  fflush(fDbg);
  #endif
              r = MpqAddFileFromBufferEx(hMPQ, mapFiles[i].data, mapFiles[i].saveSize, mapFiles[i].filename, MAFA_COMPRESS, MAFA_COMPRESS_WAVE, (mapFiles[i].mpqFlags & MPQ_FLAGS_COMP_MASK)-1);
              break;
            default:
  #ifdef SAVE_DBGLOG
  fprintf(fDbg, "Adding invalid WAV (Generic file), Comp Level: %d\nSize: %d\n", mapFiles[i].mpqFlags & MPQ_FLAGS_COMP_MASK, mapFiles[i].saveSize);
  fprintf(fDbg, "\tMpqAddWaveFromBuffer(%08x, %08X, %d, %s, %08X, %d);\n", hMPQ, mapFiles[i].data, mapFiles[i].saveSize, mapFiles[i].filename, MAFA_COMPRESS, (mapFiles[i].mpqFlags & MPQ_FLAGS_COMP_MASK)-1);
  fflush(fDbg);
  #endif
              r = MpqAddWaveFromBuffer(hMPQ, mapFiles[i].data, mapFiles[i].saveSize, mapFiles[i].filename, MAFA_COMPRESS, (mapFiles[i].mpqFlags & MPQ_FLAGS_COMP_MASK)-1);
          }
        }else{
  #ifdef SAVE_DBGLOG
  fprintf(fDbg, "Adding File, Uncompressed\nSize: %d\n", mapFiles[i].saveSize);
  fprintf(fDbg, "\tMpqAddFileFromBuffer(%08x, %08X, %d, %s, %08X);\n", hMPQ, mapFiles[i].data, mapFiles[i].saveSize, mapFiles[i].filename, 0);
  fflush(fDbg);
  #endif
          r = MpqAddFileFromBuffer(hMPQ, mapFiles[i].data, mapFiles[i].saveSize, mapFiles[i].filename, 0);
        }
      }else{
  #ifdef SAVE_DBGLOG
  fprintf(fDbg, "Adding File, Comp: %08X\nSize: %d\n", mapFiles[i].mpqFlags & MPQ_FLAGS_COMP_MASK, mapFiles[i].saveSize);
  fprintf(fDbg, "\tMpqAddFileFromBuffer(%08x, %08X, %d, %s, %08X);\n", hMPQ, mapFiles[i].data, mapFiles[i].saveSize, mapFiles[i].filename, (mapFiles[i].mpqFlags & MPQ_FLAGS_COMP_MASK));
  fflush(fDbg);
  #endif
        r = MpqAddFileFromBuffer(hMPQ, mapFiles[i].data, mapFiles[i].saveSize, mapFiles[i].filename, (mapFiles[i].mpqFlags & MPQ_FLAGS_COMP_MASK));
      }
  #ifdef SAVE_DBGLOG
  fprintf(fDbg, "File Added (r=%d)\n", r);
  fflush(fDbg);
  fputs("Getting compressed size... \n", fDbg);
  fprintf(fDbg, "\tSFileOpenFileEx(%08X, %s, %d, *%08X [%08X]);\n", hMPQ, mapFiles[i].filename, SFILE_SEARCH_CURRENT_ONLY, hFile, &hFile);
  fflush(fDbg);
  #endif
      r = SFileOpenFileEx(hMPQ, mapFiles[i].filename, SFILE_SEARCH_CURRENT_ONLY, &hFile);
  #ifdef SAVE_DBGLOG
  fprintf(fDbg, "r=%d\n", r);
  fprintf(fDbg, "\tSFileGetFileInfo(%08X, %08X);\n", hFile, SFILE_INFO_COMPRESSED_SIZE);
  fflush(fDbg);
  #endif
      mapFiles[i].saveCompSize = SFileGetFileInfo(hFile, SFILE_INFO_COMPRESSED_SIZE);
  #ifdef SAVE_DBGLOG
  fprintf(fDbg, "Size = %d\n", mapFiles[i].saveCompSize);
  fflush(fDbg);
  #endif
      SFileCloseFile(hFile);
    }
  #ifdef SAVE_DBGLOG
  fputs("Done\n\n", fDbg);
  fflush(fDbg);
  #endif
  }
  saveSize = SFileGetFileInfo(hMPQ, SFILE_INFO_SIZE);
  MpqCloseUpdatedArchive(hMPQ, 0);
  #ifdef SAVE_DBGLOG
  fputs("\n\nDone making MPQ -- Generating statistics\n", fDbg);
  fflush(fDbg);
  #endif
  
  char* stats = statsInit();
  // print CHK
  for(i = 0; i < mapFileCount; i++){
    if(mapFiles[i].mpqFlags & MPQ_FLAGS_IS_CHK) statsPrint(stats, &mapFiles[i]);
  }
  // print WAVs & OGGs
  for(i = 0; i < mapFileCount; i++){
    if(mapFiles[i].mpqFlags & MPQ_FLAGS_IS_WAV_OR_OGG) statsPrint(stats, &mapFiles[i]);
  }
  // print everything else
  for(i = 0; i < mapFileCount; i++){
    if(!(mapFiles[i].mpqFlags & MPQ_FLAGS_IS_FILE)) statsPrint(stats, &mapFiles[i]);
  }
  
  #ifdef SAVE_DBGLOG
  fputs(stats, fDbg);
  fclose(fDbg);
  #endif
  return stats;
}

void closeMap(){
  u32 i;
  for(i = 0; i < mapFileCount; i++){
    if(mapFiles[i].data) free(mapFiles[i].data);
  }
  if(mapFiles != NULL){
    free(mapFiles);
    mapFiles = NULL;
  }
  mapFileCount = 0;
  if(hOpenMPQ != NULL){
    SFileCloseArchive(hOpenMPQ);
    hOpenMPQ = NULL;
  }
  clearCHK();
}



bool mapLoadCHK(const char* filename){
  closeMap();
  FILE* f = fopen(filename, "rb");
  if(f == NULL) return false;
  if(fseek(f, 0, SEEK_END)){
    fclose(f);
    return false;
  }
  int filesize = ftell(f);
  if(filesize == 0 || filesize < 0){
    fclose(f);
    return false;
  }
  rewind(f);
  
  mapFiles = calloc(1, sizeof(MapFile));
  if(mapFiles == NULL){
    fclose(f);
    return false;
  }
  strcpy(mapFiles[0].filename, SCENARIO_CHK);
  mapFiles[0].initSize = filesize;
  mapFiles[0].initCompSize = filesize;
  mapFiles[0].data = malloc(filesize);
  if(mapFiles[0].data == NULL){
    free(mapFiles);
    mapFiles = NULL;
    fclose(f);
    return false;
  }
  if(fread(mapFiles[0].data, 1, filesize, f) != filesize){
    free(mapFiles[0].data);
    free(mapFiles);
    mapFiles = NULL;
    fclose(f);
    return false;
  }
  fclose(f);
  mapFileCount = 1;
  openSize = filesize;
  hOpenMPQ = NULL;
  
  loadCHKData(mapFiles[0].data, filesize);
  return true;
}

bool mapLoadMPQ(const char* filename){
  FILELISTENTRY *mpqList;
  DWORD hashEntries;
  u32 i;
  
  closeMap();
  SFileOpenArchive(filename, 10, 0, &hOpenMPQ);
  if(hOpenMPQ == NULL) return false;
  u32 filecount = 0;
  
  // get CHK and generate a listfile with any referenced wav files
  u8* chkData = NULL;
  u32 chkSize = loadMPQFile(SCENARIO_CHK, (void**)&chkData);
  if(chkSize == 0){
    SFileCloseArchive(hOpenMPQ);
    hOpenMPQ = NULL;
    return false;
  }
  loadCHKData(chkData, chkSize);
  genDefaultListfile();
  
  hashEntries = SFileGetFileInfo(hOpenMPQ, SFILE_INFO_HASH_TABLE_SIZE);
  mpqList = calloc(hashEntries, sizeof(FILELISTENTRY));
  SFileListFiles(hOpenMPQ, listfiles, mpqList, SFILE_LIST_FLAG_UNKNOWN);
  for(i = 0; i < hashEntries; i++){
    switch(mpqList[i].dwFileExists){
      case 0: // Unused file
        break;
      
      case 1: // File exists
        filecount++;
        break;
      
      case 3: // Unknown file
        dispError("One or more unknown files were found in the MPQ.\nPlease update your listfile in the Options window and try again.", false);
        free(mpqList);
        free(chkData);
        return false;
      
      default: // Shouldn't happen
        dispError("Some other state occured!", false);
        free(mpqList);
        free(chkData);
        return false;
    }
  }
  
  mapFiles = calloc(filecount, sizeof(MapFile));
  mapFileCount = 0;
  for(i = 0; i < hashEntries; i++){
    if(mpqList[i].dwFileExists == 1){
      strcpy(mapFiles[mapFileCount].filename, mpqList[i].szFileName);
      mapFiles[mapFileCount].initSize = mpqList[i].dwFullSize;
      mapFiles[mapFileCount].initCompSize = mpqList[i].dwCompressedSize;
      if(mpqList[i].dwFullSize == chkSize && strcmp(mpqList[i].szFileName, SCENARIO_CHK) == 0){
        // chk data is already loaded
        mapFiles[mapFileCount].data = chkData;
      }else{
        loadMapFile(&mapFiles[mapFileCount]);
        // Error check!
      }
      mapFileCount++;
    }
  }
  free(mpqList);
  openSize = SFileGetFileInfo(hOpenMPQ, SFILE_INFO_SIZE);
  return true;
}

bool loadMapFile(MapFile* file){
  MPQHANDLE hFile;
  if(!SFileOpenFile(file->filename, &hFile)) return false;
  if(file->initSize == 0){
    file->initSize = SFileGetFileSize(hFile, NULL);
    if(file->initSize == 0){ // size is actually 0
      file->data = NULL;
      SFileCloseFile(hFile);
      return true;
    }
  }
  file->data = malloc(file->initSize);
  if(file->data == NULL){
    SFileCloseFile(hFile);
    return false;
  }
  SFileReadFile(hFile, file->data, file->initSize, NULL, NULL);
  // error checking?
  SFileCloseFile(hFile);
  return true;
}


// check each mpq file & set default options
void initFiles(){
  u32 i;
  for(i = 0; i < mapFileCount; i++){
    if(getWAVChannels(mapFiles[i].data, mapFiles[i].initSize)){
      // is WAV
      mapFiles[i].wavUsage.trig = 0;
      mapFiles[i].wavUsage.mbrf = 0;
      mapFiles[i].mpqFlags = getDefaultMPQFlags(MPQ_FILETYPE_WAV) | MPQ_FLAGS_IS_WAV;
    }else if(isOGG(mapFiles[i].data, mapFiles[i].initSize)){
      // is OGG
      mapFiles[i].wavUsage.trig = 0;
      mapFiles[i].wavUsage.mbrf = 0;
      mapFiles[i].mpqFlags = getDefaultMPQFlags(MPQ_FILETYPE_OGG) | MPQ_FLAGS_IS_OGG;
    }else if(strcmpi(mapFiles[i].filename, SCENARIO_CHK) == 0){
      // is CHK
      mapFiles[i].mpqFlags = getDefaultMPQFlags(MPQ_FILETYPE_CHK) | MPQ_FLAGS_IS_CHK;
    }else if(strcmpi(mapFiles[i].filename, "(listfile)") == 0){
      // is listfile
      mapFiles[i].mpqFlags = getDefaultMPQFlags(MPQ_FILETYPE_LISTFILE) | MPQ_FLAGS_IS_FILE;
    }else{
      // unknown filetype
      mapFiles[i].mpqFlags = getDefaultMPQFlags(MPQ_FILETYPE_UNKNOWN) | MPQ_FLAGS_IS_FILE;
    }
  }
  
  countWAVs(mapFiles, mapFileCount);
  
  // mark unused WAVs as do not add
  for(i = 0; i < mapFileCount; i++){
    if((mapFiles[i].mpqFlags & MPQ_FLAGS_IS_WAV_OR_OGG) && mapFiles[i].wavUsage.trig == 0 && mapFiles[i].wavUsage.mbrf == 0){
      mapFiles[i].mpqFlags |= MPQ_FLAGS_DO_NOT_ADD;
      mapFiles[i].mpqFlags &= ~(MPQ_FLAGS_WAV_COMP | MPQ_FLAGS_WAV_CONV); // don't convert
    }
  }
}

void listFiles(){
  u32 i;
  if(!windowIsOpen()) return;
  lFilesClear();
  for(i = 0; i < mapFileCount; i++){
    lFilesAdd(mapFiles[i].filename, &mapFiles[i]);
  }
}


char* statsInit(){
  u32 i;
  u32 size = 0;
  for(i = 0; i < mapFileCount; i++){
    size += 40 + strlen(mapFiles[i].filename);
  }
  char* tmpStr = malloc(size);
  sprintf(tmpStr, " Type   Init Size New Size Comp Ratio Filename\r\n"
                  "Mapfile %9u %8u %9.2f%% MPQ\r\n", openSize, saveSize, getRatio(openSize, saveSize));
  return tmpStr;
}

void statsPrint(char* stats, MapFile* file){
  char type[12];
  char info[34];
  
  switch(file->mpqFlags & MPQ_FLAGS_TYPE_MASK){
    case MPQ_FLAGS_IS_CHK:
      strcpy(type, "Raw CHK");
      break;
    case MPQ_FLAGS_IS_WAV:
      if(file->mpqFlags & MPQ_FLAGS_WAV_CONV){
        strcpy(type, "WAV->OGG");
      }else{
        strcpy(type, "WAV");
      }
      break;
    case MPQ_FLAGS_IS_OGG:
      strcpy(type, "OGG");
      break;
    case MPQ_FLAGS_IS_FILE:
      if(strcmpi(file->filename, "(listfile)") == 0){
        strcpy(type, "Listfile");
      }else{
        strcpy(type, "Other");
      }
      break;
    default:
      strcpy(type, "Unknown");
      break;
  }
  
  if(file->mpqFlags & MPQ_FLAGS_DO_NOT_ADD){
    sprintf(info, "%-30s", " <Skipped>");
  }else{
    sprintf(info, "%9u %8u %9.2f%% ", file->initCompSize, file->saveCompSize, getRatio(file->initCompSize, file->saveCompSize));
  }
  
  // strcat
  sprintf(stats + strlen(stats), "\r\n%-8s%s%s", type, info, file->filename);
}

float getRatio(float oldSize, float newSize){
  return (newSize*100.f)/oldSize;
}
