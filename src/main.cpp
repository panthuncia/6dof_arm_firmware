#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>

#define SERVOMIN  100.0 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  470.0 // This is the 'maximum' pulse length count (out of 4096)
#define SERVO_FREQ 50.0 // Analog servos run at ~50 Hz updates
void getDataFromPC();
void updateFlashInterval();
void updateServoPos();
void replyToPC();
void flashLEDs();
void moveServo();
void parseData();
void updateLED1(); 
void updateLED2(); 

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// our servo # counter
uint8_t servonum = 0;
byte servoPos = 0;
byte newServoPos = SERVOMIN;

const byte numLEDs = 1;
byte ledPin[numLEDs] = {13};
unsigned long LEDinterval[numLEDs] = {200};
unsigned long prevLEDmillis[numLEDs] = {0};

const byte buffSize = 40;
char inputBuffer[buffSize];
const char startMarker = '<';
const char endMarker = '>';
byte bytesRecvd = 0;
boolean readInProgress = false;
boolean newDataFromPC = false;

char messageFromPC[buffSize] = {0};
int newFlashInterval = 0;
float servoFraction = 0.0; // fraction of servo range to move


unsigned long curMillis;

unsigned long prevReplyToPCmillis = 0;
unsigned long replyToPCinterval = 1000;

void setup() {
  Serial.begin(9600);
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
  pwm.setOscillatorFrequency(26000000);
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

  delay(10);
  
    // flash LEDs so we know we are alive
  for (byte n = 0; n < numLEDs; n++) {
     pinMode(ledPin[n], OUTPUT);
     digitalWrite(ledPin[n], HIGH);
  }
  delay(500); // delay() is OK in setup as it only happens once
  
  for (byte n = 0; n < numLEDs; n++) {
     digitalWrite(ledPin[n], LOW);
  }
  
    // initialize the servo
  moveServo();
  
    // tell the PC we are ready
  Serial.println("<Arduino is ready>");
}

void loop() {
  // Drive each servo one at a time using setPWM()
  //pwm.setPWM(0, 0, 470);

  curMillis = millis();
  getDataFromPC();
  updateFlashInterval();
  updateServoPos();
  replyToPC();
  flashLEDs();
  moveServo();
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
  strcpy(messageFromPC, strtokIndx); // copy it to messageFromPC
  
  strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
  newFlashInterval = atoi(strtokIndx);     // convert this part to an integer
  
  strtokIndx = strtok(NULL, ","); 
  servoFraction = atof(strtokIndx);     // convert this part to a float

}

//=============

void replyToPC() {

  if (newDataFromPC) {
    newDataFromPC = false;
    Serial.print("<Msg ");
    Serial.print(messageFromPC);
    Serial.print(" NewFlash ");
    Serial.print(newFlashInterval);
    Serial.print(" SrvFrac ");
    Serial.print(servoFraction);
    Serial.print(" SrvPos ");
    Serial.print(newServoPos);
    Serial.print(" Time ");
    Serial.print(curMillis >> 9); // divide by 512 is approx = half-seconds
    Serial.println(">");
  }
}

//============

void updateFlashInterval() {

   // this illustrates using different inputs to call different functions
  if (strcmp(messageFromPC, "LED1") == 0) {
     updateLED1();
  }
  
  if (strcmp(messageFromPC, "LED2") == 0) {
     updateLED2();
  }
}

//=============

void updateLED1() {

  if (newFlashInterval > 100) {
    LEDinterval[0] = newFlashInterval;
  }
}

//=============

void updateLED2() {

  if (newFlashInterval > 100) {
    LEDinterval[1] = newFlashInterval;
  }
}

//=============

void flashLEDs() {

  for (byte n = 0; n < numLEDs; n++) {
    if (curMillis - prevLEDmillis[n] >= LEDinterval[n]) {
       prevLEDmillis[n] += LEDinterval[n];
       digitalWrite( ledPin[n], ! digitalRead( ledPin[n]) );
    }
  }
}

//=============

void updateServoPos() {

  double servoRange = SERVOMAX - SERVOMIN;
  if (servoFraction >= 0 && servoFraction <= 1) {
    newServoPos = SERVOMIN + (servoRange * servoFraction);
  }
}

//=============

void moveServo() {
  if (servoPos != newServoPos) {
    servoPos = newServoPos;
    pwm.setPWM(0, 0, newServoPos);
  }
}