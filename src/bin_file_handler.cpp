#include <ESPAsyncWebServer.h>
#include <SD.h>

void setupBinFileHandler(AsyncWebServer& server) {
  // Alle Anfragen wie /files/datei.bin werden auf /datei.bin auf der SD gemappt
  server.serveStatic("/files", SD, "/");
}
