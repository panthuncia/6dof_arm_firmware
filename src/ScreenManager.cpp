#include "ScreenManager.h"
ParallelRA8875 ScreenManager::tft = ParallelRA8875(true, RA8875_CS, RA8875_RESET);
uint16_t ScreenManager::tx, ScreenManager::ty;
struct _ts_event ScreenManager::ts_event;
struct _ts_event ScreenManager::empty_ts_event;
uint16_t ScreenManager::total = 256;
float ScreenManager::xScale = 1024.0F / SCREEN_WIDTH;
float ScreenManager::yScale = 1024.0F / SCREEN_HEIGHT;
uint8_t ScreenManager::ss[4];
extern uint16_t fb[SCREEN_HEIGHT / SCALE_FACTOR * SCREEN_WIDTH / SCALE_FACTOR];
ScreenManager::ScreenManager() {
}

void ScreenManager::GSLX680_I2C_Write(uint8_t regAddr, uint8_t *val, uint16_t cnt) {
    uint16_t i = 0;

    Wire.beginTransmission(GSL1680_I2C_ADDRESS);
    Wire.write(regAddr);              // register 0
    for (i = 0; i < cnt; i++, val++)  // data
    {
        Wire.write(*val);  // value
    }
    uint8_t retVal = Wire.endTransmission();
}

uint8_t ScreenManager::GSLX680_I2C_Read(uint8_t regAddr, uint8_t *pBuf, uint8_t len) {
    Wire.beginTransmission(GSL1680_I2C_ADDRESS);
    Wire.write(regAddr);  // register 0
    uint8_t retVal = Wire.endTransmission();

    uint8_t returned = Wire.requestFrom(GSL1680_I2C_ADDRESS, len);  // request 1 bytes from slave device #2

    uint8_t i;
    for (i = 0; (i < len) && Wire.available(); i++) {
        pBuf[i] = Wire.read();
    }

    return i;
}

void ScreenManager::_GSLX680_clr_reg(void) {
    uint8_t regAddr;
    uint8_t Wrbuf[4] = {0x00};
    uint8_t len = 1;

    regAddr = 0xe0;
    Wrbuf[0] = 0x88;
    GSLX680_I2C_Write(regAddr, Wrbuf, 1);
    delay(1);
    regAddr = 0x80;
    Wrbuf[0] = 0x03;
    GSLX680_I2C_Write(regAddr, Wrbuf, 1);
    delay(1);
    regAddr = 0xe4;
    Wrbuf[0] = 0x04;
    GSLX680_I2C_Write(regAddr, Wrbuf, 1);
    delay(1);
    regAddr = 0xe0;
    Wrbuf[0] = 0x00;
    GSLX680_I2C_Write(regAddr, Wrbuf, 1);
    delay(1);
}

//_GSLX680 Reset
void ScreenManager::_GSLX680_reset_chip(void) {
    uint8_t regAddr;
    uint8_t Wrbuf[4] = {0x00};

    regAddr = 0xe0;
    Wrbuf[0] = 0x88;
    GSLX680_I2C_Write(regAddr, Wrbuf, 1);
    delay(1);

    regAddr = 0xe4;
    Wrbuf[0] = 0x04;
    GSLX680_I2C_Write(regAddr, Wrbuf, 1);
    delay(1);

    regAddr = 0xbc;
    Wrbuf[0] = 0x00;
    Wrbuf[1] = 0x00;
    Wrbuf[2] = 0x00;
    Wrbuf[3] = 0x00;
    GSLX680_I2C_Write(regAddr, Wrbuf, 4);
    delay(1);
}

#ifdef REFLASH_FIRMWARE
void ScreenManager::_GSLX680_load_fw(void) {
    uint8_t regAddr;
    uint8_t Wrbuf[4] = {0x00};
    uint16_t source_line = 0;
    uint16_t source_len = sizeof(GSLX680_FW) / sizeof(struct fw_data);

    for (source_line = 0; source_line < source_len; source_line++) {
        regAddr = GSLX680_FW[source_line].offset;
        Wrbuf[0] = (char)(GSLX680_FW[source_line].val & 0x000000ff);
        Wrbuf[1] = (char)((GSLX680_FW[source_line].val & 0x0000ff00) >> 8);
        Wrbuf[2] = (char)((GSLX680_FW[source_line].val & 0x00ff0000) >> 16);
        Wrbuf[3] = (char)((GSLX680_FW[source_line].val & 0xff000000) >> 24);
        GSLX680_I2C_Write(regAddr, Wrbuf, 4);
    }
}
#endif

void ScreenManager::_GSLX680_startup_chip(void) {
    uint8_t Wrbuf[4] = {0x00};

    Wrbuf[0] = 0x00;
    GSLX680_I2C_Write(0xe0, Wrbuf, 1);
}

void ScreenManager::check_mem_data(void) {
    uint8_t write_buf;
    uint8_t read_buf[4] = {0};

    delay(30);

    GSLX680_I2C_Read(0xb0, read_buf, 4);

    if ((read_buf[3] != 0x5a) & (read_buf[2] != 0x5a) & (read_buf[1] != 0x5a) & (read_buf[0] != 0x5a)) {
        Serial.println("init failure,Reinitialize");
        // Serial.println("#########check mem read 0xb0 = %x %x %x %x #########\n", read_buf[3], read_buf[2], read_buf[1], read_buf[0]);
        // tft.setCursor(0, 16);
        // tft.setTextColor(RA8875_RED, RA8875_BLACK);
        // tft.setFontScale(0);
        // tft.print("CTP initialization failure  Reinitialize.");
        digitalWrite(GSL1680_WAKE, LOW);
        delay(20);
        digitalWrite(GSL1680_WAKE, HIGH);
        delay(20);
        // test_i2c();
        _GSLX680_clr_reg();
        _GSLX680_reset_chip();
#ifdef REFLASH_FIRMWARE
        _GSLX680_load_fw();
#endif
        _GSLX680_startup_chip();

        if ((read_buf[3] != 0x5a) & (read_buf[2] != 0x5a) & (read_buf[1] != 0x5a) & (read_buf[0] != 0x5a)) {
            // tft.setCursor(0, 16);
            // tft.setTextColor(RA8875_RED, RA8875_BLACK);
            // tft.setFontScale(0);
            while (1){
                Serial.println("CTP initialization failure  Reinitialize.");
            };
        }

    } else {
        // tft.setTextColor(RA8875_RED, RA8875_BLACK);
        // tft.setCursor(0, 16);
        // tft.setFontScale(0);
        // tft.print("CTP initialization OK.");
        Serial.println("init done");
    }
}

// get the most data about capacitive touchpanel.
uint8_t ScreenManager::GSLX680_read_data(void) {
    uint8_t touch_data[24] = {0};
    uint8_t reg = 0x80;
    GSLX680_I2C_Read(reg, touch_data, 24);

    ts_event.fingers = touch_data[0];

    ts_event.y5 = (uint16_t)(touch_data[23]) << 8 | (uint16_t)touch_data[22];
    ts_event.x5 = (uint16_t)(touch_data[21]) << 8 | (uint16_t)touch_data[20];

    ts_event.y4 = (uint16_t)(touch_data[19]) << 8 | (uint16_t)touch_data[18];
    ts_event.x4 = (uint16_t)(touch_data[17]) << 8 | (uint16_t)touch_data[16];

    ts_event.y3 = (uint16_t)(touch_data[15]) << 8 | (uint16_t)touch_data[14];
    ts_event.x3 = (uint16_t)(touch_data[13]) << 8 | (uint16_t)touch_data[12];

    ts_event.y2 = (uint16_t)(touch_data[11]) << 8 | (uint16_t)touch_data[10];
    ts_event.x2 = (uint16_t)(touch_data[9]) << 8 | (uint16_t)touch_data[8];

    ts_event.y1 = (uint16_t)(touch_data[7]) << 8 | (uint16_t)touch_data[6];
    ts_event.x1 = (uint16_t)(touch_data[5]) << 8 | (uint16_t)touch_data[4];

    return 0;
}

void ScreenManager::inttostr(uint16_t value, uint8_t *str) {
    str[0] = value / 1000 + 0x30;
    str[1] = value % 1000 / 100 + 0x30;
    str[2] = value % 1000 % 100 / 10 + 0x30;
    str[3] = value % 1000 % 100 % 10 + 0x30;
}

uint16_t ScreenManager::getElement(unsigned int x, unsigned int y) {
    // Do some error tests
    return ((uint16_t *)fb)[y * SCREEN_HEIGHT / SCALE_FACTOR + SCREEN_WIDTH - x];
}

void ScreenManager::begin() {
    pinMode(5, OUTPUT);
    digitalWrite(5, HIGH);

    Serial.begin(9600);
    Serial.println("RA8875 start");
    Wire.begin();  // join i2c bus (address optional for master)

    // tft.begin(RA8875_800x480);
    tft.begin();
    Serial.println("Found RA8875");

    //tft.GPIOX(true);  // Enable TFT - display enable tied to GPIOX

    // tft.fillWindow(RA8875_BLACK);
    // tft.setCursor(0, 0);
    // tft.setTextColor(RA8875_RED, RA8875_BLACK);
    // tft.setFontScale(0);
    // tft.print("Please wait CTP initialization..............");

    pinMode(GSL1680_INT, INPUT);
    pinMode(GSL1680_WAKE, OUTPUT);
    digitalWrite(GSL1680_WAKE, HIGH);
    delay(20);
    digitalWrite(GSL1680_WAKE, LOW);
    delay(20);
    digitalWrite(GSL1680_WAKE, HIGH);
    delay(20);

    Serial.println("clr reg");
    _GSLX680_clr_reg();
    Serial.println("reset_chip");
    _GSLX680_reset_chip();
#ifdef REFLASH_FIRMWARE
    Serial.println("load_fw");
    //_GSLX680_load_fw();
#endif
    Serial.println("startup_chip");
    _GSLX680_startup_chip();
    delay(50);
    //check_mem_data();
    delay(100);
}

void ScreenManager::readTouchData() {
    if (digitalRead(GSL1680_INT) == HIGH) {
        GSLX680_read_data();
    }
}

void ScreenManager::loop() {
			//full display test
            Serial.println("Running display test");
            //GPIO6_DR = (GPIO6_DR & LOW_BYTES) | (((uint16_t)0x5555) << 16);
            //tft.GPIO6DigitalWrite(0x5555);
            // Serial.print("DR: ");
            // Serial.println(GPIO6_DR);
            // Serial.print("PSR: ");
            // Serial.println(GPIO6_PSR);
            
			tft.Test();
}
