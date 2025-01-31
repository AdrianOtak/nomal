#include <SD.h>
#include <SPI.h>
#include <GxEPD2_3C.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeMono9pt7b.h>

// Define the chip select pin for the SD card
const int CS = 5;

// Initialize the display with the correct pin assignments
GxEPD2_3C<GxEPD2_750c_Z08, GxEPD2_750c_Z08::HEIGHT> display(
    GxEPD2_750c_Z08(/*CS=*/27, /*DC=*/14, /*RST=*/12, /*BUSY=*/13));

uint8_t* bitmapData = nullptr;
int currentBitmapIndex = 0;
unsigned long lastSetupTextTime = 0;
unsigned long lastImageChangeTime = 0;
unsigned long lastScreenClearTime = 0;
unsigned long clearStartTime = 0;
bool firstImageDisplay = false;
bool isClearing = false;
File currentFile;
int totalFiles = 0;
bool case1Executed = false;
bool case2Executed = false;
bool case3Executed = false;
bool case4Executed = false;

void convertingFiles(){

    // Open the current bitmap file by dynamically reading files from the SD card
    File root = SD.open("/");
    int fileIndex = 0;

    while (File entry = root.openNextFile()) {
      if (!entry.isDirectory()) {
        if (fileIndex == currentBitmapIndex) {
          currentFile = entry; // Open the current file based on the index
          break;
        }
        fileIndex++;
      }
      entry.close();
    }

    if (currentFile) {
      free(bitmapData);
      // Allocate memory for the bitmap data
      bitmapData = (uint8_t*)malloc(currentFile.size());

      // Read the file into the bitmapData array
      int index = 0;
      while (currentFile.available() && index < currentFile.size()) {
        bitmapData[index++] = currentFile.read();
      }

      currentFile.close(); // Close the file after reading
    } else {
      Serial.println("Failed to open bitmap file");
      return;
    }
}

void displayImage() {
    display.init(115200, true, 2, false);
    display.firstPage();
    do {
        display.drawInvertedBitmap(0, 0, bitmapData, 800, 480, GxEPD_BLACK);
    } while (display.nextPage());
    Serial.println("Image displayed.");
}

void clearScreen() {
    display.init(115200, true, 2, false);
    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);
    } while (display.nextPage());
    Serial.println("Screen cleared.");
}

void firstTextDisplay()
{
  display.firstPage();
    do {
        display.println("Test");

        display.setTextColor(GxEPD_RED);  // Set text color to black
        display.setTextSize(2);  // Set text size
        display.setFont(&FreeMono9pt7b);

        // Set position and print the first line
        display.setCursor(1, 100);  // Set position for first line
        display.print("Weihnachtsgeschenk fuer Familie \nCappadonna, Kalbermatten und Ruetter");

        display.setCursor(380, 170);
        display.print(":)");

        display.setTextColor(GxEPD_BLACK);
        display.setTextSize(1);
        // Move to the next line (adjust vertical position)
        display.setCursor(0, 250);  // Move down for the second line
        display.print("-3D Printed by Mauro\n-Photoframe Design by Sumy\n-Developed by Adi");

        // Move to the next line
        display.setCursor(1, 350);  // Move down for the third line
        String charTotalFiles = String(totalFiles);
        display.print(charTotalFiles);
        display.print(" Bilder uf de SD Karte gfunde");

        // Move to the next line
        display.setCursor(480, 450);  // Move down for the third line
        display.print("Bilder werdet verarbeitet...");

    } while (display.nextPage());

    Serial.println("Setup text ausgeh");
}

void setup() {
    Serial.begin(115200);

    // Allocate initial memory for bitmapData (use a reasonable size, e.g., 1 KB)
    bitmapData = (uint8_t*)malloc(1024);

    if (!SD.begin(CS)) {
        Serial.println("SD card initialization failed!");
        return;
    }
    Serial.println("SD card initialized.");

    display.init(115200, true, 2, false);
    delay(1000);
    display.setRotation(0);
    display.setFullWindow();

    File root = SD.open("/");
    while (File entry = root.openNextFile()) {
        if (!entry.isDirectory()) {
            totalFiles++;
        }
        entry.close();
    }
    root.close();

    if (totalFiles == 0) {
        Serial.println("No files found on SD card.");
        return;
    }

    Serial.printf("Total files: %d\n", totalFiles);

    firstTextDisplay();
}

void loop() {
    static int state = 1; // Initialize the state machine
    static unsigned long stateStartTime = millis(); // Track when the state started
    unsigned long currentMillis = millis();

    switch (state) {
        case 1: // First image display after 3 minutes
            if (currentMillis - stateStartTime >= 180000 && !case1Executed) { // 3 minutes
                convertingFiles();
                displayImage();
                stateStartTime = currentMillis; // Update time for the next state
                case1Executed = true;
                state = 2; // Move to the next state
            }
            break;

        case 2: // Clear screen 10 hours after first image
            if (currentMillis - stateStartTime >= 20UL * 60 * 60 * 500 && !case2Executed) { // 10 hours
                clearScreen();
                stateStartTime = currentMillis; // Update time for the next state
                case2Executed = true;
                case3Executed = false;
                state = 3; // Move to the next state
            }
            break;

        case 3: // Redisplay the same image 10 minutes after clearing
            if (currentMillis - stateStartTime >= 20UL * 60 * 500 && !case3Executed) { // 10 minutes
                displayImage();
                stateStartTime = currentMillis; // Update time for the next state
                case3Executed = true;
                case4Executed = false;
                state = 4; // Move to the next state
            }
            break;

        case 4: // Display new image 10 hours after redisplaying
            if (currentMillis - stateStartTime >= 20UL * 60 * 60 * 500 && !case4Executed) { // 10 hours
                currentBitmapIndex++;
                currentBitmapIndex %= totalFiles;
                convertingFiles();
                displayImage();
                stateStartTime = currentMillis; // Update time for the next state
                case4Executed = true;
                case2Executed = false;
                state = 2; // Loop back to clearing logic after 8 minutes
            }
            break;

        default:
            // Reset state in case of unexpected value
            case1Executed = false;
            state = 1;
            stateStartTime = millis();
            break;
    }
}


