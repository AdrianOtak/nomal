#include <SPI.h>
#include <SD.h>
#include "epd7in3f.h"
#include "epdif.h"

#define SD_CS 5
#define DISPLAY_CS 27
#define BUSY_PIN 13

Epd epd;

void waitUntilIdle() {
  Serial.print("Warte auf Display (BUSY HIGH)...");
  while (digitalRead(BUSY_PIN) == 0) {
    Serial.print(".");
    delay(100);
  }
  Serial.println(" bereit!");
}

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("Start...");

  // --- DISPLAY: Init + Clear ---
  digitalWrite(DISPLAY_CS, LOW);
  if (epd.Init() != 0) {
    Serial.println("Display Init fehlgeschlagen!");
    return;
  }

  Serial.println("Display wird gelöscht...");
  epd.Clear(EPD_7IN3F_WHITE);
  epd.TurnOnDisplay();  // Muss bei 7-Color aufgerufen werden

  waitUntilIdle();  // ← Wichtig!
  digitalWrite(DISPLAY_CS, HIGH);
  Serial.println("Display fertig.");

  // --- SD-Karte starten ---
  if (!SD.begin(SD_CS)) {
    Serial.println("SD Init fehlgeschlagen!");
    return;
  }
  digitalWrite(SD_CS, HIGH);
  Serial.println("SD bereit.");
}

void loop() {
  // nichts
}
