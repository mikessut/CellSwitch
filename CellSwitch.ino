#include <SoftwareSerial.h>



SoftwareSerial cell(7,8);  //Create a 'fake' serial port. Pin 2 is the Rx pin, pin 3 is the Tx pin.

int onModulePin= 9;

#define BUF_LEN 100
char buffer[BUF_LEN+1];  // leave room for \0
int16_t bufPtr = -1;

void setup() {

  Serial.begin(9600);
  cell.begin(9600);

  Serial.println(F("Starting CellSwitch"));

  memset(buffer, '\0', BUF_LEN+1);    // Initialice the string

  Serial.println(buffer);
  //Serial.println(F("Powering on cell"));
  //power_on();


}

void loop() {

  char *p;

  if (Serial.available() > 0 ) {
    pushBuf(Serial.read());
    //Serial.println(buffer);
  }

  if ((p = strstr(buffer,"+CMTI:\"SM\",")) != NULL ) {

    // shift out of buffer
    shiftBuffer((p-buffer)+11);

    // now read until we have \r\n
    while ( (p = strstr(buffer,"\r\n")) == NULL) {
	if (Serial.available() > 0 ) 
	  pushBuf(Serial.read());
    }

    uint8_t msgID;
    char tmp[10];
    Serial.println(buffer);
    sscanf(buffer,"%d",&msgID);

    Serial.println("SMS received!");
    sprintf(tmp,"id: %d",msgID);
    Serial.println(tmp);
  }
}

void pushBuf(char c) {

  bufPtr++;
  if (bufPtr == BUF_LEN) {
    // copy everything over a position
    shiftBuffer();    
  }
  buffer[bufPtr] = c; 
}

void shiftBuffer(int x) {
  for (int i=0; i < x; i++)
    shiftBuffer();
}

void shiftBuffer() {
  // always stays null terminated
  if (bufPtr >= 0 ) {
    for (int i = 1; i < BUF_LEN+1; i++) {
      buffer[i-1] = buffer[i];
    }
    bufPtr--;
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