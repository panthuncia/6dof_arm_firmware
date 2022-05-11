#include "SerialServoController.h"
#include "CommunicationManager.h"
SerialServoController::SerialServoController(){
	
}
void SerialServoController::begin(int* servoStartPos, int* servoStartAngles, int* minAnglesCentDegrees, int* maxAnglesCentDegrees){
	servoBus.begin(&Serial2, 8, // on TX pin 1
			2); // use pin 2 as the TX flag for buffer
	servoBus.retry = 1; // enforce synchronous real time
	servoBus.debug(true);
    for(int i=0; i<6; i++){
        servos[i]=new LX16AServo(&servoBus, i+1);
		servos[i]->calibrate(servoStartPos[i], servoStartAngles[i], minAnglesCentDegrees[i], maxAnglesCentDegrees[i]);
    }
}
void SerialServoController::loop(){
	for(int i=0; i<6; i++){
		latestPositions.positions[i] = servos[i]->pos_read();
	}

	for(int i=0; i<6; i++){
		servos[i]->move_time_and_wait_for_sync(comms->lastCommand[i], SERVO_TIME_PARAM);
	}
}
void SerialServoController::registerComms(CommunicationManager* com){
	comms=com;
}