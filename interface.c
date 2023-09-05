#include "types.h"
#include "interface.h"
#include "chk.h"
#include "map.h"
#include "options.h"
#include "SFmpq_static.h"
#include "resource.h"
#include <windows.h>
#include <shlobj.h>

#define CLASS_NAME "TinyMap2"

#define ADVANCED_OPEN   "Ad&vanced Options >>"
#define ADVANCED_CLOSE  "Ad&vanced Options <<"
#define STATS_OPEN      "S&tats >>"
#define STATS_CLOSE     "S&tats <<"

const RECT DIM_INIT = {0, 0, 160, 40};
const RECT DIM_MAIN = {0, 0, 160, 272};
const RECT DIM_ADV  = {0, 0, 424, 272};
const RECT DIM_STAT = {0, 0, 504, 272};
const RECT DIM_OPTS = {0, 0, 504, 344};
#define GROWTH_X 8
#define GROWTH_Y 4

HINSTANCE hInstance = NULL;
HFONT hFont = NULL;
HFONT hFontFixed = NULL;
HBRUSH hBGForm = NULL;
HBRUSH hBGInput = NULL;
HICON hIcon = NULL;

struct {
  HWND hwnd;
  
  HWND bOpen, bOptions;
  
  HWND sFilesize;
  HWND gCompOpts ,gCompType, gCompGame;
  HWND oMelee, oUMS;
  HWND oStarCraft, oBroodWar, oExtended;
  HWND bAdvOpts, bSave, bStats;
  
  HWND sMPQComp;
  HWND gAdvOpts, gFManage, gFProps;
  HWND cDelName, cTrimMTXM, cDelSPActs;
  HWND cCompWAVs;
  HWND cbMPQComp;
  HWND lFiles;
  
  HWND gStats, eStats;
  
  bool resizing;
  RECT current, target, border;
  int deltaX, deltaY;
  u32 state;
  u32 curState;
} mainwnd = {0};

struct {
  HWND hwnd;
  
  HWND gSettings;
  HWND cSaveOpts, cDetect, cSaveDir;
  HWND eDir, bSelDir;
  
  HWND gListfiles;
  HWND sBlocksize, eBlocksize;
  HWND lListfiles;
  HWND bAdd, bRemove;
  
  HWND gDefaults;
  HWND sGameType, cbGameType;
  HWND sCompat, cbCompat;
  HWND cDelName, cTrimMTXM, cDelSPActs;
  HWND gFiles;
  HWND lTypes;
  HWND sMPQComp;
  HWND cbMPQComp;
  HWND cCompWAVs;
  
  HWND bSave, bCancel;
  
  HWND gDivider, sCredits, sThanks;
} optwnd = {0};

// option values
u32 newMPQComps[MPQ_FILETYPE_COUNT];
u32 newCompWAV;


void showOptions();

bool initWindow();
bool initOptions();
HWND CreateControl(LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND parent, int id);
DWORD WINAPI resizeThread(LPVOID lpp);
LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);
void lFilesClick();
void cbMPQCompClick();
void cCompWAVsClick();


void dispError(const char message[], bool alwaysShowBox){
  puts(message);
  if(alwaysShowBox){
    MessageBox(NULL, message, "Error!", MB_ICONERROR);
  }
}

bool windowIsOpen(){
  return mainwnd.hwnd != NULL;
}

void showWindow(){
  MSG msg;
  if(initWindow()){
    while(GetMessage(&msg, NULL, 0, 0)){
      if(!IsDialogMessage(mainwnd.hwnd, &msg)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    }
  }else{
    dispError("Could not make window", true);
  }
  DeleteObject(hFont);
  DeleteObject(hBGForm);
  DeleteObject(hBGInput);
}

void changeWndState(u32 newState){
  if(mainwnd.state == newState) return;
  mainwnd.state = newState;
  if(!mainwnd.resizing){
    CreateThread(NULL, 0, &resizeThread, NULL, 0, NULL);
  }
  
  if(newState == WND_STATE_ADV){
    SetWindowText(mainwnd.bAdvOpts, ADVANCED_CLOSE);
  }else{
    SetWindowText(mainwnd.bAdvOpts, ADVANCED_OPEN);
  }
  if(newState == WND_STATE_STAT){
    SetWindowText(mainwnd.bStats, STATS_CLOSE);
  }else{
    SetWindowText(mainwnd.bStats, STATS_OPEN);
  }
}

// Determines valid compatability options, changing the selected option if necessary, and updates the window
void setCompatability(){
  bool enabled[3] = {true,true,true};
  
  // Determine compatability options based on loaded game type
  if(gameType == GAMETYPE_UMS){ // Previously determined
    switch(loadedGameType){
     case VER_STARCRAFT:
       enabled[COMPAT_BROODWAR] = false; // No Brood War sections exist in SC-only map
       break;
     case VER_BROODWAR:
     case VER_BROODWAR_RM:
       enabled[COMPAT_STARCRAFT] = false; // No SC sections exist in BW-only map
       enabled[COMPAT_HYBRID] = false;
       break;
    }
  }else{ // GAMETYPE_MELEE
    if(loadedGameType != VER_BROODWAR && loadedGameType != VER_BROODWAR_RM){
      enabled[COMPAT_BROODWAR] = false;
    }
  }
  
  // If the current option is unavailable, we must change it ....
  if(!enabled[compatability]){
    if(enabled[COMPAT_HYBRID]){
      compatability = COMPAT_HYBRID;
    }else if(enabled[COMPAT_BROODWAR]){
      compatability = COMPAT_BROODWAR;
    }else if(enabled[COMPAT_STARCRAFT]){
      compatability = COMPAT_STARCRAFT;
    }
  }
  
  if(windowIsOpen()){
    SendMessage(mainwnd.oUMS, BM_SETCHECK, gameType == GAMETYPE_UMS, 0);
    SendMessage(mainwnd.oMelee, BM_SETCHECK, gameType == GAMETYPE_MELEE, 0);
    
    SendMessage(mainwnd.oStarCraft, BM_SETCHECK, compatability == COMPAT_STARCRAFT, 0);
    SendMessage(mainwnd.oBroodWar, BM_SETCHECK, compatability == COMPAT_BROODWAR, 0);
    SendMessage(mainwnd.oExtended, BM_SETCHECK, compatability == COMPAT_HYBRID, 0);
    
    EnableWindow(mainwnd.oStarCraft, enabled[COMPAT_STARCRAFT]);
    EnableWindow(mainwnd.oBroodWar, enabled[COMPAT_BROODWAR]);
    EnableWindow(mainwnd.oExtended, enabled[COMPAT_HYBRID]);
  }
}

void showFileSize(u32 filesize){
  char str[32];
  sprintf(str, "Filesize:  %u", filesize);
  SetWindowText(mainwnd.sFilesize, str);
}





void bOpenMapClick(){
  char filename[MAX_PATH];
  OPENFILENAME ofn = {0};
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = NULL;
  ofn.lpstrFile = filename;
  ofn.lpstrFile[0] = '\0';
  ofn.nMaxFile = sizeof(filename);
  ofn.lpstrFilter = "Starcraft Scenarios (*.scm;*.scx)\0*.scm;*.scx\0Raw Scenario (*.chk)\0*.chk\0";
  ofn.nFilterIndex = 1;
  ofn.lpstrFileTitle = NULL;
  ofn.nMaxFileTitle = 0;
  ofn.lpstrInitialDir = optStartDir;
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
  if(GetOpenFileName(&ofn) != false){
    copyStartDir(filename);
    if(!openMap(filename, ofn.nFilterIndex)){
      dispError("Could not open map!", true);
      return;
    }
  }
}

void bSaveMapClick(){
  OPENFILENAME ofn = {0};
  char filename[MAX_PATH] = "";
    
  u32 outType = getOutputType();
  if(outType == 0){
    dispError("Incompatable map, game type, and compatability.", true);
    return;
  }
  
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = NULL;
  ofn.nFilterIndex = 1;
  ofn.lpstrFile = filename;
  ofn.nMaxFile = sizeof(filename);
  ofn.lpstrFileTitle = NULL;
  ofn.nMaxFileTitle = 0;
  ofn.lpstrInitialDir = optStartDir;
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_NOREADONLYRETURN | OFN_OVERWRITEPROMPT;
  switch(outType & ~MODE_REMASTERED){
   case MODE_MELEE_SIMPLE:
   case MODE_UMS_SIMPLESC:
   case MODE_UMS_SIMPLEEX:
   case MODE_UMS_STARCRAFT:
   case MODE_UMS_EXTENDED:
     if(outType & MODE_REMASTERED){
       ofn.lpstrFilter = "Starcraft Remastered Scenario (*.scm)\0*.scm\0";
     }else{
       ofn.lpstrFilter = "Starcraft Scenario (*.scm)\0*.scm\0";
     }
     ofn.lpstrDefExt = "scm";
     break;
   
   case MODE_MELEE_EXTENDED:
   case MODE_UMS_BROODWAR:
     if(outType & MODE_REMASTERED){
       ofn.lpstrFilter = "Starcraft Remastered Brood War Scenario (*.scx)\0*.scx\0";
     }else{
       ofn.lpstrFilter = "Starcraft Brood War Scenario (*.scx)\0*.scx\0";
     }
     ofn.lpstrDefExt = "scx";
     break;
  }
  
  if(GetSaveFileName(&ofn) != false){
    char* stats = saveMap(filename, outType);
    if(stats == NULL){
      dispError("Could not save map!", true);
      return;
    }
    puts(stats);
    SetWindowText(mainwnd.eStats, stats);
    changeWndState(WND_STATE_STAT);
    copyStartDir(filename);
    free(stats);
  }
}

void lFilesClear(){
  SendMessage(mainwnd.lFiles, LB_RESETCONTENT, 0, 0);
}

void lFilesAdd(char* str, MapFile* file){
  int id = SendMessage(mainwnd.lFiles, LB_ADDSTRING, 0, (LPARAM)str);
  SendMessage(mainwnd.lFiles, LB_SETITEMDATA, (WPARAM)id, (LPARAM)file);
}

void lFilesClick(){
  int i;
  char filePropStr[260];
  bool hasWAV = false;
  bool hasOther = false;
  u32 maskFlags = 0xFFFFFFFF;
  u32 usedFlags = 0; // If any of these bits are equal after checking, those properties are the same across all selected files
  bool compWav = false;
  bool dontCompWav = false; // Used to check indeterminate state of CompWAV
  MapFile* file = NULL;
  
  strcpy(filePropStr, "File Properties");
  int selCount = SendMessage(mainwnd.lFiles, LB_GETSELCOUNT, 0, 0);
  if(selCount == 0){
    SendMessage(mainwnd.cbMPQComp, CB_RESETCONTENT, 0, 0);
    EnableWindow(mainwnd.cbMPQComp, false);
    SendMessage(mainwnd.cCompWAVs, BM_SETCHECK, BST_UNCHECKED, 0);
    EnableWindow(mainwnd.cCompWAVs, false);
    SetWindowText(mainwnd.gFProps, filePropStr);
    return;
  }else if(selCount > 1){
    int* selList = malloc(selCount * sizeof(int));
    selCount = SendMessage(mainwnd.lFiles, LB_GETSELITEMS, (WPARAM)selCount, (LPARAM)selList);
    for(i = 0; i < selCount; i++){
      file = (MapFile*)SendMessage(mainwnd.lFiles, LB_GETITEMDATA, (WPARAM)selList[i], 0);
      if(file == NULL) continue;
      if(file->mpqFlags & MPQ_FLAGS_IS_WAV){
        hasWAV = true;
        if(file->mpqFlags & MPQ_FLAGS_WAV_COMP){
          compWav = true;
        }else{
          dontCompWav = true;
        }
      }else{
        hasOther = true;
      }
      maskFlags &= file->mpqFlags;
      usedFlags |= file->mpqFlags;
    }
    free(selList);
  }
  
  i = SendMessage(mainwnd.lFiles, LB_GETCURSEL, 0, 0);
  file = (MapFile*)SendMessage(mainwnd.lFiles, LB_GETITEMDATA, (WPARAM)i, 0);
  if(selCount == 1){
    if(file->mpqFlags & MPQ_FLAGS_IS_WAV){
      hasWAV = true;
      compWav = file->mpqFlags & MPQ_FLAGS_WAV_COMP;
    }else{
      hasOther = true;
    }
    maskFlags = usedFlags = file->mpqFlags;
  }
  EnableWindow(mainwnd.cbMPQComp, true);
  SendMessage(mainwnd.cbMPQComp, CB_RESETCONTENT, 0, 0);
  SendMessage(mainwnd.cbMPQComp, CB_ADDSTRING, 0, (LPARAM)"Do Not Add");
  SendMessage(mainwnd.cbMPQComp, CB_ADDSTRING, 0, (LPARAM)"None");
  
  if(hasWAV){
    if(!hasOther){ // Only WAVs selected
      SendMessage(mainwnd.cbMPQComp, CB_ADDSTRING, 0, (LPARAM)"Low");
      SendMessage(mainwnd.cbMPQComp, CB_ADDSTRING, 0, (LPARAM)"Medium");
      SendMessage(mainwnd.cbMPQComp, CB_ADDSTRING, 0, (LPARAM)"High");
    }
    EnableWindow(mainwnd.cCompWAVs, true);
    if(file->mpqFlags & MPQ_FLAGS_IS_WAV){ // Display WAV usage for most recently selected file
      sprintf(filePropStr, "File Properties (TRIG = %d, MBRF = %d)", file->wavUsage.trig, file->wavUsage.mbrf);
    }
    if(compWav){
      if(dontCompWav){ // Some do, some don't
        SendMessage(mainwnd.cCompWAVs, BM_SETCHECK, BST_INDETERMINATE, 0);
      }else{ // All do
        SendMessage(mainwnd.cCompWAVs, BM_SETCHECK, BST_CHECKED, 0);
      }
    }else{ // None do
      SendMessage(mainwnd.cCompWAVs, BM_SETCHECK, BST_UNCHECKED, 0);
    }
  }else{ // No WAVs are selected
    SendMessage(mainwnd.cbMPQComp, CB_ADDSTRING, 0, (LPARAM)"Type 1");
    SendMessage(mainwnd.cbMPQComp, CB_ADDSTRING, 0, (LPARAM)"Type 2");
    SendMessage(mainwnd.cCompWAVs, BM_SETCHECK, BST_UNCHECKED, 0);
    EnableWindow(mainwnd.cCompWAVs, false);
  }
  SetWindowText(mainwnd.gFProps, filePropStr);
  
  if(maskFlags & usedFlags & MPQ_FLAGS_DO_NOT_ADD){ // All files are Do Not Add
    SendMessage(mainwnd.cbMPQComp, CB_SETCURSEL, MPQ_COMP_DO_NOT_ADD, 0);
  }else if(maskFlags == usedFlags){
    switch(usedFlags & MPQ_FLAGS_COMP_MASK){
      case 0:
        SendMessage(mainwnd.cbMPQComp, CB_SETCURSEL, MPQ_COMP_NONE, 0);
        break;
      
      case MPQ_FLAGS_WAV_LOW:
        SendMessage(mainwnd.cbMPQComp, CB_SETCURSEL, MPQ_COMP_WAV_LOW, 0);
        break;
      case MPQ_FLAGS_WAV_MED:
        SendMessage(mainwnd.cbMPQComp, CB_SETCURSEL, MPQ_COMP_WAV_MED, 0);
        break;
      case MPQ_FLAGS_WAV_HIGH:
        SendMessage(mainwnd.cbMPQComp, CB_SETCURSEL, MPQ_COMP_WAV_HIGH, 0);
        break;
      
      case MPQ_FLAGS_FILE_TYPE1:
        SendMessage(mainwnd.cbMPQComp, CB_SETCURSEL, MPQ_COMP_TYPE1, 0);
        break;
      case MPQ_FLAGS_FILE_TYPE2:
        SendMessage(mainwnd.cbMPQComp, CB_SETCURSEL, MPQ_COMP_TYPE2, 0);
        break;
      
      default:
        dispError("Unknown MPQ flag state", true);
        break;
    }
  }
}

void cbMPQCompClick(){
  int i;
  int listCount = SendMessage(mainwnd.lFiles, LB_GETCOUNT, 0, 0);
  int newComp = SendMessage(mainwnd.cbMPQComp, CB_GETCURSEL, 0, 0);
  for(i = 0; i < listCount; i++){
    if(SendMessage(mainwnd.lFiles, LB_GETSEL, (WPARAM)i, 0)){
      MapFile* file = (MapFile*)SendMessage(mainwnd.lFiles, LB_GETITEMDATA, (WPARAM)i, 0);
      if(file == NULL) continue;
      file->mpqFlags &= MPQ_FLAGS_TYPE_MASK; // Clear all but the file type
      switch(newComp){
        case MPQ_COMP_DO_NOT_ADD:
          file->mpqFlags |= MPQ_FLAGS_DO_NOT_ADD;
          break;
        
        case MPQ_COMP_NONE:
          // Do nothing (already 0)
          break;
        
        case MPQ_COMP_WAV_LOW:
          if(file->mpqFlags & MPQ_FLAGS_IS_WAV){
            file->mpqFlags |= MPQ_FLAGS_WAV_LOW;
          }else{ // MPQ_COMP_TYPE1
            file->mpqFlags |= MPQ_FLAGS_FILE_TYPE1;
          }
          break;
        case MPQ_COMP_WAV_MED:
          if(file->mpqFlags & MPQ_FLAGS_IS_WAV){
            file->mpqFlags |= MPQ_FLAGS_WAV_MED;
          }else{ // MPQ_COMP_TYPE2
            file->mpqFlags |= MPQ_FLAGS_FILE_TYPE2;
          }
          break;
        case MPQ_COMP_WAV_HIGH:
          if(file->mpqFlags & MPQ_FLAGS_IS_WAV) {
            file->mpqFlags |= MPQ_FLAGS_WAV_HIGH;
          }
          break;
      }
    }
  }
}

// TODO: compress vs. convert
void cCompWAVsClick(){
  int i;
  MapFile* file = NULL;
  int listcount = SendMessage(mainwnd.lFiles, LB_GETCOUNT, 0, 0);
  bool newcomp = SendMessage(mainwnd.cCompWAVs, BM_GETCHECK, 0, 0) != BST_CHECKED; // It is now checked if it was unchecked or indeterminate
  if(newcomp){ // Apparently you can't have an indeterminate value unless it's a 3STATE, and AUTO3STATE will allow the user to select indeterminate ... So this has to be done manually
    SendMessage(mainwnd.cCompWAVs, BM_SETCHECK, BST_CHECKED, 0);
  }else{
    SendMessage(mainwnd.cCompWAVs, BM_SETCHECK, BST_UNCHECKED, 0);
  }
  for(i = 0; i < listcount; i++){
    if(SendMessage(mainwnd.lFiles, LB_GETSEL, (WPARAM)i, 0)){
      file = (MapFile*)SendMessage(mainwnd.lFiles, LB_GETITEMDATA, (WPARAM)i, 0);
      if(file == NULL) continue;
      if(file->mpqFlags & MPQ_FLAGS_IS_WAV){
        if(newcomp){
          file->mpqFlags |= MPQ_FLAGS_WAV_COMP;
        }else{
          file->mpqFlags &= ~MPQ_FLAGS_WAV_COMP;
        }
      }
    }
  }
}



void bOptSelDirClick(){
  char displayName[MAX_PATH];
  char newStartDir[MAX_PATH];
  BROWSEINFO bi;
  ITEMIDLIST* result;
  
  bi.hwndOwner = optwnd.hwnd;
  bi.pidlRoot = 0; // Old Dir
  bi.pszDisplayName = displayName;
  bi.lpszTitle = "Pick a Directory...";
  bi.ulFlags = 0;
  bi.lpfn = NULL;
  bi.lParam = 0;
  bi.iImage = 0;
  result = SHBrowseForFolder(&bi);
  
  if(!result) return;
  SHGetPathFromIDList(result, newStartDir);
  SetWindowText(optwnd.eDir, newStartDir);
}

void bOptAddClick(){
  OPENFILENAME ofn;
  char filename[MAX_PATH];
  int i;
  ZeroMemory(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = NULL;
  ofn.lpstrFile = filename;
  ofn.lpstrFile[0] = 0;
  ofn.nMaxFile = sizeof(filename);
  ofn.lpstrFilter = "Listfiles (*.lst;*.txt)\0*.lst;*.txt\0All Files (*.*)\0*.*\0";
  ofn.nFilterIndex = 1;
  ofn.lpstrFileTitle = NULL;
  ofn.nMaxFileTitle = 0;
  ofn.lpstrInitialDir = optStartDir;
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
  size_t result;
  if(GetOpenFileName(&ofn) != false){
    SendMessage(optwnd.lListfiles, LB_ADDSTRING, 0, (LPARAM)filename);
  }else{
    dispError("Could not open file!", true);
  }
}

void bOptRemoveClick(){
  int curSel = SendMessage(optwnd.lListfiles, LB_GETCURSEL, 0, 0);
  if(curSel == -1) return;
  SendMessage(optwnd.lListfiles, LB_DELETESTRING, (WPARAM)curSel, 0);
}


void lOptTypesClick(){
  int type = SendMessage(optwnd.lTypes, LB_GETCURSEL, 0, 0);
  if(type == -1){
    SendMessage(optwnd.cbMPQComp, CB_RESETCONTENT, 0, 0);
    SendMessage(optwnd.cCompWAVs, BM_SETCHECK, BST_UNCHECKED, 0);
    EnableWindow(optwnd.cbMPQComp, FALSE);
    EnableWindow(optwnd.cCompWAVs, FALSE);
    return;
  }
  EnableWindow(optwnd.cbMPQComp, TRUE);
  SendMessage(optwnd.cbMPQComp, CB_RESETCONTENT, 0, 0);
  SendMessage(optwnd.cbMPQComp, CB_ADDSTRING, 0, (LPARAM)"Do Not Add");
  SendMessage(optwnd.cbMPQComp, CB_ADDSTRING, 0, (LPARAM)"None");
  if(type == MPQ_FILETYPE_WAV){
    SendMessage(optwnd.cbMPQComp, CB_ADDSTRING, 0, (LPARAM)"Low");
    SendMessage(optwnd.cbMPQComp, CB_ADDSTRING, 0, (LPARAM)"Medium");
    SendMessage(optwnd.cbMPQComp, CB_ADDSTRING, 0, (LPARAM)"High");
    EnableWindow(optwnd.cCompWAVs, TRUE);
    if(newCompWAV){
      SendMessage(optwnd.cCompWAVs, BM_SETCHECK, BST_CHECKED, 0);
    }else{
      SendMessage(optwnd.cCompWAVs, BM_SETCHECK, BST_UNCHECKED, 0);
    }
  }else{
    SendMessage(optwnd.cbMPQComp, CB_ADDSTRING, 0, (LPARAM)"Type 1");
    SendMessage(optwnd.cbMPQComp, CB_ADDSTRING, 0, (LPARAM)"Type 2");
    SendMessage(optwnd.cCompWAVs, BM_SETCHECK, BST_UNCHECKED, 0);
    EnableWindow(optwnd.cCompWAVs, FALSE);
  }
  SendMessage(optwnd.cbMPQComp, CB_SETCURSEL, (WPARAM)newMPQComps[type], 0);
}

void cbOptMPQCompClick(){
  int type = SendMessage(optwnd.lTypes, LB_GETCURSEL, 0, 0);
  if(type == -1) return;
  newMPQComps[type] = SendMessage(optwnd.cbMPQComp, CB_GETCURSEL, 0, 0);
}

void bOptSaveClick(){
  char filename[MAX_PATH];
  char num[16];
  int i;
  int size;
  
  optSaveLast = SendMessage(optwnd.cSaveOpts, BM_GETCHECK, 0, 0) == BST_CHECKED ? 1 : 0;
  optAutoDetect = SendMessage(optwnd.cDetect, BM_GETCHECK, 0, 0) == BST_CHECKED ? 1 : 0;
  optSaveLastDir = SendMessage(optwnd.cSaveDir, BM_GETCHECK, 0, 0) == BST_CHECKED ? 1 : 0;
  GetWindowTextA(optwnd.eDir, optStartDir, MAX_PATH);
  GetWindowTextA(optwnd.eBlocksize, num, sizeof(num));
  optMPQBlockSize = atoi(num);
  
  defGameType = SendMessage(optwnd.cbGameType, CB_GETCURSEL, 0, 0);
  defCompat = SendMessage(optwnd.cbCompat, CB_GETCURSEL, 0, 0);
  defDelName = SendMessage(optwnd.cDelName, BM_GETCHECK, 0, 0) == BST_CHECKED ? 1 : 0;
  defTrimMTXM = SendMessage(optwnd.cTrimMTXM, BM_GETCHECK, 0, 0) == BST_CHECKED ? 1 : 0;
  defDelSPActs = SendMessage(optwnd.cDelSPActs, BM_GETCHECK, 0, 0) == BST_CHECKED ? 1 : 0;
  
  memcpy(defMPQComps, newMPQComps, sizeof(newMPQComps));
  defCompWAV = newCompWAV;
  
  for(i = 0; i < listfileCount; i++) free(listfileList[i]);
  free(listfileList);
  
  listfileCount = SendMessage(optwnd.lListfiles, LB_GETCOUNT, 0, 0);
  listfileList = malloc(listfileCount * sizeof(char*));
  for(i = 0; i < listfileCount; i++){
    SendMessage(optwnd.lListfiles, LB_GETTEXT, (WPARAM)i, (LPARAM)filename);
    size = strlen(filename) + 1;
    listfileList[i] = malloc(size);
    strcpy(listfileList[i], filename);
  }
  
  saveOptions();
}



void showOptions(){  
  EnableWindow(mainwnd.hwnd, FALSE);
  if(!initOptions()) return;
  MSG msg;
  while(GetMessage(&msg, NULL, 0, 0)){
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  EnableWindow(mainwnd.hwnd, TRUE);
  SetFocus(mainwnd.hwnd);
  DestroyWindow(optwnd.hwnd);
}

bool initWindow(){
  hInstance = GetModuleHandle(NULL);
  
  //NONCLIENTMETRICS ncm = {sizeof(NONCLIENTMETRICS), 0};
  //SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);
  //hFont = CreateFontIndirect(&ncm.lfSmCaptionFont);
  hFont = (HFONT)GetStockObject(ANSI_VAR_FONT);
  hFontFixed = (HFONT)GetStockObject(ANSI_FIXED_FONT);
  hBGForm = CreateSolidBrush(skinFormBG);
  hBGInput = CreateSolidBrush(skinInputBG);
  hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ICO_RES));
  
  WNDCLASSEX wincl = {sizeof(WNDCLASSEX), CS_DBLCLKS, WinProc, 0, 0, hInstance, hIcon, LoadCursor(NULL, IDC_ARROW), hBGForm, NULL, CLASS_NAME, hIcon}; 
  if(!RegisterClassEx(&wincl)) return false;
  
  AdjustWindowRect(&mainwnd.border, WS_CAPTION | WS_SYSMENU, 0);
  mainwnd.current.top = DIM_INIT.top + mainwnd.border.top;
  mainwnd.current.left = DIM_INIT.left + mainwnd.border.left;
  mainwnd.current.bottom = DIM_INIT.bottom + mainwnd.border.bottom;
  mainwnd.current.right = DIM_INIT.right + mainwnd.border.right;
  
  int x = (GetSystemMetrics(SM_CXSCREEN) - DIM_ADV.right)/2 + mainwnd.border.left;
  int y = (GetSystemMetrics(SM_CYSCREEN) - DIM_ADV.bottom)/2;
  int w = mainwnd.current.right - mainwnd.current.left;
  int h = mainwnd.current.bottom - mainwnd.current.top;
  mainwnd.hwnd = CreateWindowEx(0, CLASS_NAME, "TinyMap v" VER_STR, WS_CAPTION | WS_SYSMENU, x, y, w, h, HWND_DESKTOP, NULL, hInstance, NULL);
  
  // Init controls
  mainwnd.bOpen = CreateControl("BUTTON", "&Open Map", WS_GROUP | WS_VISIBLE | WS_TABSTOP | WS_CHILD | BS_PUSHBUTTON, 8, 8, 68, 25, mainwnd.hwnd, MAIN_B_OPEN);
  mainwnd.bOptions = CreateControl("BUTTON", "O&ptions", WS_VISIBLE | WS_TABSTOP | WS_CHILD | BS_PUSHBUTTON, 84, 8, 68, 25, mainwnd.hwnd, MAIN_B_OPTIONS);
  
  // Main controls
  mainwnd.sFilesize = CreateControl("STATIC", "", WS_CHILD, 8, 40, 144, 17, mainwnd.hwnd, 0);
  mainwnd.gCompOpts = CreateControl("BUTTON", "Compress Options", WS_CHILD | BS_GROUPBOX, 8, 56, 145, 177, mainwnd.hwnd, 0);
  mainwnd.gCompType = CreateControl("BUTTON", "Game Type", WS_GROUP | WS_CHILD | BS_GROUPBOX, 16, 72, 128, 57, mainwnd.hwnd, 0);
  mainwnd.oMelee = CreateControl("BUTTON", "&Melee", WS_TABSTOP | WS_CHILD | BS_AUTORADIOBUTTON, 24, 88, 112, 16, mainwnd.hwnd, MAIN_O_MELEE);
  mainwnd.oUMS = CreateControl("BUTTON", "&Use Map Settings", WS_CHILD | BS_AUTORADIOBUTTON, 24, 104, 112, 16, mainwnd.hwnd, MAIN_O_UMS);
  mainwnd.gCompGame = CreateControl("BUTTON", "Compatability", WS_GROUP | WS_CHILD | BS_GROUPBOX, 16, 129, 128, 73, mainwnd.hwnd, 0);
  mainwnd.oStarCraft = CreateControl("BUTTON", "StarCraft", WS_TABSTOP | WS_CHILD | BS_AUTORADIOBUTTON, 24, 145, 112, 16, mainwnd.hwnd, MAIN_O_STARCRAFT);
  mainwnd.oBroodWar = CreateControl("BUTTON", "Brood War", WS_CHILD | BS_AUTORADIOBUTTON, 24, 161, 112, 16, mainwnd.hwnd, MAIN_O_BROODWAR);
  mainwnd.oExtended = CreateControl("BUTTON", "Both", WS_CHILD | BS_AUTORADIOBUTTON, 24, 177, 112, 16, mainwnd.hwnd, MAIN_O_EXTENDED);
  mainwnd.bAdvOpts = CreateControl("BUTTON", ADVANCED_OPEN, WS_GROUP | WS_TABSTOP | WS_CHILD | BS_PUSHBUTTON, 16, 208, 128, 17, mainwnd.hwnd, MAIN_B_ADVOPTS);
  mainwnd.bSave = CreateControl("BUTTON", "&Save Map", WS_TABSTOP | WS_CHILD | BS_PUSHBUTTON, 8, 240, 68, 25, mainwnd.hwnd, MAIN_B_SAVE);
  mainwnd.bStats = CreateControl("BUTTON", STATS_OPEN, WS_TABSTOP | WS_CHILD | BS_PUSHBUTTON, 84, 240, 68, 25, mainwnd.hwnd, MAIN_B_STATS);
  
  // Advanced controls
  mainwnd.gAdvOpts = CreateControl("BUTTON", "Advanced Options", WS_GROUP | WS_CHILD | BS_GROUPBOX, 160, 8, 257, 257, mainwnd.hwnd, 0);
  mainwnd.cDelName = CreateControl("BUTTON", "Replace Game Name with Filename", WS_TABSTOP | WS_CHILD | BS_AUTOCHECKBOX, 168, 24, 241, 16, mainwnd.hwnd, MAIN_C_DELNAME);
  mainwnd.cTrimMTXM = CreateControl("BUTTON", "Remove Bottom Row of Terrain", WS_TABSTOP | WS_CHILD | BS_AUTOCHECKBOX, 168, 40, 241, 16, mainwnd.hwnd, MAIN_C_TRIMMTXM);
  mainwnd.cDelSPActs = CreateControl("BUTTON", "Remove Single Player Actions", WS_TABSTOP | WS_CHILD | BS_AUTOCHECKBOX, 168, 56, 241, 16, mainwnd.hwnd, MAIN_C_DELSPACTS);
  mainwnd.gFManage = CreateControl("BUTTON", "File Manager", WS_CHILD | BS_GROUPBOX, 168, 72, 241, 185, mainwnd.hwnd, 0);
  mainwnd.lFiles = CreateControl("LISTBOX", "", WS_TABSTOP | WS_CHILD | LBS_STANDARD | LBS_NOINTEGRALHEIGHT | LBS_HASSTRINGS | LBS_EXTENDEDSEL, 176, 88, 225, 93, mainwnd.hwnd, MAIN_L_FILES);
  mainwnd.gFProps = CreateControl("BUTTON", "File Properties", WS_GROUP | WS_CHILD | BS_GROUPBOX, 176, 184, 225, 65, mainwnd.hwnd, 0);
  mainwnd.sMPQComp = CreateControl("STATIC", "MPQ Compression:", WS_CHILD, 184, 202, 121, 17, mainwnd.hwnd, 0);
  mainwnd.cCompWAVs = CreateControl("BUTTON", "Compress WAV File", WS_TABSTOP | WS_CHILD | WS_DISABLED | BS_3STATE, 184, 225, 209, 16, mainwnd.hwnd, MAIN_C_COMPWAVS);
  mainwnd.cbMPQComp = CreateControl("COMBOBOX", "", WS_TABSTOP | WS_CHILD | WS_DISABLED | CBS_DROPDOWNLIST, 312, 200, 81, 120, mainwnd.hwnd, MAIN_CB_MPQCOMP);
  
  // Stats
  mainwnd.gStats = CreateControl("BUTTON", "Statistics", WS_GROUP | WS_CHILD | BS_GROUPBOX, 160, 8, 337, 257, mainwnd.hwnd, 0);
  mainwnd.eStats = CreateWindow("EDIT", "", WS_CHILD | WS_HSCROLL | WS_VSCROLL | ES_READONLY | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL, 168, 24, 321, 233, mainwnd.hwnd, NULL, hInstance, NULL);
  SendMessage(mainwnd.eStats, WM_SETFONT, (WPARAM)hFontFixed, (LPARAM)true);
  
  if(defDelName) SendMessage(mainwnd.cDelName, BM_SETCHECK, BST_CHECKED, 0);
  if(defTrimMTXM) SendMessage(mainwnd.cTrimMTXM, BM_SETCHECK, BST_CHECKED, 0);
  if(defDelSPActs) SendMessage(mainwnd.cDelSPActs, BM_SETCHECK, BST_CHECKED, 0);
  
  ShowWindow(mainwnd.hwnd, SW_SHOW);
  
  return true;
}

bool initOptions(){
  RECT rect = {0, 0, 0, 0};
  char number[16];
  MSG messages;
  int i;
   
  rect.top = DIM_OPTS.top + mainwnd.border.top;
  rect.left = DIM_OPTS.left + mainwnd.border.left;
  rect.bottom = DIM_OPTS.bottom + mainwnd.border.bottom;
  rect.right = DIM_OPTS.right + mainwnd.border.right;
  optwnd.hwnd = CreateWindowEx(0, CLASS_NAME, "Options", WS_SYSMENU | WS_CAPTION, (GetSystemMetrics(SM_CXSCREEN) - DIM_OPTS.right)/2 + mainwnd.border.left, (GetSystemMetrics(SM_CYSCREEN) - DIM_OPTS.bottom)/2 + mainwnd.border.top, rect.right - rect.left, rect.bottom - rect.top, mainwnd.hwnd, NULL, hInstance, NULL);
  
  optwnd.gSettings = CreateControl("BUTTON", "Settings", WS_VISIBLE | BS_GROUPBOX | WS_CHILD | WS_GROUP, 8, 8, 241, 89, optwnd.hwnd, 0);
  optwnd.cSaveOpts = CreateControl("BUTTON", "Save Last Selected Options as Default", WS_VISIBLE | BS_AUTOCHECKBOX | WS_CHILD | WS_TABSTOP, 16, 24, 225, 17, optwnd.hwnd, OPT_C_SAVEOPTS);
  optwnd.cDetect = CreateControl("BUTTON", "Auto-Detect Game Type and Compatability", WS_VISIBLE | BS_AUTOCHECKBOX | WS_CHILD | WS_TABSTOP, 16, 40, 225, 17, optwnd.hwnd, OPT_C_DETECT);
  optwnd.cSaveDir = CreateControl("BUTTON", "Save Last Directory", WS_VISIBLE | BS_AUTOCHECKBOX | WS_CHILD | WS_TABSTOP, 16, 56, 225, 17, optwnd.hwnd, OPT_C_SAVEDIR);
  optwnd.eDir = CreateControl("EDIT", "", WS_VISIBLE | ES_AUTOHSCROLL | WS_CHILD | WS_BORDER | WS_TABSTOP, 30, 72, 193, 19, optwnd.hwnd, OPT_E_DIR);
  optwnd.bSelDir = CreateControl("BUTTON", "...", WS_VISIBLE | BS_PUSHBUTTON | WS_CHILD | WS_TABSTOP, 223, 71, 16, 21, optwnd.hwnd, OPT_B_SELDIR);
  
  optwnd.gListfiles = CreateControl("BUTTON", "MPQ Options && Listfiles", WS_VISIBLE | BS_GROUPBOX | WS_CHILD | WS_GROUP, 8, 104, 241, 137, optwnd.hwnd, 0);
  optwnd.sBlocksize = CreateControl("STATIC", "Block Size Factor:", WS_VISIBLE | WS_CHILD, 16, 122, 128, 17, optwnd.hwnd, 0);
  optwnd.eBlocksize = CreateControl("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP | ES_NUMBER, 136, 120, 64, 19, optwnd.hwnd, OPT_E_BLOCKSIZE);
  optwnd.lListfiles = CreateControl("LISTBOX", "", WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY | WS_BORDER | LBS_HASSTRINGS | WS_CHILD | WS_TABSTOP, 16, 146, 225, 81, optwnd.hwnd, OPT_L_LISTFILES);
  optwnd.bAdd = CreateControl("BUTTON", "Add", WS_VISIBLE | BS_PUSHBUTTON | WS_CHILD | WS_TABSTOP, 40, 208, 65, 21, optwnd.hwnd, OPT_B_ADD);
  optwnd.bRemove = CreateControl("BUTTON", "Remove", WS_VISIBLE | BS_PUSHBUTTON | WS_CHILD | WS_TABSTOP, 152, 208, 65, 21, optwnd.hwnd, OPT_B_DEL);
  
  optwnd.gDefaults = CreateControl("BUTTON", "Defaults", WS_VISIBLE | BS_GROUPBOX | WS_CHILD | WS_GROUP, 256, 8, 241, 201, optwnd.hwnd, 0);
  optwnd.sGameType = CreateControl("STATIC", "Game Type:", WS_VISIBLE | WS_CHILD, 264, 26, 73, 17, optwnd.hwnd, 0);
  optwnd.cbGameType = CreateControl("COMBOBOX", "", WS_VISIBLE | CBS_DROPDOWNLIST | WS_CHILD | WS_TABSTOP, 360, 22, 121, 92, optwnd.hwnd, OPT_CB_GAMETYPE);
  optwnd.sCompat = CreateControl("STATIC", "Compatability:", WS_VISIBLE | WS_CHILD, 264, 50, 73, 17, optwnd.hwnd, 0);
  optwnd.cbCompat = CreateControl("COMBOBOX", "", WS_VISIBLE | CBS_DROPDOWNLIST | WS_CHILD | WS_TABSTOP, 360, 46, 121, 92, optwnd.hwnd, OPT_CB_COMPAT);
  optwnd.cDelName = CreateControl("BUTTON", "Replace Game Name with Filename", WS_VISIBLE | BS_AUTOCHECKBOX | WS_CHILD | WS_TABSTOP, 264, 70, 225, 17, optwnd.hwnd, OPT_C_DELNAME);
  optwnd.cTrimMTXM = CreateControl("BUTTON", "Remove Bottom Row of Terrain", WS_VISIBLE | BS_AUTOCHECKBOX | WS_CHILD | WS_TABSTOP, 264, 86, 225, 17, optwnd.hwnd, OPT_C_TRIMMTXM);
  optwnd.cDelSPActs = CreateControl("BUTTON", "Remove Single Player Actions", WS_VISIBLE | BS_AUTOCHECKBOX | WS_CHILD | WS_TABSTOP, 264, 102, 225, 17, optwnd.hwnd, OPT_C_DELSPACTS);
  optwnd.gFiles = CreateControl("BUTTON", "File Manager Defaults", WS_VISIBLE | BS_GROUPBOX | WS_CHILD, 264, 120, 225, 81, optwnd.hwnd, 0);
  optwnd.lTypes = CreateControl("LISTBOX", "", WS_VISIBLE | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT | WS_BORDER | WS_CHILD | WS_TABSTOP, 272, 138, 73, 67, optwnd.hwnd, OPT_L_TYPES);
  optwnd.sMPQComp = CreateControl("STATIC", "MPQ Compression:", WS_VISIBLE | WS_CHILD, 352, 136, 97, 17, optwnd.hwnd, 0);
  optwnd.cbMPQComp = CreateControl("COMBOBOX", "", WS_VISIBLE | CBS_DROPDOWNLIST | WS_CHILD | WS_DISABLED | WS_TABSTOP, 360, 152, 97, 92, optwnd.hwnd, OPT_CB_MPQCOMP);
  optwnd.cCompWAVs = CreateControl("BUTTON", "Compress WAV Files", WS_VISIBLE | BS_AUTOCHECKBOX | WS_CHILD | WS_DISABLED | WS_TABSTOP, 352, 176, 129, 17, optwnd.hwnd, OPT_C_COMPWAVS);
  
  optwnd.bSave = CreateControl("BUTTON", "Save", WS_VISIBLE | BS_PUSHBUTTON | WS_CHILD | WS_GROUP | WS_TABSTOP, 280, 216, 81, 25, optwnd.hwnd, OPT_B_SAVE);
  optwnd.bCancel = CreateControl("BUTTON", "Cancel", WS_VISIBLE | BS_PUSHBUTTON | WS_CHILD | WS_TABSTOP, 392, 216, 81, 25, optwnd.hwnd, OPT_B_CANCEL);
  
  optwnd.gDivider = CreateControl("BUTTON", "", WS_VISIBLE | BS_GROUPBOX | WS_CHILD | WS_GROUP, 8, 240, 489, 8, optwnd.hwnd, 0);
  optwnd.sCredits = CreateControl("STATIC", "TinyMap v" VER_STR "\t\xA9 2009-" VSTR_YEAR "\n"
                                            "  Programming and Design by:\n"
                                            "    O)FaRTy1billion\n"
                                            "  Compression Ideas by:\n"
                                            "    Heinermann && O)FaRTy1billion\n"
                                            "ShadowFlare MPQ API Library", WS_VISIBLE | WS_CHILD, 16, 256, 241, 81, optwnd.hwnd, 0);
  optwnd.sThanks = CreateControl("STATIC", "Special Thanks To:\n"
                                           "- Heinermann (Compression, etc.)\n"
                                           "- ShadowFlare (SFMPQAPI && a specific update ;) )\n"
                                           "- Corbo[MM] (Testing && ideas)\n"
                                           "- Falkoner (Testing)\t-Lanthanide (Testing)\n"
                                           "    And You for supporting this project!", WS_VISIBLE | WS_CHILD, 256, 256, 241, 81, optwnd.hwnd, 0);
  
  sprintf(number, "%d", optMPQBlockSize);
  SendMessage(optwnd.eBlocksize, WM_SETTEXT, 0, (LPARAM)number);
  for(i = 0; i < listfileCount; i++){
    SendMessage(optwnd.lListfiles, LB_ADDSTRING, 0, (LPARAM)listfileList[i]);
  }
  SendMessage(optwnd.cbGameType, CB_ADDSTRING, 0, (LPARAM)"Melee");
  SendMessage(optwnd.cbGameType, CB_ADDSTRING, 0, (LPARAM)"Use Map Settings");
  SendMessage(optwnd.cbCompat, CB_ADDSTRING, 0, (LPARAM)"StarCraft");
  SendMessage(optwnd.cbCompat, CB_ADDSTRING, 0, (LPARAM)"Brood War");
  SendMessage(optwnd.cbCompat, CB_ADDSTRING, 0, (LPARAM)"Both");
  SendMessage(optwnd.lTypes, LB_ADDSTRING, 0, (LPARAM)"(listfile)");
  SendMessage(optwnd.lTypes, LB_ADDSTRING, 0, (LPARAM)"CHK");
  SendMessage(optwnd.lTypes, LB_ADDSTRING, 0, (LPARAM)"WAV");
  SendMessage(optwnd.lTypes, LB_ADDSTRING, 0, (LPARAM)"OGG");
  SendMessage(optwnd.lTypes, LB_ADDSTRING, 0, (LPARAM)"Other File");
  
  if(optSaveLast) SendMessage(optwnd.cSaveOpts, BM_SETCHECK, BST_CHECKED, 0);
  if(optAutoDetect) SendMessage(optwnd.cDetect, BM_SETCHECK, BST_CHECKED, 0);
  if(optSaveLastDir) SendMessage(optwnd.cSaveDir, BM_SETCHECK, BST_CHECKED, 0);
  SetWindowText(optwnd.eDir, optStartDir);
  
  SendMessage(optwnd.cbGameType, CB_SETCURSEL, (WPARAM)defGameType, 0);
  SendMessage(optwnd.cbCompat, CB_SETCURSEL, (WPARAM)defCompat, 0);
  if(defDelName) SendMessage(optwnd.cDelName, BM_SETCHECK, BST_CHECKED, 0);
  if(defTrimMTXM) SendMessage(optwnd.cTrimMTXM, BM_SETCHECK, BST_CHECKED, 0);
  if(defDelSPActs) SendMessage(optwnd.cDelSPActs, BM_SETCHECK, BST_CHECKED, 0);
  
  // copy default options
  memcpy(newMPQComps, defMPQComps, sizeof(defMPQComps));
  newCompWAV = defCompWAV;
  
  ShowWindow(optwnd.hwnd, SW_SHOW);
  return true;
}

HWND CreateControl(LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND parent, int id){
  HWND hwnd = CreateWindow(lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, parent, (HMENU)id, hInstance, NULL);
  SendMessage(hwnd, WM_SETFONT, (WPARAM)hFont, (LPARAM)true);
  return hwnd;
}

DWORD WINAPI resizeThread(LPVOID lpp){
  mainwnd.resizing = true;
  
  if(mainwnd.curState == WND_STATE_INIT){
    ShowWindow(mainwnd.sFilesize, SW_SHOWNA);
    ShowWindow(mainwnd.bAdvOpts, SW_SHOWNA);
    ShowWindow(mainwnd.bSave, SW_SHOWNA);
    ShowWindow(mainwnd.bStats, SW_SHOWNA);
    ShowWindow(mainwnd.gCompOpts, SW_SHOWNA);
    ShowWindow(mainwnd.gCompType, SW_SHOWNA);
    ShowWindow(mainwnd.oMelee, SW_SHOWNA);
    ShowWindow(mainwnd.oUMS, SW_SHOWNA);
    ShowWindow(mainwnd.gCompGame, SW_SHOWNA);
    ShowWindow(mainwnd.oStarCraft, SW_SHOWNA);
    ShowWindow(mainwnd.oBroodWar, SW_SHOWNA);
    ShowWindow(mainwnd.oExtended, SW_SHOWNA);
  }
  
  while(mainwnd.curState != mainwnd.state) {
    if(mainwnd.curState != WND_STATE_MAIN){ // close before switching tabs
      mainwnd.curState = WND_STATE_MAIN;
    }else{
      mainwnd.curState = mainwnd.state;
    }
    
    switch(mainwnd.curState){
      case WND_STATE_INIT:
        mainwnd.target = DIM_INIT;
        break;
      case WND_STATE_MAIN:
      default:
        mainwnd.target = DIM_MAIN;
        break;
      case WND_STATE_ADV:
        mainwnd.target = DIM_ADV;
        ShowWindow(mainwnd.sMPQComp, SW_SHOWNA);
        ShowWindow(mainwnd.gAdvOpts, SW_SHOWNA);
        ShowWindow(mainwnd.gFManage, SW_SHOWNA);
        ShowWindow(mainwnd.gFProps, SW_SHOWNA);
        ShowWindow(mainwnd.cDelName, SW_SHOWNA);
        ShowWindow(mainwnd.cTrimMTXM, SW_SHOWNA);
        ShowWindow(mainwnd.cDelSPActs, SW_SHOWNA);
        ShowWindow(mainwnd.cCompWAVs, SW_SHOWNA);
        ShowWindow(mainwnd.cbMPQComp, SW_SHOWNA);
        ShowWindow(mainwnd.lFiles, SW_SHOWNA);
        break;
      case WND_STATE_STAT:
        mainwnd.target = DIM_STAT;
        ShowWindow(mainwnd.gStats, SW_SHOWNA);
        ShowWindow(mainwnd.eStats, SW_SHOWNA);
        break;
    }
    
    mainwnd.target.left += mainwnd.border.left;
    mainwnd.target.top += mainwnd.border.top;
    mainwnd.target.right += mainwnd.border.right;
    mainwnd.target.bottom += mainwnd.border.bottom;
    
    int curWidth = mainwnd.current.right - mainwnd.current.left;
    int curHeight = mainwnd.current.bottom - mainwnd.current.top;
    int targetWidth = mainwnd.target.right - mainwnd.target.left;
    int targetHeight = mainwnd.target.bottom - mainwnd.target.top;
    if(curWidth < targetWidth){
      mainwnd.deltaX = GROWTH_X;
    }else if(curWidth > targetWidth){
      mainwnd.deltaX = -GROWTH_X;
    }else{
      mainwnd.deltaX = 0;
    }
    if(curHeight < targetHeight){
      mainwnd.deltaY = GROWTH_Y;
    }else if(curHeight > targetHeight){
      mainwnd.deltaY = -GROWTH_Y;
    }else{
      mainwnd.deltaY = 0;
    }
    sleep(2);
    
    while(curWidth != targetWidth || curHeight != targetHeight){
      GetWindowRect(mainwnd.hwnd, &mainwnd.current);
      curWidth = mainwnd.current.right - mainwnd.current.left + mainwnd.deltaX;
      curHeight = mainwnd.current.bottom - mainwnd.current.top + mainwnd.deltaY;
      
      // prevent overshoot
      if((mainwnd.deltaX < 0 && curWidth < targetWidth) || (mainwnd.deltaX > 0 && curWidth > targetWidth)){
        mainwnd.deltaX = 0;
        curWidth = targetWidth;
      }
      if((mainwnd.deltaY < 0 && curHeight < targetHeight) || (mainwnd.deltaY > 0 && curHeight > targetHeight)){
        mainwnd.deltaY = 0;
        curHeight = targetHeight;
      }
      
      SetWindowPos(mainwnd.hwnd, NULL, 0, 0, curWidth, curHeight, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
      sleep(2);
    }
    
    // hide controls only after tab has closed
    if(mainwnd.curState == WND_STATE_MAIN){
      ShowWindow(mainwnd.sMPQComp, SW_HIDE);
      ShowWindow(mainwnd.gAdvOpts, SW_HIDE);
      ShowWindow(mainwnd.gFManage, SW_HIDE);
      ShowWindow(mainwnd.gFProps, SW_HIDE);
      ShowWindow(mainwnd.cDelName, SW_HIDE);
      ShowWindow(mainwnd.cTrimMTXM, SW_HIDE);
      ShowWindow(mainwnd.cDelSPActs, SW_HIDE);
      ShowWindow(mainwnd.cCompWAVs, SW_HIDE);
      ShowWindow(mainwnd.cbMPQComp, SW_HIDE);
      ShowWindow(mainwnd.lFiles, SW_HIDE);
      ShowWindow(mainwnd.gStats, SW_HIDE);
      ShowWindow(mainwnd.eStats, SW_HIDE);
    }
  }
  
  mainwnd.resizing = false;
  return 0;
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
  switch(msg){
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
    case WM_COMMAND:
      switch(HIWORD(wParam)){
        case BN_CLICKED:
        {
          switch(LOWORD(wParam)){
            case MAIN_B_OPEN:
              bOpenMapClick();
              return 0;
            case MAIN_B_OPTIONS:
              showOptions();
              return 0;
            case MAIN_B_SAVE:
              bSaveMapClick();
              return 0;
            
            case MAIN_B_ADVOPTS:
              if(mainwnd.state == WND_STATE_ADV){
                changeWndState(WND_STATE_MAIN);
              }else{
                changeWndState(WND_STATE_ADV);
              }
              return 0;
            case MAIN_B_STATS:
              if(mainwnd.state == WND_STATE_STAT){
                changeWndState(WND_STATE_MAIN);
              }else{
                changeWndState(WND_STATE_STAT);
              }
              return 0;
            
            case MAIN_O_MELEE:
              gameType = GAMETYPE_MELEE;
              setCompatability();
              return 0;
            case MAIN_O_UMS:
              gameType = GAMETYPE_UMS;
              setCompatability();
              return 0;
            
            case MAIN_O_STARCRAFT:
              compatability = COMPAT_STARCRAFT;
              return 0;
            case MAIN_O_BROODWAR:
              compatability = COMPAT_BROODWAR;
              return 0;
            case MAIN_O_EXTENDED:
              compatability = COMPAT_HYBRID;
              return 0;
            
            case MAIN_C_DELNAME:
              delGameName = (SendMessage(mainwnd.cDelName, BM_GETCHECK, 0, 0) == BST_CHECKED);
              return 0;
            case MAIN_C_TRIMMTXM:
              trimMTXM = (SendMessage(mainwnd.cTrimMTXM, BM_GETCHECK, 0, 0) == BST_CHECKED);
              return 0;
            case MAIN_C_DELSPACTS:
              delSPActions = (SendMessage(mainwnd.cDelSPActs, BM_GETCHECK, 0, 0) == BST_CHECKED);
              return 0;
            case MAIN_C_COMPWAVS:
              cCompWAVsClick();
              return 0;
            
            case OPT_C_SAVEOPTS:
            case OPT_C_DETECT:
            case OPT_C_SAVEDIR:
            case OPT_E_DIR:
            case OPT_E_BLOCKSIZE:
            case OPT_C_DELNAME:
            case OPT_C_TRIMMTXM:
            case OPT_C_DELSPACTS:
            case OPT_C_COMPWAVS:
              break;
            case OPT_B_ADD:
              bOptAddClick();
              return 0;
            case OPT_B_DEL:
              bOptRemoveClick();
              return 0;
            case OPT_B_SELDIR:
              bOptSelDirClick();
              return 0;
            
            case OPT_B_SAVE:
              bOptSaveClick();
            case OPT_B_CANCEL:
              DestroyWindow(optwnd.hwnd);
              return 0;
          }
          break;
        }
        case LBN_SELCHANGE: // CBN_SELCHANGE
          switch(LOWORD(wParam)){
            case MAIN_L_FILES:
              lFilesClick();
              return 0;
            case MAIN_CB_MPQCOMP:
              cbMPQCompClick();
              return 0;
            
            //case OPT_L_LISTFILES:
            //  return 0;
            //case OPT_CB_GAMETYPE:
            //  return 0;
            //case OPT_CB_COMPAT:
            //  return 0;
            case OPT_L_TYPES:
              lOptTypesClick();
              return 0;
            case OPT_CB_MPQCOMP:
              cbOptMPQCompClick();
              return 0;
          }
          break;
      }
      break;
    
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORBTN:
      SetBkColor((HDC)wParam, skinFormBG);
      SetTextColor((HDC)wParam, skinFormText);
      return (INT_PTR)hBGForm;
    
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLOREDIT:
      SetBkColor((HDC)wParam, skinInputBG);
      SetTextColor((HDC)wParam, skinInputText);
      return (INT_PTR)hBGInput;
    
    default:
      break;
  }
  return DefWindowProc(hwnd, msg, wParam, lParam);
}
