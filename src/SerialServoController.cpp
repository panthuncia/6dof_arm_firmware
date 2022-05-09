#include "SerialServoController.h"

SerialServoController::SerialServoController(){
	servoBus.begin(&Serial2, 8, // on TX pin 1
			2); // use pin 2 as the TX flag for buffer
	servoBus.retry = 1; // enforce synchronous real time
	servoBus.debug(true);
    for(int i=0; i<6; i++){
        servos[i]=new LX16AServo(&servoBus, i+1);
    }
}

void SerialServoController::loop(){
    int divisor = 3;
	for (int i = 0; i < 1000 / divisor; i++) {
		long start = millis();
		int16_t pos[6] = {0,0,0,0,0,0};
		for(int i=0; i<6; i++){
			pos[i] = servos[i]->pos_read();
		}

		uint16_t angle = i * 24 * divisor;

		for(int i=0; i<6; i++){
			servos[i]->move_time_and_wait_for_sync(angle, 10 * divisor);
		}

		servoBus.move_sync_start();

		//if(abs(pos2-pos)>100){
		Serial.printf("\n\nPosition at %d and %d-> %s\n", pos[0], pos[2],
				servos[0]->isCommandOk() ? "OK" : "\n\nERR!!\n\n");
		Serial.printf("Move to %d -> %s\n", angle,
				servos[0]->isCommandOk() ? "OK" : "\n\nERR!!\n\n");
		//}
		long took = millis() - start;

		long time = (10 * divisor) - took;
		if (time > 0)
			delay(time);
		else {
			Serial.println("Real Time broken, took: " + String(took));
		}
	}
	Serial.println("Interpolated Set pos done, not long set");

	servoBus.retry = 5; // These commands must arrive
	for(int i=0; i<6; i++){
		servos[i]->move_time(0, 10000);
	}
	servoBus.retry = 0; // Back to low latency mode
	for (int i = 0; i < 1000 / divisor; i++) {
		long start = millis();
		int16_t pos[6] = {0,0,0,0,0,0};
		for(int i=0; i<6; i++){
			pos[i] = servos[i]->pos_read();
		}

		Serial.printf("\n\nPosition at %d and %d\n", pos[0], pos[1]);

		Serial.println("Voltage = " + String(servos[1]->vin()));
		Serial.println("Temp = " + String(servos[1]->temp()));

		long took = millis() - start;

		long time = (10 * divisor) - took;
		if (time > 0)
			delay(time);
		else {
			Serial.println("Real Time broken, took: " + String(took));
		}
	}
	Serial.println("Loop resetting");
}