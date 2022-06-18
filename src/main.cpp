#include <Arduino.h>

#include "CommunicationManager.h"
#include "RenderManager.h"
#include "ScreenManager.h"
#include "SerialServoController.h"
#define SERVO_1_ZERO_POS 517
#define SERVO_2_ZERO_POS 535
#define SERVO_3_ZERO_POS 96
#define SERVO_4_ZERO_POS 498
#define SERVO_5_ZERO_POS 850
#define SERVO_6_ZERO_POS 460
// 0 means starts at arm limit, might need to change
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
ScreenManager ScreenManager;

IntervalTimer touchTimer;
IntervalTimer servoTimer;

#if defined(__IMXRT1062__)
extern "C" uint32_t set_arm_clock(uint32_t frequency);
#endif

void touchTimerCallback() {
    ScreenManager.readTouchData();
}

void servoTimerCallback() {
    sei();
    communicationManager.loop();
    servoController.loop();
}

void setup() {
    Serial.begin(115200);
    Serial.println("Starting...");
    pinMode(14, OUTPUT);
    digitalWrite(14, HIGH);
    int servoStartPos[6] = {SERVO_1_ZERO_POS, SERVO_2_ZERO_POS, SERVO_3_ZERO_POS, SERVO_4_ZERO_POS, SERVO_5_ZERO_POS, SERVO_6_ZERO_POS};
    int servoStartAngles[6] = {0, 0, 0, 0, 0, 0};
    int minAnglesCentDegrees[6] = {SERVO_1_MIN_CENTIDEGREES, SERVO_2_MIN_CENTIDEGREES, SERVO_3_MIN_CENTIDEGREES, SERVO_4_MIN_CENTIDEGREES, SERVO_5_MIN_CENTIDEGREES, SERVO_6_MIN_CENTIDEGREES};
    int maxAnglesCentDegrees[6] = {SERVO_1_MAX_CENTIDEGREES, SERVO_2_MAX_CENTIDEGREES, SERVO_3_MAX_CENTIDEGREES, SERVO_4_MAX_CENTIDEGREES, SERVO_5_MAX_CENTIDEGREES, SERVO_6_MAX_CENTIDEGREES};
    servoController.registerComms(&communicationManager);
    //servoController.begin(servoStartPos, servoStartAngles, minAnglesCentDegrees, maxAnglesCentDegrees, &Serial3);
    communicationManager.registerServoController(&servoController);
    //communicationManager.begin();
    //render_setup();
    ScreenManager.begin();
    //touchTimer.begin(touchTimerCallback, 500);
    //servoTimer.begin(servoTimerCallback, 30000);
#if defined(__IMXRT1062__)
    //set_arm_clock(912000000);
    Serial.print("F_CPU_ACTUAL=");
    Serial.println(F_CPU_ACTUAL);
#endif
    Serial.println("Setup done!");
}

void loop() {
    render_loop();
    ScreenManager.loop();
}