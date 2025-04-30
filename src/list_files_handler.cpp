#include "list_files_handler.h"

void setupListFilesHandler(AsyncWebServer& server) {
  server.on("/list", HTTP_GET, [](AsyncWebServerRequest *request) {
    File root = SD.open("/");
    if (!root) {
      request->send(500, "text/plain", "SD open failed");
      return;
    }

    String json = "[";
    bool first = true;

    while (File entry = root.openNextFile()) {
      if (!entry.isDirectory() && String(entry.name()).endsWith(".bin")) {
        if (!first) json += ",";
        json += "\"" + String(entry.name()) + "\"";
        first = false;
      }
      entry.close();
    }
    json += "]";
    request->send(200, "application/json", json);
  });
}
