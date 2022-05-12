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
}
void CommunicationManager::getDataFromPC() {
    // receive data from PC and save it into inputBuffer
    if (Serial.available() > 0) {
        char x = (char)Serial.read();
        // the order of these IF clauses is significant
        if (x == endMarker) {
            readInProgress = false;
            inputBuffer[bytesRecvd] = 0;
            parseData();
        }

        if (readInProgress) {
            inputBuffer[bytesRecvd] = x;
            // Serial.print("<Received byte: ");
            // Serial.print(x);
            // Serial.print(">");
            bytesRecvd++;
            if (bytesRecvd == BUFF_SIZE) {
                bytesRecvd = BUFF_SIZE - 1;
            }
        }

        if (x == startMarker) {
            bytesRecvd = 0;
            readInProgress = true;
            // Serial.print("<Received start byte>");
        }
    }
}
void CommunicationManager::parseData() {
    // split the data into its parts
    char* strtokIndx;  // this is used by strtok() as an index

    strtokIndx = strtok(inputBuffer, ",");  // get the first part - the string
    uint8_t command = atoi(strtokIndx);     // convert this part to centidegrees
    switch (command) {
        case WRITE_SERVO_COMMAND: {
            strtokIndx = strtok(NULL, ",");
            lastCommand[0] = atof(strtokIndx) * 100;

            strtokIndx = strtok(NULL, ",");
            lastCommand[1] = atof(strtokIndx) * 100;

            strtokIndx = strtok(NULL, ",");
            lastCommand[2] = atof(strtokIndx) * 100;

            strtokIndx = strtok(NULL, ",");
            lastCommand[3] = atof(strtokIndx) * 100;

            strtokIndx = strtok(NULL, ",");
            lastCommand[4] = atof(strtokIndx) * 100;

            strtokIndx = strtok(NULL, ",");
            lastCommand[5] = atof(strtokIndx) * 100;

            newDataFromPC = true;
            break;
        }
        case READ_SERVO_COMMAND: {
            Serial.print("<" + String(servoController->latestPositions.positions[0]) + "," + 
                String(servoController->latestPositions.positions[1]) + "," + String(servoController->latestPositions.positions[2]) 
                + "," + String(servoController->latestPositions.positions[3]) + "," + String(servoController->latestPositions.positions[4])
                + "," + String(servoController->latestPositions.positions[5]) + ">");
            break;
        }
    }
}

void CommunicationManager::registerServoController(SerialServoController* servos) {
    servoController = servos;
}