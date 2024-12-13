#include <Arduino.h>
#include "SerialServoController.h"
#include "CommunicationManager.h"
#define SERVO_0_ZERO_POS 470
#define SERVO_1_ZERO_POS 1000
#define SERVO_2_ZERO_POS 975
#define SERVO_3_ZERO_POS 520
#define SERVO_4_ZERO_POS 520
#define SERVO_5_ZERO_POS 540
//0 means starts at arm limit, might need to change
#define SERVO_0_MIN_CENTIDEGREES -9000
#define SERVO_1_MIN_CENTIDEGREES -18000
#define SERVO_2_MIN_CENTIDEGREES -18000
#define SERVO_3_MIN_CENTIDEGREES -9000
#define SERVO_4_MIN_CENTIDEGREES -9000
#define SERVO_5_MIN_CENTIDEGREES -10000
#define SERVO_0_MAX_CENTIDEGREES 9000
#define SERVO_1_MAX_CENTIDEGREES 0
#define SERVO_2_MAX_CENTIDEGREES 0
#define SERVO_3_MAX_CENTIDEGREES 9000
#define SERVO_4_MAX_CENTIDEGREES 9000
#define SERVO_5_MAX_CENTIDEGREES 0
SerialServoController servoController;
CommunicationManager communicationManager;

unsigned long previousMillis = 0;
const long interval = 1;

void setup(){
	Serial.begin(9600);
	Serial.println("Starting...");
	int servoStartPos[6]={SERVO_0_ZERO_POS, SERVO_1_ZERO_POS, SERVO_2_ZERO_POS, SERVO_3_ZERO_POS, SERVO_4_ZERO_POS, SERVO_5_ZERO_POS};
	int servoStartAngles[6]={0,0,0,0,0,0};
	int minAnglesCentDegrees[6]={SERVO_0_MIN_CENTIDEGREES, SERVO_1_MIN_CENTIDEGREES, SERVO_2_MIN_CENTIDEGREES, SERVO_3_MIN_CENTIDEGREES, SERVO_4_MIN_CENTIDEGREES, SERVO_5_MIN_CENTIDEGREES};
	int maxAnglesCentDegrees[6]={SERVO_0_MAX_CENTIDEGREES, SERVO_1_MAX_CENTIDEGREES, SERVO_2_MAX_CENTIDEGREES, SERVO_3_MAX_CENTIDEGREES, SERVO_4_MAX_CENTIDEGREES, SERVO_5_MAX_CENTIDEGREES};
	servoController.registerComms(&communicationManager);
	servoController.begin(servoStartPos, servoStartAngles, minAnglesCentDegrees, maxAnglesCentDegrees);
	communicationManager.registerServoController(&servoController);
	communicationManager.begin();
	Serial.println("Setup done!");
}

void loop(){
	//Serial.println("looping!");
	communicationManager.loop();
	unsigned long currentMillis = millis();

	if (currentMillis - previousMillis >= interval) {
		previousMillis = currentMillis; // Update the time marker
		servoController.loop();
	}
}