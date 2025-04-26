// epdif.cpp – GxEPD2-kompatible SPI-Verwaltung integriert

#include "epdif.h"
#include <SPI.h>

EpdIf::EpdIf() {};
EpdIf::~EpdIf() {};

int EpdIf::IfInit() {
  pinMode(CS_PIN, OUTPUT);
  pinMode(RST_PIN, OUTPUT);
  pinMode(DC_PIN, OUTPUT);
  pinMode(BUSY_PIN, INPUT);

  digitalWrite(CS_PIN, HIGH);
  digitalWrite(RST_PIN, HIGH);
  digitalWrite(DC_PIN, LOW);

  SPI.begin();  // MOSI/MISO/SCK initialisieren
  return 0;
}

void EpdIf::DigitalWrite(int pin, int value) {
  digitalWrite(pin, value);
}

int EpdIf::DigitalRead(int pin) {
  return digitalRead(pin);
}

void EpdIf::DelayMs(unsigned int delaytime) {
  delay(delaytime);
}

void EpdIf::SpiBeginTransaction() {
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));  // 2 MHz, sicher für EPD
  digitalWrite(CS_PIN, LOW);
}

void EpdIf::SpiEndTransaction() {
  digitalWrite(CS_PIN, HIGH);
  SPI.endTransaction();
}

void EpdIf::SpiTransferBatch(uint8_t* data, size_t length) {
  SpiBeginTransaction();
  for (size_t i = 0; i < length; i++) {
    SPI.transfer(data[i]);
  }
  SpiEndTransaction();
}

void EpdIf::SpiTransfer(uint8_t data) {
  SPI.transfer(data);
}
