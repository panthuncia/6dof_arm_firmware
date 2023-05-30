#include "CommunicationManager.h"

#include <Arduino.h>

#include "SerialServoController.h"
CommunicationManager::CommunicationManager() {
}
void CommunicationManager::begin() {
    Serial.print("<Arm ready!>");
}
void CommunicationManager::loop() {
    getDataFromPC();
    parseData();
}
void CommunicationManager::getDataFromPC() {
    // receive data from PC and save it into inputBuffer
    bool isFirstByte = false;
    char* currentInputBuffer = moveCommandBuffer;
    byte bytesRecvd = 0;
    boolean readInProgress = false;
    int commandsRead = 0;
    while (usb_serial_available() > 0) {
        char x = (char)usb_serial_getchar();
        // the order of these IF clauses is significant
        if (x == endMarker) {
            readInProgress = false;
            currentInputBuffer[bytesRecvd] = ',';
            //Serial.print("<Received end byte>");
            commandsRead+=1;
            //loop after 100 commands
            if(commandsRead>99)
                break;
        }

        if (readInProgress) {
            if(isFirstByte){
                isFirstByte = false;
                //Serial.println("Got first byte");
                //Serial.println(x);
                if(x=='0'){
                    //Serial.println("was move command");
                    currentInputBuffer = moveCommandBuffer;
                    processMoveCommand = true;
                }
                else if (x=='1'){
                    processPrintCommand = true;
                    currentInputBuffer = printCommandBuffer;
                }
            }
            currentInputBuffer[bytesRecvd] = x;
            //Serial.print("Received byte: ");
            //Serial.println(x);
            bytesRecvd++;
            if (bytesRecvd == BUFF_SIZE) {
                bytesRecvd = BUFF_SIZE - 1;
            }
        }

        if (x == startMarker) {
            
            bytesRecvd = 0;
            readInProgress = true;
            isFirstByte = true;
            //Serial.print("<Received start byte>");
        }
    }
}
void CommunicationManager::parseData() {
    // split the data into its parts
    if(processMoveCommand){
        //Serial.println("In process move command");
        processMoveCommand = false;
        char* inputBuffer = moveCommandBuffer;
        char* strtokIndx;  // this is used by strtok() as an index
        strtokIndx = strtok(inputBuffer, ",");  // get the first part - the string
        uint8_t command = atoi(strtokIndx);
        strtokIndx = strtok(NULL, ",");
        lastCommand[0] = atoi(strtokIndx);

        strtokIndx = strtok(NULL, ",");
        lastCommand[1] = atoi(strtokIndx);

        strtokIndx = strtok(NULL, ",");
        lastCommand[2] = atoi(strtokIndx);

        strtokIndx = strtok(NULL, ",");
        lastCommand[3] = atoi(strtokIndx);

        strtokIndx = strtok(NULL, ",");
        lastCommand[4] = atoi(strtokIndx);

        strtokIndx = strtok(NULL, ",");
        lastCommand[5] = atoi(strtokIndx);

        newDataFromPC = true;
        // Serial.print("lastCommand: ");
        // Serial.print(lastCommand[0]);
        // Serial.print(lastCommand[1]);
        // Serial.print(lastCommand[2]);
        // Serial.print(lastCommand[3]);
        // Serial.print(lastCommand[4]);
        // Serial.print(lastCommand[5]);
        // Serial.println("");
    }
    if (processPrintCommand){           
        processPrintCommand = false; 
        Serial.println("<1," + String(servoController->latestPositions.positions[0]) + "," + 
            String(servoController->latestPositions.positions[1]) + "," + String(servoController->latestPositions.positions[2]) 
            + "," + String(servoController->latestPositions.positions[3]) + "," + String(servoController->latestPositions.positions[4])
            + "," + String(servoController->latestPositions.positions[5]) + ">");
    }
}

void CommunicationManager::registerServoController(SerialServoController* servos) {
    servoController = servos;
}