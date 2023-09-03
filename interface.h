#ifndef H_INTERFACE
#define H_INTERFACE

// Main dialog controls (Button, Option, Checkbox, Listbox, ComboBox)
#define MAIN_B_OPEN       0x0001
#define MAIN_B_OPTIONS    0x0002

#define MAIN_B_ADVOPTS    0x0101
#define MAIN_B_SAVE       0x0102
#define MAIN_B_STATS      0x0103
#define MAIN_O_MELEE      0x0111
#define MAIN_O_UMS        0x0112
#define MAIN_O_STARCRAFT  0x0121
#define MAIN_O_BROODWAR   0x0122
#define MAIN_O_EXTENDED   0x0123

#define MAIN_C_DELNAME    0x0201
#define MAIN_C_TRIMMTXM   0x0202
#define MAIN_C_DELSPACTS  0x0203
#define MAIN_L_FILES      0x0204
#define MAIN_C_COMPWAVS   0x0205
#define MAIN_CB_MPQCOMP   0x0206

// Option dialog controls
#define OPT_C_SAVEOPTS    0x1001
#define OPT_C_DETECT      0x1002
#define OPT_C_SAVEDIR     0x1003
#define OPT_E_DIR         0x1004
#define OPT_B_SELDIR      0x1005

#define OPT_E_BLOCKSIZE   0x1011
#define OPT_L_LISTFILES   0x1012
#define OPT_B_ADD         0x1013
#define OPT_B_DEL         0x1014

#define OPT_CB_GAMETYPE   0x1021
#define OPT_CB_COMPAT     0x1022
#define OPT_C_DELNAME     0x1023
#define OPT_C_TRIMMTXM    0x1024
#define OPT_C_DELSPACTS   0x1025
#define OPT_L_TYPES       0x1026
#define OPT_CB_MPQCOMP    0x1027
#define OPT_C_COMPWAVS    0x1028

#define OPT_B_SAVE        0x1031
#define OPT_B_CANCEL      0x1032

#define WND_STATE_INIT 0
#define WND_STATE_MAIN 1
#define WND_STATE_ADV  2
#define WND_STATE_STAT 3

void dispError(const char message[], bool alwaysShowBox);
bool windowIsOpen();
void showWindow();
void changeWndState(u32 newState);
void setCompatability();
void showFileSize(u32 filesize);
void lFilesClear();
void lFilesAdd(char* str, MapFile* file);

#endif

