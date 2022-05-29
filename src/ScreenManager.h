#ifndef SCREEN_MANAGER
#define SCREEN_MANAGER

#include "GSL1680_FW.h"
#include <SPI.h>
#include <Wire.h>
#include <stdint.h>
#include "Adafruit_GFX.h"
#include "Adafruit_RA8875.h"

#define RA8875_INT 4 //RA8875 interrupt pin
#define RA8875_CS 10 //RA8875 cs pin
#define RA8875_RESET 9 //RA8875 reset pin
#define GSL1680_WAKE 6 //GSL1680 wake pin
#define GSL1680_INT 7 //GSL1680 interrupt pin
#define GSL1680_I2C_ADDRESS 0x40  //GSL1680 is always at I2C address 0x40
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 480

struct _ts_event {
        uint16_t x1;
        uint16_t y1;
        uint16_t x2;
        uint16_t y2;
        uint16_t x3;
        uint16_t y3;
        uint16_t x4;
        uint16_t y4;
        uint16_t x5;
        uint16_t y5;
        uint8_t fingers;
    };

class ScreenManager{
 public:
    ScreenManager();
    static void begin(void);
    void loop(void);
 private:
    uint8_t addr = 0x40;
    static uint16_t tx, ty;
    static struct _ts_event ts_event;
    // LCD:hardware SPI  CTP:hardware I2C
    // Teensy 4.1 + ER-AS8875 + ER-TFTM050A2-3
    static Adafruit_RA8875 tft;
    static uint16_t total;
    static float xScale;
    static float yScale;
    static uint8_t ss[4];

    static void GSLX680_I2C_Write(uint8_t regAddr, uint8_t *val, uint16_t cnt);
    static uint8_t GSLX680_I2C_Read(uint8_t regAddr, uint8_t *pBuf, uint8_t len);
    static uint8_t GSLX680_read_data(void);
    static void _GSLX680_clr_reg(void);
    static void _GSLX680_reset_chip(void);
    static void _GSLX680_load_fw(void);
    static void _GSLX680_startup_chip(void);
    static void check_mem_data(void);
    static void inttostr(uint16_t value, uint8_t *str);
};


#endif
