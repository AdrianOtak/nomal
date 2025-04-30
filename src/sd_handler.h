#ifndef SD_HANDLER_H
#define SD_HANDLER_H

#include <Arduino.h>
#include <SD.h>
#include "epd7in3f.h"

#define IMAGE_SIZE 192000

extern File currentFile;
extern uint8_t* bitmapData;
extern int totalFiles;
extern int currentBitmapIndex;
extern Epd epd;

void loadImageFromSD();
void displaySDImage();
void countFilesFromSD();

#endif
