#include "free_space_handler.h"
#include <SD.h>

void setupFreeSpaceRoute(AsyncWebServer &server) {
  server.on("/freespace", HTTP_GET, [](AsyncWebServerRequest *request) {
    uint64_t totalBytes = SD.totalBytes();
    uint64_t usedBytes = SD.usedBytes();
    uint64_t freeBytes = totalBytes - usedBytes;

    String json = "{";
    json += "\"total\":" + String(totalBytes / 1024);  // KB
    json += ",\"used\":" + String(usedBytes / 1024);   // KB
    json += ",\"free\":" + String(freeBytes / 1024);   // KB
    json += "}";

    request->send(200, "application/json", json);
  });
}
