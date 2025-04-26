// epd7in3f.cpp – Angepasst für GxEPD2-kompatibles SPI-Handling

#include "epd7in3f.h"
#include "epdif.h"
#include "SPI.h"

Epd::Epd() {
    reset_pin = RST_PIN;
    dc_pin = DC_PIN;
    cs_pin = CS_PIN;
    busy_pin = BUSY_PIN;
    width = EPD_WIDTH;
    height = EPD_HEIGHT;
};

Epd::~Epd() {
}

int Epd::Init(void) {
    Serial.println("start init");
    if (EpdIf::IfInit() != 0) {
        return -1;
    }
    Serial.println("start reset");

    Reset();
    Serial.println("reset fertig");
    delay(20);
    EPD_7IN3F_BusyHigh();

    SendCommand(0xAA);    // CMDH
    SendData(0x49);
    SendData(0x55);
    SendData(0x20);
    SendData(0x08);
    SendData(0x09);
    SendData(0x18);

    SendCommand(0x01);
    SendData(0x3F);
    SendData(0x00);
    SendData(0x32);
    SendData(0x2A);
    SendData(0x0E);
    SendData(0x2A);

    SendCommand(0x00);
    SendData(0x5F);
    SendData(0x69);

    SendCommand(0x03);
    SendData(0x00);
    SendData(0x54);
    SendData(0x00);
    SendData(0x44); 

    SendCommand(0x05);
    SendData(0x40);
    SendData(0x1F);
    SendData(0x1F);
    SendData(0x2C);

    SendCommand(0x06);
    SendData(0x6F);
    SendData(0x1F);
    SendData(0x1F);
    SendData(0x22);

    SendCommand(0x08);
    SendData(0x6F);
    SendData(0x1F);
    SendData(0x1F);
    SendData(0x22);

    SendCommand(0x13);    // IPC
    SendData(0x00);
    SendData(0x04);

    SendCommand(0x30);
    SendData(0x3C);

    SendCommand(0x41);     // TSE
    SendData(0x00);

    SendCommand(0x50);
    SendData(0x3F);

    SendCommand(0x60);
    SendData(0x02);
    SendData(0x00);

    SendCommand(0x61);
    SendData(0x03);
    SendData(0x20);
    SendData(0x01); 
    SendData(0xE0);

    SendCommand(0x82);
    SendData(0x1E); 

    SendCommand(0x84);
    SendData(0x00);

    SendCommand(0x86);    // AGID
    SendData(0x00);

    SendCommand(0xE3);
    SendData(0x2F);

    SendCommand(0xE0);   // CCSET
    SendData(0x00); 

    SendCommand(0xE6);   // TSSET
    SendData(0x00);

    return 0;
}


void Epd::SendCommand(uint8_t command) {
    EpdIf::SpiBeginTransaction();
    EpdIf::DigitalWrite(DC_PIN, LOW);
    EpdIf::SpiTransfer(command);
    EpdIf::SpiEndTransaction();
}

void Epd::SendData(uint8_t data) {
    EpdIf::SpiBeginTransaction();
    EpdIf::DigitalWrite(DC_PIN, HIGH);
    EpdIf::SpiTransfer(data);
    EpdIf::SpiEndTransaction();
}

void Epd::WaitUntilIdle(void) {
    while (EpdIf::DigitalRead(BUSY_PIN) == 0) {
        delay(100);
    }
}

void Epd::Reset(void) {
    EpdIf::DigitalWrite(RST_PIN, HIGH);
    delay(20);
    EpdIf::DigitalWrite(RST_PIN, LOW);
    delay(1);
    EpdIf::DigitalWrite(RST_PIN, HIGH);
    delay(20);
}

void Epd::Clear(uint8_t color) {
    SendCommand(0x10);
    for(int i=0; i<width/2; i++) {
        for(int j=0; j<height; j++) {
            SendData((color<<4)|color);
		}
	}
    
    TurnOnDisplay();
}

void Epd::TurnOnDisplay(void) {
    SendCommand(0x04);  // POWER_ON
    EPD_7IN3F_BusyHigh();
    
    SendCommand(0x12);  // DISPLAY_REFRESH
    SendData(0x00);
    EPD_7IN3F_BusyHigh();
    
    SendCommand(0x02);  // POWER_OFF
    SendData(0x00);
    EPD_7IN3F_BusyHigh();
}

void Epd::Sleep(void) {
    SendCommand(0x07);
    SendData(0xA5);
    delay(10);
	EpdIf::DigitalWrite(RST_PIN, 0); // Reset
}

void Epd::EPD_7IN3F_BusyHigh(void) {
    Serial.println("Display Busy");
    while(EpdIf::DigitalRead(BUSY_PIN) == 0) {
        delay(10);
    }
}

void Epd::EPD_7IN3F_Display(uint8_t *Image) {
    SendCommand(0x10);
    for (uint32_t i = 0; i < EPD_7IN3F_BUFFER_SIZE; i++) {
        SendData(Image[i]);
    }
    TurnOnDisplay();
}
