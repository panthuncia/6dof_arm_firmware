#include "TouchScreen.h"

//globals for display driver, documentation here https://github.com/vindar/ILI9341_T4
ILI9341_T4::ILI9341Driver tft(CS_PIN, DC_PIN, SCK_PIN, MOSI_PIN, MISO_PIN, RST_PIN, TOUCH_CS_PIN, TOUCH_IRQ_PIN);
uint16_t fb[240*320]; // our memory framebuffer. The screen has size 320 x 240 with color in 16 bits - 565 format 
DMAMEM uint16_t fb_internal1[240*320];  // an 'internal' frame buffer for double buffering
ILI9341_T4::DiffBuffStatic<4096> diff1; // a first diff buffer with 4K memory (statically allocated)
ILI9341_T4::DiffBuffStatic<4096> diff2; // and a second one. 
TouchScreen::TouchScreen(){

}

void TouchScreen::update(){
    //draw_something_onto(fb); // draw the next frame onto fb
    tft.update(fb); // push the framebuffer. That's it !
}

void TouchScreen::begin(){
    //begin display driver, SPI_WRITE_SPEED and SPI_READ_SPEED are optional. Higher values runs the SPI bus faster.
    if (!tft.begin(/*SPI_WRITE_SPEED, SPI_READ_SPEED*/)){
        Serial.print("Display driver failed to initialize!");
    }
    tft.setRotation(0); // use the most efficient orientation (portrait mode)
    tft.setFramebuffers(fb_internal1); // registering one framebuffer. This activates double buffering
    tft.setDiffBuffers(&diff1, &diff2); // registering 2 diff buffers. This activates differential update mode
    tft.setRefreshRate(120); // set the display refresh rate around 120Hz
    tft.setVSyncSpacing(2); // enable vsync and set framerate = refreshrate/2 (typical choice
    tft.output(&Serial);  // output debug infos to serial port.
}