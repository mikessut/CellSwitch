#include <SoftwareSerial.h>
#include "Buffer.h"



SoftwareSerial cell(7,8);  //Create a 'fake' serial port. Pin 2 is the Rx pin, pin 3 is the Tx pin.

int onModulePin= 9;
int switchPin = 10;






//char buffer[BUF_LEN+1];  // leave room for \0
//int16_t bufPtr = -1;

Buffer buf(&Serial);
//Buffer buf(&cell);

void setup() {


  Serial.begin(9600);
  cell.begin(9600);

  Serial.println(F("Starting CellSwitch"));

  pinMode(switchPin,OUTPUT);
  digitalWrite(switchPin,HIGH);

  //memset(buffer, '\0', BUF_LEN+1);    // Initialice the string
  //buf = Buffer(Serial);

  //Serial.println(buffer);
  //Serial.println(F("Powering on cell"));
  //power_on();


}

void loop() {

  char *p;

  //  if (Serial.available() > 0 ) {
  //buf.pushBuf(Serial.read());
    //Serial.println(buffer);
  //}

  buf.read();

  if (buf.matchShift("+CMTI:\"SM\",")) {

    // now read until we have \r\n
    while (!buf.matchCRLF() ) {
      buf.read();
    }

    uint8_t msgID;
    char tmp[10];
    Serial.println(buf.getStr());
    sscanf(buf.getStr(),"%d",&msgID);

    Serial.println("SMS received!");
    sprintf(tmp,"id: %d",msgID);
    Serial.println(tmp);
  } else if (buf.matchShift("ON")) {
    digitalWrite(switchPin,LOW);
    Serial.println("ON CMD");
  } else if (buf.matchShift("OFF")) {
    digitalWrite(switchPin,HIGH);
    Serial.println("OFF CMD");
  }
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
    
}

int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout){

    uint8_t x=0,  answer=0;
    char response[100];
    unsigned long previous;

    memset(response, '\0', 100);    // Initialice the string
    
    delay(100);
    
    while( cell.available() > 0) cell.read();    // Clean the input buffer
    
    cell.println(ATcommand);    // Send the AT command 


    x = 0;
    previous = millis();

    // this loop waits for the answer
    do{
        // if there are data in the UART input buffer, reads it and checks for the asnwer
        if(cell.available() != 0){    
            response[x] = cell.read();
            x++;
            // check if the desired answer is in the response of the module
            if (strstr(response, expected_answer) != NULL)    
            {
                answer = 1;
            }
        }
    // Waits for the asnwer with time out
    }while((answer == 0) && ((millis() - previous) < timeout));    

    return answer;
}


