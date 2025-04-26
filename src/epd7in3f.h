// epd7in3f.h – Waveshare 7.3" ACeP Display mit GxEPD2-kompatiblem SPI

#ifndef EPD7IN3F_H
#define EPD7IN3F_H

#include "epdif.h"

#define EPD_WIDTH       800
#define EPD_HEIGHT      480

#define EPD_7IN3F_BUFFER_SIZE (EPD_WIDTH * EPD_HEIGHT / 2)  // 4-bit pro Pixel → 2 Pixel/Byte

/**********************************
 * 7-Color Palette for 7.3" ACeP Display
 **********************************/
#define EPD_7IN3F_BLACK   0x0  // 000
#define EPD_7IN3F_WHITE   0x1  // 001
#define EPD_7IN3F_GREEN   0x2  // 010
#define EPD_7IN3F_BLUE    0x3  // 011
#define EPD_7IN3F_RED     0x4  // 100
#define EPD_7IN3F_YELLOW  0x5  // 101
#define EPD_7IN3F_ORANGE  0x6  // 110
#define EPD_7IN3F_CLEAN   0x7  // 111 → reserved


class Epd {
public:
    Epd();
    ~Epd();

    int Init(void);
    void SendCommand(uint8_t command);
    void SendData(uint8_t data);
    void WaitUntilIdle(void);
    void Reset(void);
    void Clear(uint8_t color);
    void TurnOnDisplay(void);
    void Sleep(void);
    void EPD_7IN3F_Display(uint8_t *Image);
    void EPD_7IN3F_BusyHigh(void);

private:
    unsigned int reset_pin;
    unsigned int dc_pin;
    unsigned int cs_pin;
    unsigned int busy_pin;
    unsigned long width;
    unsigned long height;    
};

#endif /* EPD7IN3F_H */
