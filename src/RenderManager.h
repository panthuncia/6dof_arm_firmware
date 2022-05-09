// the screen driver library : https://github.com/vindar/ILI9341_T4
#ifndef RENDER_MANAGER
#define RENDER_MANAGER

#include "TouchScreen.h"
#include <ILI9341_T4.h> 

// the tgx library 
#include <tgx.h> 
#include <font_tgx_OpenSans_Bold.h>

// let's not burden ourselves with the tgx:: prefix
using namespace tgx;


// meshes (stored in PROGMEM) for Teensy 4.0 and 4.1
#include "3Dmodels/nanosuit/nanosuit.h"
#include "3Dmodels/R2D2/R2D2.h"

// additional meshes for Teensy 4.1 since it has more flash. 
#if defined(ARDUINO_TEENSY41)
#include "3Dmodels/elementalist/elementalist.h"
#include "3Dmodels/sinbad/sinbad.h"
#include "3Dmodels/cyborg/cyborg.h"
#include "3Dmodels/dennis/dennis.h"
#include "3Dmodels/manga3/manga3.h"
#include "3Dmodels/naruto/naruto.h"
#include "3Dmodels/stormtrooper/stormtrooper.h"
#endif


// DEFAULT WIRING USING SPI 0 ON TEENSY 4/4.1
// Recall that DC must be on a valid cs pin !!! 
#define PIN_SCK     13      // mandatory 
#define PIN_MISO    12      // mandatory
#define PIN_MOSI    11      // mandatory
#define PIN_DC      10      // mandatory
#define PIN_CS      9       // mandatory (but can be any digital pin)
#define PIN_RESET   6       // could be omitted (set to 255) yet it is better to use (any) digital pin whenever possible.
#define PIN_BACKLIGHT 255   // optional. Set this only if the screen LED pin is connected directly to the Teensy 
#define PIN_TOUCH_IRQ 255   // optional. Set this only if touch is connected on the same spi bus (otherwise, set it to 255)
#define PIN_TOUCH_CS  255   // optional. Set this only if touch is connected on the same spi bus (otherwise, set it to 255)


// ALTERNATE WIRING USING SPI 1 ON TEENSY 4/4.1
// Recall that DC must be on a valid cs pin !!! 

//#define PIN_SCK     27      // mandatory 
//#define PIN_MISO    1       // mandatory
//#define PIN_MOSI    26      // mandatory
//#define PIN_DC      0       // mandatory
//#define PIN_CS      30      // mandatory (but can be any digital pin)
//#define PIN_RESET   29      // could be omitted (set to 255) yet it is better to use (any) digital pin whenever possible.
//#define PIN_BACKLIGHT 255   // optional. Set this only if the screen LED pin is connected directly to the Teensy 
//#define PIN_TOUCH_IRQ 255   // optional. Set this only if touch is connected on the same spi bus (otherwise, set it to 255)
//#define PIN_TOUCH_CS  255   // optional. Set this only if touch is connected on the same spi bus (otherwise, set it to 255)


// 30MHz SPI, we can go higher with short wires
#define SPI_SPEED       30000000

class RenderManager{
    private:
        TouchScreen touchScreen;
        bool moveModel();
        void drawTouchscreenInfo(tgx::Image<tgx::RGB565>& im, int shader, const tgx::Mesh3D<tgx::RGB565>* mesh);  // remark: need to keep the tgx:: prefix in function signatures because arduino messes with ino files....
    public:
        RenderManager();
        void begin();
        void loop();
};
#endif