#include "SerialServoController.h"

#include "CommunicationManager.h"
SerialServoController::SerialServoController() {
}
void SerialServoController::begin(int* servoStartPos, int* servoStartAngles, int* minAnglesCentDegrees, int* maxAnglesCentDegrees, HardwareSerial* serial) {
    servoBus.begin(serial, 8,  // on TX pin 1
                   2);           // use pin 2 as the TX flag for buffer
    servoBus.retry = 1;          // enforce synchronous real time
    servoBus.debug(true);
    for (uint8_t i = 0; i < 6; i++) {
        servos[i] = new LX16AServo(&servoBus, i + 1);
        servos[i]->calibrate(servoStartPos[i], servoStartAngles[i], minAnglesCentDegrees[i], maxAnglesCentDegrees[i]);
        Serial.println(i);
    }
}
void SerialServoController::loop() {
    for (int i = 0; i < 6; i++) {
        latestPositions.positions[i] = servos[i]->pos_read();
    }
	// long start = millis();
    if (comms->newDataFromPC) {
        for (int i = 0; i < 6; i++) {
            Serial.print("Moving servo to ");
			Serial.println(comms->lastCommand[i]);
            servos[i]->move_time_and_wait_for_sync(comms->lastCommand[i], SERVO_TIME_PARAM);
        }
		servoBus.move_sync_start();
		// long took = millis() - start;
		// long time = (SERVO_TIME_PARAM) - took;
		// if (time > 0)
		// 	delay(time);
		// else {
		// 	Serial.println("Real Time broken, took: " + String(took));
		// }
        comms->newDataFromPC = false;
    }
}
void SerialServoController::registerComms(CommunicationManager* com) {
    comms = com;
}