#ifndef COMMUNICATION_MANAGER
#define COMMUNICATION_MANAGER
#include <Arduino.h>

#define WRITE_SERVO_COMMAND 0
#define READ_SERVO_COMMAND 1
#define BUFF_SIZE 40

class SerialServoController;
class CommunicationManager{
    private:
        char inputBuffer[BUFF_SIZE];
        const char startMarker = '<';
        const char endMarker = '>';
        byte bytesRecvd = 0;
        boolean readInProgress = false;
        SerialServoController* servoController;

char messageFromPC[BUFF_SIZE] = {0};

    public:
        boolean newDataFromPC = false;
        double lastCommand[6];
        CommunicationManager();
        void begin();
        void loop();
        void getDataFromPC();
        void parseData();
        void registerServoController(SerialServoController* servos);
};
#endif