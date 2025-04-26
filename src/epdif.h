#ifndef _EPDIF_H_
#define _EPDIF_H_

#include <Arduino.h>

// Pinbelegung – an dein Setup anpassen
#define RST_PIN         25
#define DC_PIN          4
#define CS_PIN          27
#define BUSY_PIN        13

class EpdIf {
public:
    EpdIf(void);
    ~EpdIf(void);

    static int  IfInit(void);
    static void DigitalWrite(int pin, int value);
    static int  DigitalRead(int pin);
    static void DelayMs(unsigned int delaytime);

    static void SpiBeginTransaction();
    static void SpiEndTransaction();
    static void SpiTransferBatch(uint8_t* data, size_t length);
    static void SpiTransfer(uint8_t data);
};

#endif /* _EPDIF_H_ */
