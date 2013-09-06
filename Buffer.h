#include <string.h>

#define BUF_LEN 100

class Buffer {
public:

  Buffer();

  void pushBuf(char c);

  void shiftBuffer(int x);
  void shiftBuffer();

  char * getStr();

  bool matchShift(char *str);

  char* matchCRLF(); 

private:
  char buffer[BUF_LEN+1];
  int bufPtr;


};
