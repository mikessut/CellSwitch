

#include "Buffer.h"

Buffer::Buffer(Stream *ser) {
  empty();
  //bufPtr = -1;

  this->ser = ser;
}

void Buffer::empty() {
  memset(buffer,'\0',BUF_LEN+1);
  bufPtr = -1;

}

void Buffer::pushBuf(char c) {

  bufPtr++;
  if (bufPtr == BUF_LEN) {
    // copy everything over a position
    shiftBuffer();    
  }
  if ( c != '\0' ) {
    buffer[bufPtr] = c; 
  } else {
    buffer[bufPtr] = '-'; 
  }
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

int Buffer::matchCRLF() {
  char *p;
  p = strstr(buffer,"\r\n") ;
  if (p != NULL)
    return p-buffer+2;
  else
    return -1;

}

bool Buffer::read() {
  if (ser->available() > 0 ) {
    pushBuf(ser->read());
    return true;
  }
  return false;
}


char Buffer::getChar(int i) {
  if (i < BUF_LEN ) {
    return buffer[i];
  } else {
    return NULL;
  }
}

void Buffer::println(char* str) {
  ser->println(str);
}

int Buffer::len() {
  return bufPtr+1;
}

void Buffer::flushStream() {
  while( ser->available() > 0 ) {
    ser->read();
  }
}
