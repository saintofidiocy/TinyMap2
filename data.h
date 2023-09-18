//#define DATA_EMBED
#ifdef DATA_EMBED
#define DATA_MPQ               ".\\TinyMap2.exe"
#else
#define DATA_MPQ               ".\\data.mpq"
#endif


// Globals
extern UNITSDAT unitsdat;
extern WEAPONSDAT weaponsdat;
extern UPGRADESDAT upgradesdat;
extern TECHDATADAT techdatadat;
extern VCOD refVCOD;
extern u16* tileData[8];
extern int tileDataSize[8];

// Functions
bool loadDataMPQ();
void unloadDataMPQ();
u32 loadMPQFile(const char filename[], void** buffer);
bool loadDatFile(const char* filename, void* buffer, u32 size);
