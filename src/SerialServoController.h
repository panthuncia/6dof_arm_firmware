#ifndef SERIAL_SERVO_CONTROLLER
#define SERIAL_SERVO_CONTROLLER
#include "lx16a-servo.h"
//#include "CommunicationManager.h"
#define SERVO_TIME_PARAM 30
struct Positions {
   double positions[6];
};
class CommunicationManager;
class SerialServoController{
    private:
        LX16ABus servoBus;
        LX16AServo** servos;
        CommunicationManager* comms;
    public:
        Positions latestPositions;
        SerialServoController();
        void begin(int* servoStartPos, int* servoStartAngles, int* minAnglesCentDegrees, int* maxAnglesCentDegrees);
        void loop();
        void registerComms(CommunicationManager* com);
};
#endif