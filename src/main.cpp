#include <SPI.h>
#include <SD.h>
#include "epd7in3f.h"
#include "epdif.h"

#define SD_CS 5
#define DISPLAY_CS 27
#define IMAGE_SIZE 192000

#define TIME_INIT_TO_IMAGE    10000       // 10 Sekunden
#define TIME_IMAGE_TO_CLEAR   300000       // 10 Stunden
#define TIME_CLEAR_TO_IMAGE   300000       // 10 Minuten

#define STATE_FIRST_IMAGE 1
#define STATE_CLEAR       2
#define STATE_REDISPLAY   3
#define STATE_NEXT_IMAGE  4

Epd epd;
File currentFile;
uint8_t* bitmapData = nullptr;
int currentBitmapIndex = 0;
int totalFiles = 0;
bool case1Executed = false;
bool case2Executed = false;
bool case3Executed = false;
bool case4Executed = false;
bool halftime = true;

void loadImageFromSD() {
  Serial.println("loading image");

  File root = SD.open("/");
  if (!root) {
    Serial.println("failed opening root!");
    return;
  }
  Serial.println("Root opened.");

  int fileIndex = 0;
  while (File entry = root.openNextFile()) {
    if (!entry.isDirectory() && String(entry.name()).endsWith(".bin")) {
        totalFiles++;
        Serial.print("File found: ");
        Serial.println(entry.name());

      if (fileIndex == currentBitmapIndex) {
        currentFile = entry;  
        break;
      }
      fileIndex++;
    }
    entry.close();
  }

  if (!currentFile || !currentFile.available()) {
    Serial.println("no valid file found!");
    return;
  }

  if (bitmapData) free(bitmapData);

  bitmapData = (uint8_t*)ps_malloc(IMAGE_SIZE);
  if (!bitmapData) {
    Serial.println("Not enough PSRAM!");
    return;
  }

  if (currentFile.size() > IMAGE_SIZE) {
    Serial.println("File too large!");
    return;
  }
  
  size_t bytesRead = currentFile.read(bitmapData, IMAGE_SIZE);
  currentFile.close();

  if (bytesRead != IMAGE_SIZE) {
    Serial.println("Error *found file size does not match with display size");
    free(bitmapData);
    bitmapData = nullptr;
    return;
  }

  Serial.print("Image loaded (");
  Serial.print(bytesRead);
  Serial.print(" Bytes | ");
  Serial.print(bytesRead / 1000);
  Serial.println(" KB)");
}


void displaySDImage() {
  if (!bitmapData) return;

  epd.SendCommand(0x10);
  for (unsigned int i = 0; i < IMAGE_SIZE; i++) {
    epd.SendData(bitmapData[i]);
  }

  if (bitmapData) {
    free(bitmapData);
    bitmapData = nullptr;
  }

  Serial.println("Image display completed");
}

void clearScreen(){
  Serial.println("Clearing Screen...");
  epd.Clear(EPD_7IN3F_WHITE);
  Serial.println("Screen cleared");
}

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("Start...");

  // Init SD
  if (!SD.begin(SD_CS)) {
    Serial.println("SD Init failed!");
    return;
  }
  Serial.println("SD ready.");

  // Datei zählen
  File root = SD.open("/");
  while (File entry = root.openNextFile()) {
    if (!entry.isDirectory() && String(entry.name()).endsWith(".bin")) {
      totalFiles++;
      Serial.print("File found: ");
      Serial.println(entry.name());
    }
    entry.close();
  }
  root.close();

  if (totalFiles == 0) {
    Serial.println("No files found on SD");
    return;
  }
  Serial.printf("Total files: %d\n", totalFiles);

  // Init Display
  if (epd.Init() != 0) {
    Serial.println("Display Init failed");
    return;
  }
  
  clearScreen();
}

void loop() {
  static int state = STATE_FIRST_IMAGE;
  static unsigned long stateStartTime = millis();
  unsigned long currentMillis = millis();

  switch (state) {
    case STATE_FIRST_IMAGE:
      if (currentMillis - stateStartTime >= TIME_INIT_TO_IMAGE && !case1Executed) {
        loadImageFromSD();
        displaySDImage();
        stateStartTime = currentMillis;
        case1Executed = true;
        case2Executed = false;
        halftime = true;
        state = STATE_CLEAR;
      }
      break;

    case STATE_CLEAR:
      if (currentMillis - stateStartTime >= (TIME_IMAGE_TO_CLEAR / 2) && halftime){
        Serial.println("reached Halftime");
        halftime = false;
      }

      if (currentMillis - stateStartTime >= TIME_IMAGE_TO_CLEAR && !case2Executed) {
        clearScreen();
        stateStartTime = currentMillis;
        case2Executed = true;
        case3Executed = false;
        halftime = true;
        state = STATE_REDISPLAY;
      }
      break;

    case STATE_REDISPLAY:
      if (currentMillis - stateStartTime >= (TIME_CLEAR_TO_IMAGE / 2) && halftime){
        Serial.println("reached Halftime");
        halftime = false;
      }

      if (currentMillis - stateStartTime >= TIME_CLEAR_TO_IMAGE && !case3Executed) {
        displaySDImage();
        stateStartTime = currentMillis;
        case3Executed = true;
        case4Executed = false;
        halftime = true;
        state = STATE_NEXT_IMAGE;
      }
      break;

    case STATE_NEXT_IMAGE:
      if (currentMillis - stateStartTime >= (TIME_IMAGE_TO_CLEAR / 2) && halftime){
        Serial.println("reached Halftime");
        halftime = false;
      }

      if (currentMillis - stateStartTime >= TIME_IMAGE_TO_CLEAR && !case4Executed) {
        currentBitmapIndex++;
        currentBitmapIndex %= totalFiles;
        loadImageFromSD();
        displaySDImage();
        stateStartTime = currentMillis;
        case4Executed = true;
        case2Executed = false;
        halftime = true;
        state = STATE_CLEAR;
      }
      break;

    default:
      case1Executed = false;
      state = 1;
      stateStartTime = millis();
      break;
  }
}
