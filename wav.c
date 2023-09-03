#include "wav.h"

u32 getWAVChannels(u8* buffer, u32 size){
  RIFF riff;
  wavhdrfmt srcfmt;
  CHKH section;
  u32 dataPtr;
  
  srcfmt.wFormatTag = 0;
  memcpy(&riff, buffer, sizeof(RIFF));
  dataPtr = sizeof(RIFF);
  if(size < (sizeof(RIFF) + sizeof(wavhdrfmt))) return 0;
  if(riff.ckID != TYPE_RIFF || riff.WAVEID != TYPE_WAVE) return 0;
  while(dataPtr + sizeof(CHKH) < (riff.cksize+sizeof(RIFF)) && dataPtr + sizeof(CHKH) < size) {
    memcpy(&section, &buffer[dataPtr], sizeof(CHKH));
    if(section.name == SECT_fmt){
      memcpy(&srcfmt, &buffer[dataPtr+sizeof(CHKH)], sizeof(wavhdrfmt));
      if(srcfmt.wFormatTag != 1) return 0;
      return srcfmt.nChannels;
    }
    dataPtr += section.size + sizeof(CHKH);
  }
  return 0;
}

bool isOGG(u8* buffer, u32 size){
  ogghdr* ogg;
  u32 offset = 0;
  u32 i;
  // is following the chunks necessary, or just do return *(u32*)(buffer) == TYPE_OGG; ?
  while(offset < size){
    ogg = (ogghdr*)(buffer + offset);
    if(ogg->signature != TYPE_OGG) return false;
    if((ogg->flags & 4) || ogg->segments == 0) return true; // end of stream
    offset += sizeof(ogghdr) + ogg->segments - 1;
    for(i = 0; i < ogg->segments; i++){
      offset += ogg->segTable[i];
    }
  }
  return false;
}



// should probably make this better
u8* compWAV(u8* buffer){
  RIFF riff;
  CHKH section;
  wavhdrfmt srcfmt;
  u32 i,j,dataPtr=0;
  u32 bytesPerSample;
  u32 channelProc;
  double newSample;
  u8 *outBuffer = NULL;
  u32 srcdatasize = 0;
  u8 *srcdata = NULL;
  wavhdrfmt dstfmt;
  u32 dstdatasize;
  u16 *dstdata = NULL;
  u32 samples;
  
  srcfmt.wFormatTag = 0;
  srcdatasize = 0;
  memcpy(&riff, buffer, sizeof(RIFF));
  dataPtr = sizeof(RIFF);
  if(riff.ckID != TYPE_RIFF || riff.WAVEID != TYPE_WAVE) return NULL;
  do{
    memcpy(&section, buffer + dataPtr, sizeof(CHKH));
    dataPtr += sizeof(CHKH);
    switch(section.name){
      case SECT_fmt:
        memcpy(&srcfmt, buffer + dataPtr, sizeof(wavhdrfmt));
        break;
      case SECT_data:
        if(srcdatasize != 0){
          // multiple data chunks is unsupported
          return NULL;
        }
        srcdata = buffer + dataPtr;
        srcdatasize = section.size;
        break;
      default:
        break;
    }
    dataPtr += section.size;
  } while(dataPtr < (riff.cksize + sizeof(RIFF)));
  
  if(srcfmt.wFormatTag != 1 || srcdatasize == 0){
    return NULL;
  }
  
  dstfmt.wFormatTag = 1;
  dstfmt.nChannels = 1;
  dstfmt.nSamplesPerSec = srcfmt.nSamplesPerSec;
  dstfmt.nAvgBytesPerSec = srcfmt.nSamplesPerSec*2;
  dstfmt.nBlockAlign = 2;
  dstfmt.wBitsPerSample = 16;
  samples = srcdatasize / srcfmt.nBlockAlign;
  bytesPerSample = srcfmt.nBlockAlign / srcfmt.nChannels;
  dstdatasize = samples * 2;
  dstdata = malloc(dstdatasize);
  for(i = 0; i < samples; i++){
    newSample = 0;
    for(j = 0; j < srcfmt.nChannels; j++){
      channelProc = 0;
      memcpy(&channelProc, &srcdata[i*srcfmt.nBlockAlign + j*bytesPerSample], bytesPerSample);
      switch(srcfmt.wBitsPerSample){
        case 4:
          channelProc -= 8;
          break;
        case 8:
          channelProc -= 128;
          break;
        case 16:
          if(channelProc & 0x8000) channelProc |= 0xFFFF0000;
          break;
        case 24:
          if(channelProc & 0x800000) channelProc |= 0xFF000000;
          break;
        default:
          return 0;
      }
      newSample += (signed int)channelProc;
    }
    newSample /= srcfmt.nChannels;
    if(srcfmt.wBitsPerSample == 8) newSample -= 128;
    dstdata[i] = (65536 * newSample) / pow(2,srcfmt.wBitsPerSample);
  }
  
  riff.cksize = 36+dstdatasize;
  outBuffer = malloc(riff.cksize + 8);
  memcpy(outBuffer, &riff, 12);
  section.name = SECT_fmt;
  section.size = 16;
  memcpy(outBuffer+12, &section, sizeof(CHKH));
  memcpy(outBuffer+20, &dstfmt, 16);
  section.name = SECT_data;
  section.size = dstdatasize;
  memcpy(outBuffer+36, &section, sizeof(CHKH));
  memcpy(outBuffer+44, dstdata, dstdatasize);
  free(dstdata);
  return outBuffer;
}
