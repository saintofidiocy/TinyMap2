#include "types.h"
#include "options.h"
#include "data.h"

int main(int argc, char *argv[]){
  if(!loadDataMPQ()){
    dispError("Could not load Data.mpq.", argc == 1);
    return 0;
  }
  loadOptions();
  puts(listfiles);
  
  // do command-line
  
  showWindow();
  
  exitSaveOptions();
  unloadDataMPQ();
  return 0;
}
