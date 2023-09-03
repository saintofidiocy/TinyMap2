#include "types.h"

#define SECT_fmt  FOURCC('f','m','t',' ')
#define SECT_data FOURCC('d','a','t','a')

#define TYPE_RIFF FOURCC('R','I','F','F')
#define TYPE_WAVE FOURCC('W','A','V','E')
#define TYPE_OGG  FOURCC('O','g','g','S')

#define WAV_MONO    1
#define WAV_STEREO  2

u32 getWAVChannels(u8* buffer, u32 size);
bool isOGG(u8* buffer, u32 size);

u8* compWAV(u8* buffer);
