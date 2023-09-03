#ifndef H_MAP
#define H_MAP

bool openMap(const char* filename, int filetype);
char* saveMap(const char* filename, u32 outType);
void closeMap();
void listFiles();

#endif
