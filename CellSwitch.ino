#include <SoftwareSerial.h>
#include "Buffer.h"
//#include <string.h>



SoftwareSerial cell(7,8);  //Create a 'fake' serial port. Pin 2 is the Rx pin, pin 3 is the Tx pin.

int onModulePin= 9;
int switchPin = 10;






//char buffer[BUF_LEN+1];  // leave room for \0
//int16_t bufPtr = -1;

//Buffer buf(&Serial);
Buffer buf(&cell);

void setup() {

  char tmp[100];

  Serial.begin(9600);
  cell.begin(9600);

  Serial.println(F("Starting CellSwitch"));

  pinMode(switchPin,OUTPUT);
  digitalWrite(switchPin,HIGH);

  //memset(buffer, '\0', BUF_LEN+1);    // Initialice the string
  //buf = Buffer(Serial);

  //Serial.println(buffer);
  //Serial.println(F("Powering on cell"));
  power_on();
  Serial.println("Power On Complete");


  readSMS(2,tmp,100);
  Serial.println("msg:");
  Serial.println(tmp);

}

void loop() {

  char *p;

  //  if (Serial.available() > 0 ) {
  //buf.pushBuf(Serial.read());
    //Serial.println(buffer);
  //}

  buf.read();

  if (buf.matchShift("+CMTI: \"SM\",")) {

    // now read until we have \r\n
    while (buf.matchCRLF() < 0) {
      buf.read();
    }

    uint8_t msgID;
    char tmp[30];
    Serial.println(buf.getStr());
    sscanf(buf.getStr(),"%d",&msgID);

    Serial.println("SMS received!");
    sprintf(tmp,"id: %d",msgID);
    Serial.println(tmp);
    
    readSMS(msgID,tmp,30);
    if (strcmp(tmp,"On") == 0) {
      turnOn();
    } else if (strcmp(tmp,"Off") == 0) {
      turnOff();
    }

  } else if (buf.matchShift("ON")) {
    turnOn();
  } else if (buf.matchShift("OFF")) {
    turnOff();
  }
}

void turnOn() {
  digitalWrite(switchPin,LOW);
  Serial.println("ON CMD");
}

void turnOff() {
  digitalWrite(switchPin,HIGH);
  Serial.println("OFF CMD");
}

void readSMS(int num, char * b, int maxLen) {
  char atcmd[13];
  int i,j;

  buf.flushStream();
  buf.empty();
  sprintf(atcmd,"AT+CMGR=%d",num);
	    
  //cell.println(atcmd);
  buf.println(atcmd);

  // read echo of commandread until \r\n and shift out

  while ( !buf.match("\r\n\r\n") ) {
    buf.read();
  }

  //Serial.println("0");
  Serial.println(buf.getStr());
  buf.empty();

  // read SMS reader (do nothing iwth it for now?)

  while ( (i = buf.matchCRLF()) < 0 ) {
  //while (buf.len() < 3) {
    buf.read();
  }

  //Serial.println("1");
  Serial.println(buf.getStr());
  buf.empty();

  // read SMS message and copyread to next \r\n

  while ( (i = buf.matchCRLF()) < 0 ) {
    buf.read();
  }

  //Serial.println("2");
  Serial.println(buf.getStr());

  for (j=0; (j < i-2) && (j < maxLen); j++) {  // disregard the last two chars (\r\n)
    b[j] = buf.getChar(j);
  }
  b[i-2] = '\0';


}

void power_on(){

    uint8_t answer=0;
    
    // checks if the module is started
    answer = sendATcommand("AT", "OK", 2000);
    if (answer == 0)
    {
        // power on pulse
        digitalWrite(onModulePin,HIGH);
        delay(3000);
        digitalWrite(onModulePin,LOW);
    
        // waits for an answer from the module
        while(answer == 0){     // Send AT every two seconds and wait for the answer
            answer = sendATcommand("AT", "OK", 2000);    
        }
    }

    while( (sendATcommand("AT+CREG?", "+CREG: 0,1", 500) || 
	  sendATcommand("AT+CREG?", "+CREG: 0,5", 500)) == 0 );
    
    sendATcommand("AT+CMGF=1","OK",5000);  // put SMS into text mode
}

int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout){

    uint8_t x=0,  answer=0;
    //char response[100];
    unsigned long previous;

    //memset(response, '\0', 100);    // Initialice the string
    
    delay(100);
    
    //while( cell.available() > 0) cell.read();    // Clean the input buffer
    
    buf.println(ATcommand);    // Send the AT command 


    x = 0;
    previous = millis();

    // this loop waits for the answer
    do{
      buf.read();
      if (buf.matchShift(expected_answer)) {
	answer = 1;
      }
        // if there are data in the UART input buffer, reads it and checks for the asnwer
        // if(cell.available() != 0){    
        //     response[x] = cell.read();
        //     x++;
        //     // check if the desired answer is in the response of the module
        //     if (strstr(response, expected_answer) != NULL)    
        //     {
        //         answer = 1;
        //     }
        // }
    // Waits for the asnwer with time out
    }while((answer == 0) && ((millis() - previous) < timeout));    

    buf.flushStream();
    return answer;
}

/*

+CMTI: "SM",1
 
at+cmgf=1  put into text mode

at+cmgr=2

+CMGR: "REC UNREAD","+16514423855","","13/09/14,06:55:39-24"
                  
On
                                                                            
    
       
OK

*/
