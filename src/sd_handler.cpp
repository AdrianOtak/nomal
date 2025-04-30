#include "sd_handler.h"
#include <SD.h>
#include "epd7in3f.h"

extern Epd epd;
extern File currentFile;
extern uint8_t* bitmapData;
extern int currentBitmapIndex;
extern int totalFiles;

#define IMAGE_SIZE 192000

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
    Serial.println("Error: file size mismatch");
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

  epd.TurnOnDisplay();

  if (bitmapData) {
    free(bitmapData);
    bitmapData = nullptr;
  }

  Serial.println("Image display completed");
}

void countFilesFromSD() {
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
}


