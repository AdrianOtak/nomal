#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <SPI.h>
#include <SD.h>
//Display Library
#include "epd7in3f.h"
#include "epdif.h"
//Local Library
#include "sd_handler.h"
#include "upload_handler.h"
#include "free_space_handler.h"
#include "list_files_handler.h"
#include "bin_file_handler.h"

// PIN-Definitionen
#define SD_CS 5
#define DISPLAY_CS 27
#define IMAGE_SIZE 192000

// Zeiteinstellungen
#define TIME_INIT_TO_IMAGE    10000       
#define TIME_IMAGE_TO_CLEAR   300000       
#define TIME_CLEAR_TO_IMAGE   50000       

// Zustände
#define STATE_FIRST_IMAGE 1
#define STATE_CLEAR       2
#define STATE_NEXT_IMAGE  4

bool case1Executed = false;
bool case2Executed = false;
bool case3Executed = false;
bool halftime = true;

File currentFile;
uint8_t* bitmapData = nullptr;
int totalFiles = 0;
int currentBitmapIndex = 0;
Epd epd;

// WLAN-Daten
const char* ssid = "CGA2121_5ML8yKP";       // <<< HIER DEIN WLAN NAME EINTIPPEN
const char* password = "cEk3fPkjmYhfPzy4vK"; // <<< HIER DEIN WLAN PASSWORT EINTIPPEN

AsyncWebServer server(80); // Webserver Objekt

void clearScreen() {
  Serial.println("Clearing Screen...");
  epd.Clear(EPD_7IN3F_WHITE);
  Serial.println("Screen cleared");
}

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("Start...");

  // WLAN verbinden
  WiFi.begin(ssid, password);
  Serial.print("Verbinde mit WLAN...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWLAN verbunden! IP Adresse:");
  Serial.println(WiFi.localIP());

  // SD initialisieren
  if (!SD.begin(SD_CS)) {
    Serial.println("SD Init failed!");
    return;
  }
  Serial.println("SD ready.");

  // SPIFFS initialisieren
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Init fehlgeschlagen!");
    return;
  }
  Serial.println("SPIFFS bereit.");
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  // Display initialisieren
  if (epd.Init() != 0) {
    Serial.println("Display Init failed");
    return;
  }
  
  setupUploadHandler(server);
  setupListFilesHandler(server);
  setupFreeSpaceRoute(server);
  setupBinFileHandler(server);
  server.begin();
  Serial.println("Server gestartet");
  countFilesFromSD();
  clearScreen();

}

void loop() {
  static int state = STATE_FIRST_IMAGE;
  static unsigned long stateStartTime = millis();
  unsigned long currentMillis = millis();

  switch (state) {
    case STATE_FIRST_IMAGE:
      if (currentMillis - stateStartTime >= TIME_INIT_TO_IMAGE && !case1Executed) {
        Serial.println(currentBitmapIndex);
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
      if (currentMillis - stateStartTime >= (TIME_IMAGE_TO_CLEAR / 2) && halftime) {
        Serial.println("reached Halftime 1st phase");
        halftime = false;
      }

      if (currentMillis - stateStartTime >= TIME_IMAGE_TO_CLEAR && !case2Executed) {
        clearScreen();
        stateStartTime = currentMillis;
        case2Executed = true;
        case3Executed = false;
        halftime = true;
        state = STATE_NEXT_IMAGE;
      }
      break;

    case STATE_NEXT_IMAGE:
      if (currentMillis - stateStartTime >= (TIME_CLEAR_TO_IMAGE / 2) && halftime) {
        Serial.println("reached Halftime 2nd phase");
        halftime = false;
      }

      if (currentMillis - stateStartTime >= TIME_CLEAR_TO_IMAGE && !case3Executed) {
        currentBitmapIndex++;
        currentBitmapIndex %= totalFiles;
        Serial.println(currentBitmapIndex);
        loadImageFromSD();
        displaySDImage();
        stateStartTime = currentMillis;
        case3Executed = true;
        case2Executed = false;
        halftime = true;
        state = STATE_CLEAR;
      }
      break;

    default:
      case1Executed = false;
      state = STATE_FIRST_IMAGE;
      stateStartTime = millis();
      break;
  }
}
