#ifndef SERIAL_SERVO_CONTROLLER
#define SERIAL_SERVO_CONTROLLER
#include <lx16a-servo.h>
class SerialServoController{
    private:
        LX16ABus servoBus;
        LX16AServo** servos;
    public:
        SerialServoController();
        void loop();
};
#endif