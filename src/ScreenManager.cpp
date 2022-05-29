#include "ScreenManager.h"

Adafruit_RA8875 ScreenManager::tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);
uint16_t ScreenManager::tx, ScreenManager::ty;
struct _ts_event ScreenManager::ts_event;
uint16_t ScreenManager::total = 256;
float ScreenManager::xScale = 1024.0F / SCREEN_WIDTH;
float ScreenManager::yScale = 1024.0F / SCREEN_HEIGHT;
uint8_t ScreenManager::ss[4];

ScreenManager::ScreenManager(){
    
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
        tft.textSetCursor(0, 16);
        tft.textColor(RA8875_RED, RA8875_BLACK);
        tft.textEnlarge(0);
        tft.textWrite("CTP initialization failure  Reinitialize.");
        digitalWrite(GSL1680_WAKE, LOW);
        delay(20);
        digitalWrite(GSL1680_WAKE, HIGH);
        delay(20);
        // test_i2c();
        _GSLX680_clr_reg();
        _GSLX680_reset_chip();
        _GSLX680_load_fw();
        _GSLX680_startup_chip();
        //_GSLX680_reset_chip();
        //_GSLX680_startup_chip();

        if ((read_buf[3] != 0x5a) & (read_buf[2] != 0x5a) & (read_buf[1] != 0x5a) & (read_buf[0] != 0x5a)) {
            tft.textSetCursor(0, 16);
            tft.textColor(RA8875_RED, RA8875_BLACK);
            tft.textEnlarge(0);
            tft.textWrite("CTP initialization failure  Reinitialize.");
            while (1)
                ;
        }

    } else {
        tft.textMode();
        tft.textColor(RA8875_RED, RA8875_BLACK);
        tft.textSetCursor(0, 16);
        tft.textEnlarge(0);
        tft.textWrite("CTP initialization OK.");
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

void ScreenManager::begin() {
    pinMode(5, OUTPUT);
    digitalWrite(5, HIGH);

    Serial.begin(9600);
    Serial.println("RA8875 start");
    Wire.begin();  // join i2c bus (address optional for master)

    while (!tft.begin(RA8875_800x480)) {
        Serial.println("RA8875 Not Found!");
        delay(100);
    }
    Serial.println("Found RA8875");

    tft.displayOn(true);
    tft.GPIOX(true);                               // Enable TFT - display enable tied to GPIOX
    tft.PWM1config(true, RA8875_PWM_CLK_DIV1024);  // PWM output for backlight
    tft.PWM1out(255);

    tft.fillScreen(RA8875_BLACK);
    tft.textMode();
    tft.textSetCursor(0, 0);
    tft.textColor(RA8875_RED, RA8875_BLACK);
    tft.textEnlarge(0);
    tft.textWrite("Buydisplay.com     Please wait CTP initialization..............");

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
    Serial.println("load_fw");
    _GSLX680_load_fw();
    Serial.println("startup_chip");
    _GSLX680_startup_chip();
    //_GSLX680_reset_chip();
    //_GSLX680_startup_chip();
    delay(50);
    check_mem_data();
    delay(100);
    tft.fillScreen(RA8875_BLACK);
    tft.textMode();
    tft.textSetCursor(0, 0);
    tft.textColor(RA8875_RED, RA8875_BLACK);
    tft.textEnlarge(0);
    tft.textWrite("Capacitive touch screen test.");
    tft.textSetCursor(SCREEN_WIDTH - 40, 0);
    tft.textWrite("CLEAR");
}

void ScreenManager::loop() {
    if (digitalRead(GSL1680_INT) == HIGH) {
        GSLX680_read_data();

        tft.graphicsMode();
        if (ts_event.fingers == 1) {
            tft.fillCircle(ts_event.x1, ts_event.y1, 5, RA8875_RED);
        }
        if (ts_event.fingers == 2) {
            tft.fillCircle(ts_event.x1, ts_event.y1, 5, RA8875_RED);
            tft.fillCircle(ts_event.x2, ts_event.y2, 5, RA8875_GREEN);
        }
        if (ts_event.fingers == 3) {
            tft.fillCircle(ts_event.x1, ts_event.y1, 5, RA8875_RED);
            tft.fillCircle(ts_event.x2, ts_event.y2, 5, RA8875_GREEN);
            tft.fillCircle(ts_event.x3, ts_event.y3, 5, RA8875_BLUE);
        }
        if (ts_event.fingers == 4) {
            tft.fillCircle(ts_event.x1, ts_event.y1, 5, RA8875_RED);
            tft.fillCircle(ts_event.x2, ts_event.y2, 5, RA8875_GREEN);
            tft.fillCircle(ts_event.x3, ts_event.y3, 5, RA8875_BLUE);
            tft.fillCircle(ts_event.x4, ts_event.y4, 5, RA8875_CYAN);
        }
        if (ts_event.fingers == 5) {
            tft.fillCircle(ts_event.x1, ts_event.y1, 5, RA8875_RED);
            tft.fillCircle(ts_event.x2, ts_event.y2, 5, RA8875_GREEN);
            tft.fillCircle(ts_event.x3, ts_event.y3, 5, RA8875_BLUE);
            tft.fillCircle(ts_event.x4, ts_event.y4, 5, RA8875_CYAN);
            tft.fillCircle(ts_event.x5, ts_event.y5, 5, RA8875_MAGENTA);
        }

        if (ts_event.x1 >= 760 && (ts_event.y1 & 0x0fff) <= 30) {
            tft.fillScreen(RA8875_BLACK);
            tft.textMode();
            tft.textSetCursor(0, 0);
            tft.textColor(RA8875_RED, RA8875_BLACK);
            tft.textEnlarge(0);
            tft.textWrite("Capacitive touch screen test.");
            tft.textSetCursor(SCREEN_WIDTH - 40, 0);
            tft.textWrite("CLEAR");
        }

        inttostr(ts_event.x1 & 0x0fff, ss);

        tft.textMode();
        tft.textSetCursor(100, 80);
        tft.textColor(RA8875_RED, RA8875_BLACK);
        tft.textWrite("X =  ");
        tft.textSetCursor(140, 80);
        tft.writeCommand(RA8875_MRWC);
        tft.writeData(ss[0]);
        delay(1);
        tft.writeData(ss[1]);
        delay(1);
        tft.writeData(ss[2]);
        delay(1);
        tft.writeData(ss[3]);

        inttostr(ts_event.y1 & 0x0fff, ss);
        tft.textSetCursor(100, 140);
        tft.textWrite("Y =  ");
        tft.textSetCursor(140, 140);
        tft.writeCommand(RA8875_MRWC);
        tft.writeData(ss[0]);
        delay(1);
        tft.writeData(ss[1]);
        delay(1);
        tft.writeData(ss[2]);
        delay(1);
        tft.writeData(ss[3]);
    }
}
