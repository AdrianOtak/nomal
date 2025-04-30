#include "upload_handler.h"
#include <SD.h>

void setupUploadHandler(AsyncWebServer &server) {
    server.on("/upload", HTTP_POST,
        [](AsyncWebServerRequest *request) { },
        [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
            static File uploadFile;
            static String finalName;

            if (index == 0) {
                String name = filename;
                if (!name.endsWith(".bin")) name += ".bin";

                int counter = 0;
                finalName = "/" + name;
                while (SD.exists(finalName)) {
                    counter++;
                    finalName = "/" + name.substring(0, name.lastIndexOf(".")) + String(counter) + ".bin";
                }

                Serial.println("Starte Upload: " + finalName);
                uploadFile = SD.open(finalName, FILE_WRITE);
                if (!uploadFile) {
                    Serial.println("Fehler beim Öffnen der Datei auf SD!");
                    request->send(500, "text/plain", "Fehler beim Öffnen");
                    return;
                }
            }

            if (uploadFile) {
                uploadFile.write(data, len);
            }

            if (final) {
                if (uploadFile) {
                    uploadFile.close();
                    Serial.println("Upload abgeschlossen: " + finalName);
                }
                request->send(200, "text/plain", finalName);
            }
        }
    );
}
