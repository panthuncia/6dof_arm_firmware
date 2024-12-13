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
    int commandsRead = 0;
    while (Serial.available() > 0) {
        char x = (char)Serial.read();
        // the order of these IF clauses is significant
        if (x == endMarker) {
            readInProgress = false;
            bytesRecvd++;
            //Serial.print("Bytes recieved: ");
            //Serial.println(bytesRecvd);
            currentInputBuffer[bytesRecvd] = '\0';
            //Serial.print("<Received end byte>");
            commandsRead+=1;
            if(readingMoveCommand){
                readingMoveCommand = false;
                processMoveCommand = true;
            }
            if(readingPrintCommand){
                readingPrintCommand = false;
                processPrintCommand = true;
            }
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
                    readingMoveCommand = true;
                    currentInputBuffer = moveCommandBuffer;
                }
                else if (x=='1'){
                    readingPrintCommand = true;
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
            //Serial.print("Bytes recieved: ");
            //Serial.println(bytesRecvd);
        }

        if (x == startMarker) {
            
            bytesRecvd = 0;
            readInProgress = true;
            isFirstByte = true;
            //Serial.print("<Received start byte>");
            //memset(currentInputBuffer, 0, BUFF_SIZE); // Clear the buffer
        }
    }
}
void CommunicationManager::parseData() {
    // split the data into its parts
    if(processMoveCommand){
        //Serial.println("In process move command");
        //Serial.println(moveCommandBuffer);
        processMoveCommand = false;
        char* inputBuffer = moveCommandBuffer;
        char* strtokIndx;  // this is used by strtok() as an index
        strtokIndx = strtok(inputBuffer, ",");  // get the first part - the string
        uint8_t command = atoi(strtokIndx);

        int i=0;
        strtokIndx = strtok(NULL, ",");
        while (strtokIndx != NULL){
            //Serial.println(i);
            lastCommand[i] = atoi(strtokIndx);
            strtokIndx = strtok(NULL, ",");
            i++;
        }

        //Serial.print("Got commands for ");
        //Serial.print(i);
        //Serial.println(" servos");

        newDataFromPC = true;
        // Serial.println("lastCommand: ");
        // Serial.println(lastCommand[0]);
        // Serial.println(lastCommand[1]);
        // Serial.println(lastCommand[2]);
        // Serial.println(lastCommand[3]);
        // Serial.println(lastCommand[4]);
        // Serial.println(lastCommand[5]);
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