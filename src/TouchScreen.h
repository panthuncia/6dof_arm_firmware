#ifndef TOUCH_SCREEN
#define TOUCH_SCREEN

#include <ILI9341_T4.h>

#define CS_PIN 9
#define DC_PIN 10
#define SCK_PIN 13
#define MOSI_PIN 11
#define MISO_PIN 12
#define RST_PIN 6
#define TOUCH_CS_PIN 5
#define TOUCH_IRQ_PIN 4

//globals for display driver, documentation here https://github.com/vindar/ILI9341_T4
extern ILI9341_T4::ILI9341Driver tft;
extern uint16_t fb[240*320]; // our memory framebuffer. The screen has size 320 x 240 with color in 16 bits - 565 format 
extern DMAMEM uint16_t fb_internal1[240*320];  // an 'internal' frame buffer for double buffering
extern ILI9341_T4::DiffBuffStatic<4096> diff1; // a first diff buffer with 4K memory (statically allocated)
extern ILI9341_T4::DiffBuffStatic<4096> diff2; // and a second one. 

class TouchScreen{
    private:
    public:
        TouchScreen();
        void begin();
        void update();
};
#endif