#include <string.h>
//#include <HardwareSerial.h>
#include <Stream.h>

#define BUF_LEN 100

class Buffer {
public:

  Buffer(Stream *ser);

  void pushBuf(char c);

  void shiftBuffer(int x);
  void shiftBuffer();

  char * getStr();

  bool matchShift(char *str);
  bool match(char *str);

  int matchCRLF(); 

  bool read() ;

  void empty();

  char getChar(int);

  void println(char*);
  int len();

  void flushStream();

private:
  char buffer[BUF_LEN+1];
  int bufPtr;
  Stream *ser;


};
