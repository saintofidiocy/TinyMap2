#include "options.h"
#include "chk.h"
#include "SFmpq_static.h"
#include <windows.h>


// Default option valus
#define OPT_SAVELAST      true
#define OPT_AUTODETECT    true
#define OPT_SAVELASTDIR   true
#define OPT_STARTDIR      ""
#define OPT_MPQBLOCKSIZE  3
#define DEF_GAMETYPE      GAMETYPE_UMS
#define DEF_COMPAT        COMPAT_HYBRID
#define DEF_DELNAME       false
#define DEF_TRIMMTXM      false
#define DEF_DELSPACTS     true
#define DEF_MPQCOMP_FILE  MPQ_COMP_DO_NOT_ADD
#define DEF_MPQCOMP_CHK   MPQ_COMP_TYPE2
#define DEF_MPQCOMP_WAV   MPQ_COMP_WAV_HIGH
#define DEF_MPQCOMP_OGG   MPQ_COMP_TYPE2
#define DEF_MPQCOMP_LIST  MPQ_COMP_DO_NOT_ADD
#define DEF_COMPWAV       COMPWAV_CONV
#define SKIN_FORMBG       0x00DFD3CC
#define SKIN_FORMTEXT     0x00000000
#define SKIN_INPUTBG      0x00FFFFFF
#define SKIN_INPUTTEXT    0x00000000


char iniPath[MAX_PATH];

// Settings
bool optSaveLast = OPT_SAVELAST;
bool optAutoDetect = OPT_AUTODETECT;
bool optSaveLastDir = OPT_SAVELASTDIR;
char optStartDir[MAX_PATH] = OPT_STARTDIR;
u32 optMPQBlockSize = OPT_MPQBLOCKSIZE;

// Defaults
u32 defGameType = DEF_GAMETYPE;
u32 defCompat = DEF_COMPAT;
bool defDelName = DEF_DELNAME;
bool defTrimMTXM = DEF_TRIMMTXM;
bool defDelSPActs = DEF_DELSPACTS;
u32 defMPQComps[MPQ_FILETYPE_COUNT] = {DEF_MPQCOMP_FILE, DEF_MPQCOMP_CHK, DEF_MPQCOMP_WAV, DEF_MPQCOMP_OGG, DEF_MPQCOMP_LIST};
u32 defCompWAV = DEF_COMPWAV;

// Skin
u32 skinFormBG = SKIN_FORMBG;
u32 skinFormText = SKIN_FORMTEXT;
u32 skinInputBG = SKIN_INPUTBG;
u32 skinInputText = SKIN_INPUTTEXT;

// Listfiles
char defaultListfile[MAX_PATH];
char* listfiles;

char** listfileList;
u32 listfileCount;

void generateListfiles();
BOOL WritePrivateProfileInt(char* lpAppName, char* lpKeyName, int nValue, char* lpFileName);

void loadOptions(){
  char appPath[MAX_PATH];
  char loadFile[MAX_PATH];
  char numbuf[12];
  u32 i;
  u32 size;
  
  GetModuleFileName(NULL, appPath, MAX_PATH);
  for(i = strlen(appPath); i > 0 && appPath[i] != '\\'; i--);
  appPath[i] = 0;
  sprintf(iniPath, "%s\\TinyMap2.ini", appPath);
  sprintf(defaultListfile, "%s\\listfile", appPath);
  
  optSaveLast = GetPrivateProfileInt("Settings", "SaveLast", OPT_SAVELAST, iniPath);
  optAutoDetect = GetPrivateProfileInt("Settings", "AutoDetect", OPT_AUTODETECT, iniPath);
  optSaveLastDir = GetPrivateProfileInt("Settings", "SaveLastDir", OPT_SAVELASTDIR, iniPath);
  optMPQBlockSize = GetPrivateProfileInt("Settings", "MPQBlockSize", OPT_MPQBLOCKSIZE, iniPath);
  GetPrivateProfileString("Settings", "StartDir", OPT_STARTDIR, optStartDir, MAX_PATH, iniPath);
  
  defGameType = GetPrivateProfileInt("Defaults", "GameType", DEF_GAMETYPE, iniPath);
  defCompat = GetPrivateProfileInt("Defaults", "Compatability", DEF_COMPAT, iniPath);
  defDelName = GetPrivateProfileInt("Defaults", "DelName", DEF_DELNAME, iniPath);
  defTrimMTXM = GetPrivateProfileInt("Defaults", "TrimMTXM", DEF_TRIMMTXM, iniPath);
  defDelSPActs = GetPrivateProfileInt("Defaults", "DelSPActs", DEF_DELSPACTS, iniPath);
  defMPQComps[MPQ_FILETYPE_LISTFILE] = GetPrivateProfileInt("Defaults", "listfile", DEF_MPQCOMP_LIST, iniPath);
  defMPQComps[MPQ_FILETYPE_CHK] = GetPrivateProfileInt("Defaults", "chk", DEF_MPQCOMP_CHK, iniPath);
  defMPQComps[MPQ_FILETYPE_WAV] = GetPrivateProfileInt("Defaults", "wav", DEF_MPQCOMP_WAV, iniPath);
  defMPQComps[MPQ_FILETYPE_OGG] = GetPrivateProfileInt("Defaults", "ogg", DEF_MPQCOMP_OGG, iniPath);
  defMPQComps[MPQ_FILETYPE_UNKNOWN] = GetPrivateProfileInt("Defaults", "other", DEF_MPQCOMP_FILE, iniPath);
  defCompWAV = GetPrivateProfileInt("Defaults", "wavcomp", DEF_COMPWAV, iniPath);
  
  skinFormBG = GetPrivateProfileInt("Skin", "FormBG", SKIN_FORMBG, iniPath);
  skinFormText = GetPrivateProfileInt("Skin", "FormText", SKIN_FORMTEXT, iniPath);
  skinInputBG = GetPrivateProfileInt("Skin", "InputBG", SKIN_INPUTBG, iniPath);
  skinInputText = GetPrivateProfileInt("Skin", "InputText", SKIN_INPUTTEXT, iniPath);
  
  listfileCount = GetPrivateProfileInt("Listfiles", "Count", 0, iniPath);
  if(listfileCount > 0){
    listfileList = malloc(listfileCount * sizeof(char*));
    for(i = 0; i < listfileCount; i++){
      sprintf(numbuf, "%d", i);
      size = GetPrivateProfileString("Listfiles", numbuf, "", loadFile, MAX_PATH, iniPath);
      listfileList[i] = malloc(size+1);
      strcpy(listfileList[i], loadFile);
    }
  }
  
  generateListfiles();
  
  gameType = defGameType;
  compatability = defCompat;
  delGameName = defDelName;
  trimMTXM = defTrimMTXM;
  delSPActions = defDelSPActs;
}


void saveOptions(){
  char num[12];
  int i;
  
  WritePrivateProfileInt("Settings", "SaveLast", optSaveLast, iniPath);
  WritePrivateProfileInt("Settings", "AutoDetect", optAutoDetect, iniPath);
  WritePrivateProfileInt("Settings", "SaveLastDir", optSaveLastDir, iniPath);
  WritePrivateProfileString("Settings", "StartDir", optStartDir, iniPath);
  WritePrivateProfileInt("Settings", "MPQBlockSize", optMPQBlockSize, iniPath);
  
  WritePrivateProfileInt("Defaults", "GameType", defGameType, iniPath);
  WritePrivateProfileInt("Defaults", "Compatability", defCompat, iniPath);
  WritePrivateProfileInt("Defaults", "DelName", defDelName, iniPath);
  WritePrivateProfileInt("Defaults", "TrimMTXM", defTrimMTXM, iniPath);
  WritePrivateProfileInt("Defaults", "DelSPActs", defDelSPActs, iniPath);
  WritePrivateProfileInt("Defaults", "listfile", defMPQComps[MPQ_FILETYPE_LISTFILE], iniPath);
  WritePrivateProfileInt("Defaults", "chk", defMPQComps[MPQ_FILETYPE_CHK], iniPath);
  WritePrivateProfileInt("Defaults", "wav", defMPQComps[MPQ_FILETYPE_WAV], iniPath);
  WritePrivateProfileInt("Defaults", "ogg", defMPQComps[MPQ_FILETYPE_OGG], iniPath);
  WritePrivateProfileInt("Defaults", "other", defMPQComps[MPQ_FILETYPE_UNKNOWN], iniPath);
  WritePrivateProfileInt("Defaults", "wavcomp", defCompWAV, iniPath);
  
  if(skinFormBG != SKIN_FORMBG){
    sprintf(num, "0x%6X", skinFormBG);
    WritePrivateProfileString("Skin", "FormBG", num, iniPath);
  }
  if(skinFormText != SKIN_FORMTEXT){
    sprintf(num, "0x%6X", skinFormText);
    WritePrivateProfileString("Skin", "FormFG", num, iniPath);
  }
  if(skinInputBG != SKIN_INPUTBG){
    sprintf(num, "0x%6X", skinInputBG);
    WritePrivateProfileString("Skin", "InputBG", num, iniPath);
  }
  if(skinInputText != SKIN_INPUTTEXT){
    sprintf(num, "0x%6X", skinInputText);
    WritePrivateProfileString("Skin", "InputFG", num, iniPath);
  }
  
  WritePrivateProfileString("Listfiles", NULL, NULL, iniPath);
  WritePrivateProfileInt("Listfiles", "Count", listfileCount, iniPath);
  for(i = 0; i < listfileCount; i++){
    sprintf(num, "%d", i);
    WritePrivateProfileString("Listfiles", num, listfileList[i], iniPath);
  }
  
  generateListfiles();
}

void exitSaveOptions(){
  remove(defaultListfile);
  if(optSaveLast){
    WritePrivateProfileInt("Defaults", "GameType", gameType, iniPath);
    WritePrivateProfileInt("Defaults", "Compatability", compatability, iniPath);
    WritePrivateProfileInt("Defaults", "DelName", delGameName, iniPath);
    WritePrivateProfileInt("Defaults", "TrimMTXM", trimMTXM, iniPath);
    WritePrivateProfileInt("Defaults", "DelSPActs", delSPActions, iniPath);
  }
  if(optSaveLastDir) WritePrivateProfileString("Settings", "StartDir", optStartDir, iniPath);
}

void copyStartDir(const char* path){
  if(!optSaveLastDir) return;
  int i;
  strcpy(optStartDir, path);
  for(i = strlen(optStartDir); i >= 0 && optStartDir[i] != '\\'; i--);
  optStartDir[i] = 0;
}


u32 getDefaultMPQFlags(u32 filetype){
  if(defMPQComps[filetype] == MPQ_COMP_DO_NOT_ADD){
    return MPQ_FLAGS_DO_NOT_ADD;
  }
  u32 wavComp = 0;
  
  if(filetype == MPQ_FILETYPE_WAV){
    wavComp = defCompWAV * MPQ_FLAGS_WAV_COMP;
    if(defCompWAV != COMPWAV_CONV){
      // wav or compressed wav
      switch(defMPQComps[MPQ_FILETYPE_WAV]){
        case MPQ_COMP_NONE:
          return wavComp;
        case MPQ_COMP_WAV_LOW:
          return wavComp | MPQ_FLAGS_WAV_LOW;
        case MPQ_COMP_WAV_MED:
          return wavComp | MPQ_FLAGS_WAV_MED;
        case MPQ_COMP_WAV_HIGH:
          return wavComp | MPQ_FLAGS_WAV_HIGH;
        default:
          return MPQ_FLAGS_DO_NOT_ADD;
      }
    }
  }
  
  // non-wav, or converted wav
  switch(defMPQComps[filetype]){
    case MPQ_COMP_NONE:
      return wavComp;
    case MPQ_COMP_TYPE1:
      return wavComp | MPQ_FLAGS_FILE_TYPE1;
    case MPQ_COMP_TYPE2:
      return wavComp | MPQ_FLAGS_FILE_TYPE2;
    default:
      return wavComp | MPQ_FLAGS_DO_NOT_ADD;
  }
}

void generateListfiles(){
  u32 i;
  u32 size;
  u32 bufferSize = strlen(defaultListfile) + 1;
  for(i = 0; i < listfileCount; i++){
    bufferSize += strlen(listfileList[i]) + 1;
  }
  
  if(listfiles != NULL) free(listfiles);
  listfiles = malloc(bufferSize);
  strcpy(listfiles, defaultListfile);
  size = strlen(defaultListfile);
  for(i = 0; i < listfileCount; i++){
    listfiles[size] = '\n';
    size++;
    strcpy(listfiles + size, listfileList[i]);
    size += strlen(listfileList[i]);
  }
}


BOOL WritePrivateProfileInt(char* lpAppName, char* lpKeyName, int nValue, char* lpFileName){
  char out[16];
  sprintf(out, "%d", nValue);
  return WritePrivateProfileString(lpAppName, lpKeyName, out, lpFileName);
}
