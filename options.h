#ifndef H_OPTIONS
#define H_OPTIONS
#include "types.h"

// Settings
extern bool optSaveLast;
extern bool optAutoDetect;
extern bool optSaveLastDir;
extern char optStartDir[MAX_PATH];
extern u32 optMPQBlockSize;

// Defaults
extern u32 defGameType;
extern u32 defCompat;
extern bool defDelName;
extern bool defTrimMTXM;
extern bool defDelSPActs;
extern u32 defMPQComps[MPQ_FILETYPE_COUNT];
extern u32 defCompWAV;

// Skin
extern u32 skinFormBG;
extern u32 skinFormText;
extern u32 skinInputBG;
extern u32 skinInputText;

// Listfiles
extern char defaultListfile[MAX_PATH];
extern char* listfiles;
extern char** listfileList;
extern u32 listfileCount;

void loadOptions();
void saveOptions();
void exitSaveOptions();
void copyStartDir(const char path[]);
u32 getDefaultMPQFlags(u32 filetype);

#endif
