// the screen driver library : https://github.com/vindar/ILI9341_T4
#ifndef RENDER_MANAGER
#define RENDER_MANAGER
#include "SerialServoController.h"
#define BUFF_SIZE 40
class CommunicationManager{
    private:
        char inputBuffer[BUFF_SIZE];
        const char startMarker = '<';
        const char endMarker = '>';
        byte bytesRecvd = 0;
        boolean readInProgress = false;
        boolean newDataFromPC = false;
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