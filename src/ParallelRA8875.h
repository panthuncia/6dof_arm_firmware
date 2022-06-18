#ifndef FAST_RA8875
#define FAST_RA8875

#include <arduino.h>
#define BL_PIN 2
#define color_brown 0x40c0
#define color_black 0x0000
#define color_white 0xffff
#define color_red 0xf800
#define color_green 0x07e0
#define color_blue 0x001f
#define color_yellow color_red | color_green
#define color_cyan color_green | color_blue
#define color_purple color_red | color_blue

#define LOW_BYTES 0xFFFF
#define HIGH_BYTES 0xFFFF0000
#define BIT1 0x01
#define BIT2 0x02
#define BIT3 0x04
#define BIT4 0x08
#define BIT5 0x10
#define BIT6 0x20
#define BIT7 0x40
#define BIT8 0x80
#define BIT9 0x100
#define BIT10 0x200
#define BIT11 0x400
#define BIT12 0x800
#define BIT13 0x1000
#define BIT14 0x2000
#define BIT15 0x4000
#define BIT16 0x8000


class ParallelRA8875 {
   public:
    ParallelRA8875(boolean MODE, const uint8_t RD = 33, const uint8_t WR = 32, const uint8_t CS = 3, const uint8_t RS = 4, const uint8_t WAIT = 5);
    void begin();
    void NextStep(void);
    ///////////write command
    void LCD_CmdWrite(unsigned char Cmd);
    ///////////write data or parameter
    void LCD_DataWrite(unsigned int Data);
    ////////////////write command and parameter
    void Write_Dir(uint8_t Cmd, uint8_t Data);
    ///////////////read  status
    uint8_t LCD_StatusRead(void);
    /////////////////read data parameter
    uint16_t LCD_DataRead(void);
    /*
    ////////LCM reset
    void LCD_Reset(void)
    {
            MCU_RST = 0;
            Delay1ms(1);
            MCU_RST = 1;
            Delay1ms(1);
    }*/
    ///////////////check busy
    void Chk_Busy(void);
    ///////////////check bte busy
    void Chk_BTE_Busy(void);
    ///////////////check dma busy
    void Chk_DMA_Busy(void);
    /////////////PLL setting
    void PLL_ini(void);
    /////////////////Set the working window area
    void Active_Window(uint16_t XL, uint16_t XR, uint16_t YT, uint16_t YB);
    /////////////LCM initial
    void LCD_Initial(void);
    ///////////////Background color settings
    void Text_Background_Color1(uint16_t b_color);
    ///////////////Background color settings
    void Text_Background_Color(uint8_t setR, uint8_t setG, uint8_t setB);
    ////////////////Foreground color settings
    void Text_Foreground_Color1(uint16_t b_color);
    ////////////////Foreground color settings
    void Text_Foreground_Color(uint8_t setR, uint8_t setG, uint8_t setB);
    //////////////////BTE area size settings
    void BTE_Size(uint8_t width, uint8_t height);
    ////////////////////BTE starting position
    void BTE_Source(uint16_t SX, uint16_t DX, uint16_t SY, uint16_t DY);
    ///////////////Memory write position
    void MemoryWrite_Position(uint16_t X, uint16_t Y);
    ////////////////Text write position
    void FontWrite_Position(uint16_t X, uint16_t Y);
    //////////////writing text
    void String(const char *str);
    /////////////////Scroll window size
    void Scroll_Window(uint16_t XL, uint16_t XR, uint16_t YT, uint16_t YB);
    ///////////////Window scroll offset Settings
    void Scroll(uint16_t X, uint16_t Y);
    ///////////////The FLASH reading area   setting
    void DMA_block_mode_size_setting(uint16_t BWR, uint16_t BHR, uint16_t SPWR);
    /////////////FLASH read start position Settings
    void DMA_Start_address_setting(uint32_t set_address);
    ///////////drawing circle
    void Draw_Circle(uint16_t X, uint16_t Y, uint16_t R);
    ///////////drawing elliptic curve
    void Draw_Ellipse(uint16_t X, uint16_t Y, uint16_t R1, uint16_t R2);
    ///////////drawing line, rectangle, triangle
    void Draw_Line(uint16_t XS, uint16_t XE, uint16_t YS, uint16_t YE);
    ////////////draw a triangle of three point
    void Draw_Triangle(uint16_t X3, uint16_t Y3);
    /////////////Touch the interrupt judgment
    uint8_t Touch_Status(void);
    //////////check interrupt flag bit
    uint8_t Chk_INT(void);
    uint8_t Chk_INT2(void);
    /////////Read TP the X coordinate  Resistive Touch Screen
    uint8_t ADC_X(void);
    /////////Read TP the Y coordinate   Resistive Touch Screen
    uint8_t ADC_Y(void);
    ////////////Read TP the XY coordinates, the coordinates (high)   Resistive Touch Screen
    uint8_t ADC_XY(void);
    //////////////Resistive Touch Screen the coordinate display
    void TP(void);
    ////////////Show the picture of the flash
    void Displaypicture(uint8_t picnum);
    //////Shear pictures
    // Shear pictures number:picnum
    // display position:x1,y1,x2,y2
    // the upper left of the shear image coordinates :x,y
    void CutPictrue(uint8_t picnum, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, unsigned long x, unsigned long y);
    // full display test
    void Test(void);
    void GPIO6DigitalWrite(uint16_t data);
    uint16_t GPIO6DigitalRead();

   private:
    bool _c86;
    uint8_t _rd;
    uint8_t _wr;
    uint8_t _cs;
    uint8_t _rs;
    uint8_t _wait;
    unsigned int X1, Y1, X2, Y2, X3, Y3, X4, Y4;
    uint8_t taby[4] = {0};
    uint8_t tabx[4] = {0};
    uint16_t x[6], y[6], xmin, ymin, xmax, ymax;

    static uint8_t PROGMEM pic1[];
    static unsigned char PROGMEM pic[31360];
#define BL_ON digitalWrite(BL_PIN, HIGH)
#define CS_ON digitalWriteFast(_cs, LOW)
#define CS_OFF digitalWriteFast(_cs, HIGH)
#define RD_ON digitalWriteFast(_rd, LOW)
#define RD_OFF digitalWriteFast(_rd, HIGH)
#define WR_ON digitalWriteFast(_wr, LOW)
#define WR_OFF digitalWriteFast(_wr, HIGH)
#define RS_CMD digitalWriteFast(_rs, HIGH)
#define RS_DATA digitalWriteFast(_rs, LOW)
#define COMMUNICATION_DELAY delayMicroseconds(1)
    void delayus(unsigned int i);
    void Delay1ms(uint16_t i);
    void Delay10ms(uint16_t i);
    void Delay100ms(uint16_t i);
};
#endif