#include <Arduino.h>
#include "CommunicationManager.h"
#include "SerialServoController.h"
CommunicationManager::CommunicationManager(){

}
void CommunicationManager::begin(){
  Serial.print("<Arm ready!>");
}
void CommunicationManager::loop(){
  getDataFromPC();
}
void CommunicationManager::getDataFromPC() {
    // receive data from PC and save it into inputBuffer
  if(Serial.available() > 0) {
    char x = (char)Serial.read();
      // the order of these IF clauses is significant
    if (x == endMarker) {
      readInProgress = false;
      newDataFromPC = true;
      inputBuffer[bytesRecvd] = 0;
      parseData();
    }
    
    if(readInProgress) {
      inputBuffer[bytesRecvd] = x;
      Serial.print("<Received byte: ");
      Serial.print(x);
      Serial.print(">");
      bytesRecvd ++;
      if (bytesRecvd == BUFF_SIZE) {
        bytesRecvd = BUFF_SIZE - 1;
      }
    }

    if (x == startMarker) { 
      bytesRecvd = 0; 
      readInProgress = true;
      Serial.print("<Received start byte>");
    }
  }
}
void CommunicationManager::parseData() {
  // split the data into its parts
  char * strtokIndx; // this is used by strtok() as an index
  
  strtokIndx = strtok(inputBuffer,",");      // get the first part - the string
  lastCommand[0] = atoi(strtokIndx);     // convert this part to an int

  strtokIndx = strtok(NULL,",");      // get the first part - the string
  lastCommand[1] = atoi(strtokIndx);     // convert this part to an int

  strtokIndx = strtok(NULL,",");      // get the first part - the string
  lastCommand[2] = atoi(strtokIndx);     // convert this part to an int

  strtokIndx = strtok(NULL,",");      // get the first part - the string
  lastCommand[3] = atoi(strtokIndx);     // convert this part to an int

  strtokIndx = strtok(NULL,",");      // get the first part - the string
  lastCommand[4] = atoi(strtokIndx);     // convert this part to an int

  strtokIndx = strtok(NULL,",");      // get the first part - the string
  lastCommand[5] = atoi(strtokIndx);     // convert this part to an int

}

void CommunicationManager::registerServoController(SerialServoController* servos){
  servoController = servos;
}