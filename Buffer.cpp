

#include "Buffer.h"

Buffer::Buffer(Stream *ser) {
  memset(buffer,'\0',BUF_LEN+1);
  bufPtr = -1;

  this->ser = ser;
}

void Buffer::pushBuf(char c) {

  bufPtr++;
  if (bufPtr == BUF_LEN) {
    // copy everything over a position
    shiftBuffer();    
  }
  buffer[bufPtr] = c; 
}

void Buffer::shiftBuffer(int x) {
  for (int i=0; i < x; i++)
    shiftBuffer();
}

void Buffer::shiftBuffer() {
  // always stays null terminated
  if (bufPtr >= 0 ) {
    for (int i = 1; i < BUF_LEN+1; i++) {
      buffer[i-1] = buffer[i];
    }
    bufPtr--;
  }
}

char * Buffer::getStr() {
  return buffer;
}

bool Buffer::matchShift(char *str) {

  char * p;

  if ( (p = strstr(buffer,str)) != NULL ) {
    shiftBuffer((p-buffer) + strlen(str));
    return true;
  } else {
    return false;
  }

}

char* Buffer::matchCRLF() {
  return strstr(buffer,"\r\n");

}

bool Buffer::read() {
  if (ser->available() > 0 ) {
    pushBuf(ser->read());
    return true;
  }
  return false;
}

