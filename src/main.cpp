#include <Arduino.h>
#include "SerialServoController.h"
#include "CommunicationManager.h"
#define SERVO_1_ZERO_POS 517
#define SERVO_2_ZERO_POS 535
#define SERVO_3_ZERO_POS 96
#define SERVO_4_ZERO_POS 498
#define SERVO_5_ZERO_POS 884
#define SERVO_6_ZERO_POS 500
//0 means starts at arm limit, might need to change
#define SERVO_1_MIN_CENTIDEGREES -10000
#define SERVO_2_MIN_CENTIDEGREES -9000
#define SERVO_3_MIN_CENTIDEGREES 0
#define SERVO_4_MIN_CENTIDEGREES -10000
#define SERVO_5_MIN_CENTIDEGREES -18000
#define SERVO_6_MIN_CENTIDEGREES -10000
#define SERVO_1_MAX_CENTIDEGREES 10000
#define SERVO_2_MAX_CENTIDEGREES 9000
#define SERVO_3_MAX_CENTIDEGREES 18000
#define SERVO_4_MAX_CENTIDEGREES 10000
#define SERVO_5_MAX_CENTIDEGREES 0
#define SERVO_6_MAX_CENTIDEGREES 10000
SerialServoController servoController;
CommunicationManager communicationManager;
void setup(){
	Serial.begin(115200);
	int servoStartPos[6]={SERVO_1_ZERO_POS, SERVO_2_ZERO_POS, SERVO_3_ZERO_POS, SERVO_4_ZERO_POS, SERVO_5_ZERO_POS, SERVO_6_ZERO_POS};
	int servoStartAngles[6]={0,0,0,0,0,0};
	int minAnglesCentDegrees[6]={SERVO_1_MIN_CENTIDEGREES, SERVO_2_MIN_CENTIDEGREES, SERVO_3_MIN_CENTIDEGREES, SERVO_4_MIN_CENTIDEGREES, SERVO_5_MIN_CENTIDEGREES, SERVO_6_MIN_CENTIDEGREES};
	int maxAnglesCentDegrees[6]={SERVO_1_MAX_CENTIDEGREES, SERVO_2_MAX_CENTIDEGREES, SERVO_3_MAX_CENTIDEGREES, SERVO_4_MAX_CENTIDEGREES, SERVO_5_MAX_CENTIDEGREES, SERVO_6_MAX_CENTIDEGREES};
	servoController.begin(servoStartPos, servoStartAngles, minAnglesCentDegrees, maxAnglesCentDegrees);
	servoController.registerComms(&communicationManager);
	communicationManager.begin();
	communicationManager.registerServoController(&servoController);
}

void loop(){
	communicationManager.loop();
	servoController.loop();
}