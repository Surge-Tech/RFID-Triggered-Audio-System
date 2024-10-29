// Initializing necessary libraries
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// Declaration for OLED Screen
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define LOGO_HEIGHT 16
#define LOGO_WIDTH 16
static const unsigned char PROGMEM logo_bmp[] =
{ 0b00000000, 0b11000000,
  0b00000001, 0b11000000,
  0b00000001, 0b11000000,
  0b00000011, 0b11100000,
  0b11110011, 0b11100000,
  0b11111110, 0b11111000,
  0b01111110, 0b11111111,
  0b00110011, 0b10011111,
  0b00011111, 0b11111100,
  0b00001101, 0b01110000,
  0b00011011, 0b10100000,
  0b00111111, 0b11100000,
  0b00111111, 0b11110000,
  0b01111100, 0b11110000,
  0b01110000, 0b01110000,
  0b00000000, 0b00110000 };

// Declaration for RFID scanner
#define RST_PIN 9 // Configurable, see typical pin layout above
#define SS_PIN 10 // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

// Define RFID tag UID
byte tagUIDs[14][4] = {
  //  EXAMPLE: {0xA3, 0xF6, 0x85, 0x06}
  {0xF9, 0x9D, 0xF0, 0x00}, // Primary 1
  {0xFB, 0x47, 0x10, 0x00}, // Primary 2
  {0x99, 0x9C, 0xF1, 0x00}, // Primary 3
  {0xB5, 0x70, 0xF0, 0x00}, // Primary 4
  {0x17, 0x97, 0xF0, 0x00}, // Primary 5
  {0x57, 0x11, 0xF1, 0x00}, // Primary 6
  {0x5E, 0x37, 0x20, 0x00}, // Primary 7
  // System is made for 7 main and 7 backups
  // Backups are not required
  {0x00, 0x00, 0x00, 0x00}, // Backup 1
  {0x00, 0x00, 0x00, 0x00}, // Backup 2
  {0x00, 0x00, 0x00, 0x00}, // Backup 3
  {0x00, 0x00, 0x00, 0x00}, // Backup 4
  {0x00, 0x00, 0x00, 0x00}, // Backup 5
  {0x00, 0x00, 0x00, 0x00}, // Backup 6
  {0x00, 0x00, 0x00, 0x00}, // Backup 7
};

// DF Player Mini
static const uint8_t DF_TX = 5;
static const uint8_t DF_RX = 4;
int BUSY_PIN = 6; // To check is audio is playing
int busyState = 0;

SoftwareSerial mySoftwareSerial(DF_RX, DF_TX);
DFRobotDFPlayerMini myDFPlayer;


void setup() {
  // put your setup code here, to run once:
  pinMode(BUSY_PIN, INPUT);

  // Initialize serial communications with the DFPlayer
  mySoftwareSerial.begin(9600);

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
  }

  SPI.begin(); // Init SPI bus
	mfrc522.PCD_Init();	// Init MFRC522
	delay(400);	// Optional delay. Some board do need more time after init to be ready

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(1000); // Pause for 1 seconds
  // Clear the buffer
  display.clearDisplay();
  delay(1000);

  // Set text properties
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  printOLED("Equipment Activated.");
  delay(1000);

  // Initialize the DF Player
  myDFPlayer.begin(mySoftwareSerial, true, false);
  myDFPlayer.volume(5); // Set volume value (0-30)

  // Write on screen that everything is working
  printOLED("Audio System Online.");
  delay(1000);
  
  printOLED("Ready to Scan.");
}


void loop() {
  // put your main code here, to run repeatedly:
  busyState = digitalRead(BUSY_PIN);
  if (!busyState == LOW) {
    // wait for audio to stop playing
    return;
  }

  // Check for new RFID tag
	// Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle
  if (!mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	// Select one of the cards
	if (!mfrc522.PICC_ReadCardSerial()) {
		return;
	}

  // Compare the scanned RFID UID with the predefined ones
  int tagIndex = findTagIndex(mfrc522.uid.uidByte);
  if (tagIndex != -1) {
    // Valid RFID tag detected, play corresponding audio
    printOLED("Signal Detected.");
    delay(500);
    playAudio(tagIndex % 7 + 1);  // %7 ensures backup tags map to same file
  } else {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("ERROR: Unrecognized.");
    display.println("Tag compromised.");
    display.display();
  }

  mfrc522.PICC_HaltA();  // Stop reading the RFID tag

  // Add a short delay after each scan
  delay(500);

} // End void loop


// Function to find the index of the scanned RFID UID in the array
int findTagIndex(byte *scannedUID) {
  for (int i = 0; i < 14; i++) {
    if (compareUID(scannedUID, tagUIDs[i])) {
      return i;
    }
  }
  return -1;  // Return -1 if the tag is not found
}

// Function to compare two UIDs
bool compareUID(byte *uid1, byte *uid2) {
  for (int i = 0; i < 4; i++) {
    if (uid1[i] != uid2[i]) {
      return false;
    }
  }
  return true;
}

// Function to play an audio file
void playAudio(int trackNumber) {
  printOLED("Playing from tag #");
  display.println(trackNumber);
  display.display();
  myDFPlayer.play(trackNumber);  // Play specific track
}

// Function to print to screen
void printOLED(String message) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(message);
  display.display();
}
