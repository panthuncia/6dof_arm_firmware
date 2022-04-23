#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>

#define SERVO0MIN  115 // 0 degrees
#define SERVO0MAX  480 // 270 degrees
#define SERVO1MIN  111 // 0 degrees
#define SERVO1MAX  473 // 270 degrees
#define SERVO2MIN  116 // 0 degrees
#define SERVO2MAX  477 // 270 degrees
#define SERVO3MIN  121 // 0 degrees
#define SERVO3MAX  486 // 270 degrees
#define SERVO4MIN  124 // This is the 'minimum' pulse length count (out of 4096)
#define SERVO4MAX  494 // This is the 'maximum' pulse length count (out of 4096)
#define SERVO5MIN  118 // This is the 'minimum' pulse length count (out of 4096)
#define SERVO5MAX  479 // This is the 'maximum' pulse length count (out of 4096)
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

double SERVO0DIFF;
double SERVO1DIFF;
double SERVO2DIFF;
double SERVO3DIFF;
double SERVO4DIFF;
double SERVO5DIFF;

void getDataFromPC();
void replyToPC();
void parseData();

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

const byte buffSize = 40;
char inputBuffer[buffSize];
const char startMarker = '<';
const char endMarker = '>';
byte bytesRecvd = 0;
boolean readInProgress = false;
boolean newDataFromPC = false;

char messageFromPC[buffSize] = {0};

unsigned long curMillis;

unsigned long prevReplyToPCmillis = 0;
unsigned long replyToPCinterval = 1000;

void setup() {
  SERVO0DIFF=(SERVO0MAX - SERVO0MIN);
  SERVO1DIFF=(SERVO1MAX - SERVO1MIN);
  SERVO2DIFF=(SERVO2MAX - SERVO2MIN);
  SERVO3DIFF=(SERVO3MAX - SERVO3MIN);
  SERVO4DIFF=(SERVO4MAX - SERVO5MIN);
  SERVO5DIFF=(SERVO5MAX - SERVO5MIN);
  Serial.begin(57600);
  Serial.println("8 channel Servo test!");

  pwm.begin();
  /*
   * In theory the internal oscillator (clock) is 25MHz but it really isn't
   * that precise. You can 'calibrate' this by tweaking this number until
   * you get the PWM update frequency you're expecting!
   * The int.osc. for the PCA9685 chip is a range between about 23-27MHz and
   * is used for calculating things like writeMicroseconds()
   * Analog servos run at ~50 Hz updates, It is importaint to use an
   * oscilloscope in setting the int.osc frequency for the I2C PCA9685 chip.
   * 1) Attach the oscilloscope to one of the PWM signal pins and ground on
   *    the I2C PCA9685 chip you are setting the value for.
   * 2) Adjust setOscillatorFrequency() until the PWM update frequency is the
   *    expected value (50Hz for most ESCs)
   * Setting the value here is specific to each individual I2C PCA9685 chip and
   * affects the calculations for the PWM update frequency. 
   * Failure to correctly set the int.osc value will cause unexpected PWM results
   */
  pwm.setOscillatorFrequency(24100000);
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

  delay(10);
  pwm.setPWM(0, 0, SERVO0MAX);
  pwm.setPWM(2, 0, SERVO1MAX);
  pwm.setPWM(4, 0, SERVO2MAX);
  pwm.setPWM(6, 0, SERVO3MAX);
  pwm.setPWM(8, 0, SERVO4MAX);
  pwm.setPWM(10, 0, SERVO5MAX);
  // tell the PC we are ready
  Serial.println("<Arduino is ready>");
}

void loop() {
  curMillis = millis();
  getDataFromPC();
  replyToPC();
}

//=============

void getDataFromPC() {
    // receive data from PC and save it into inputBuffer
  if(Serial.available() > 0) {
    char x = (char)Serial.read();
      // the order of these IF clauses is significant
    if (x == endMarker) {
      readInProgress = false;
      newDataFromPC = true;
      inputBuffer[bytesRecvd] = 0;
      parseData();
    }
    
    if(readInProgress) {
      inputBuffer[bytesRecvd] = x;
      Serial.print("<Received byte: ");
      Serial.print(x);
      Serial.print(">");
      bytesRecvd ++;
      if (bytesRecvd == buffSize) {
        bytesRecvd = buffSize - 1;
      }
    }

    if (x == startMarker) { 
      bytesRecvd = 0; 
      readInProgress = true;
      Serial.print("<Received start byte>");
    }
  }
}

//=============
 
void parseData() {
    // split the data into its parts
    
  char * strtokIndx; // this is used by strtok() as an index
  
  strtokIndx = strtok(inputBuffer,",");      // get the first part - the string
  uint16_t newServo0Pos = atoi(strtokIndx);     // convert this part to an int
  newServo0Pos = (((newServo0Pos) * SERVO0DIFF) / (270)) + SERVO0MIN;
  if(newServo0Pos<SERVO0MIN){
    newServo0Pos=SERVO0MIN;
  } else if(newServo0Pos>SERVO0MAX){
    newServo0Pos=SERVO0MAX;
  }
  pwm.setPWM(0, 0, newServo0Pos);
  strtokIndx = strtok(NULL,",");      // get the first part - the string
  uint16_t newServo1Pos = atoi(strtokIndx);     // convert this part to an int
  newServo1Pos = (((newServo1Pos) * SERVO1DIFF) / (270)) + SERVO1MIN;
  if(newServo1Pos<SERVO1MIN){
    newServo1Pos=SERVO1MIN;
  } else if(newServo1Pos>SERVO1MAX){
    newServo1Pos=SERVO1MAX;
  }
  pwm.setPWM(2, 0, newServo1Pos);
  strtokIndx = strtok(NULL,",");      // get the first part - the string
  uint16_t newServo2Pos = atoi(strtokIndx);     // convert this part to an int
  newServo2Pos = (((newServo2Pos) * SERVO2DIFF) / (270)) + SERVO2MIN;
  if(newServo2Pos<SERVO2MIN){
    newServo2Pos=SERVO2MIN;
  } else if(newServo2Pos>SERVO2MAX){
    newServo2Pos=SERVO2MAX;
  }
  pwm.setPWM(4, 0, newServo2Pos);
  strtokIndx = strtok(NULL,",");      // get the first part - the string
  uint16_t newServo3Pos = atoi(strtokIndx);     // convert this part to an int
  newServo3Pos = (((newServo3Pos) * SERVO3DIFF) / (270)) + SERVO3MIN;
  if(newServo3Pos<SERVO3MIN){
    newServo3Pos=SERVO3MIN;
  } else if(newServo3Pos>SERVO3MAX){
    newServo3Pos=SERVO3MAX;
  }
  pwm.setPWM(6, 0, newServo3Pos);
  strtokIndx = strtok(NULL,",");      // get the first part - the string
  uint16_t newServo4Pos = atoi(strtokIndx);     // convert this part to an int
  newServo4Pos = (((newServo4Pos) * SERVO4DIFF) / (270)) + SERVO4MIN;
  if(newServo4Pos<SERVO4MIN){
    newServo4Pos=SERVO4MIN;
  } else if(newServo4Pos>SERVO4MAX){
    newServo4Pos=SERVO4MAX;
  }
  pwm.setPWM(8, 0, newServo4Pos);
  strtokIndx = strtok(NULL,",");      // get the first part - the string
  uint16_t newServo5Pos = atoi(strtokIndx);     // convert this part to an int
  newServo5Pos = (((newServo5Pos) * SERVO5DIFF) / (270)) + SERVO5MIN;
  if(newServo5Pos<SERVO5MIN){
    newServo5Pos=SERVO5MIN;
  } else if(newServo5Pos>SERVO5MAX){
    newServo5Pos=SERVO5MAX;
  }
  pwm.setPWM(10, 0, newServo5Pos);
}

//=============

void replyToPC() {

  if (newDataFromPC) {
    newDataFromPC = false;
    Serial.print("<Time ");
    Serial.print(millis()-curMillis);
    Serial.println(">");
  }
}