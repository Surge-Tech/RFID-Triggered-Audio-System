# RFID-Triggered-Audio-System
This project is an RFID-based audio player that uses an OLED display for user feedback. When an authorized RFID tag is scanned, a corresponding audio file is played through the DFPlayer Mini. The OLED screen provides status updates throughout the process.

## Features
OLED Display for visual feedback (Adafruit SSD1306 128x32).
RFID Reader (MFRC522) to scan RFID tags and trigger specific audio tracks.
DFPlayer Mini to play audio files stored on an SD card.
System ready and error messages are displayed on the OLED screen.

## Libraries Required:
Ensure you have the following libraries installed before uploading the code:

* Adafruit GFX Library
* Adafruit SSD1306 Library
* MFRC522 Library
* DFRobotDFPlayerMini Library

## Hardware Setup
OLED Display 128x32
RFID Reader (MFRC522)
DFPlayer Mini with an SD card containing the audio files
Microcontroller (ESP32/Arduino)

#### Pin Connections for Arduino Uno:
* OLED GND GND
* OLED VCC 5V
* OLED (SCL) SCK	A5
* OLED SDA A4
* RFID RST 9
* RFID SDA 10
* DFPlayer Mini RX 4
* DFPlayer Mini TX 5 **1kOhm resistor inbetween**.
* DFPlayer Mini BUSY 6

More pinouts for the RFID module can be found on the [RC522 GitHub](https://github.com/miguelbalboa/rfid?tab=readme-ov-file#id24).
Note: You can modify DFPlayerMini pins numbers according to your setup if needed.

## Wiring Diagram

Insert wiring diagram here. (Future update)

## How to Use
#### Prepare the RFID Tags:
Replace the 00 values in the tagUIDs array with the actual UID values of your RFID tags.
Use the DumpInfo code that comes with the RFID library to find the UID of each tag.

#### Prepare the SD Card for DFPlayer Mini:
Store the audio files on the SD card. The files should be named 0001.mp3, 0002.mp3, and so on.
Insert the SD card into the DFPlayer Mini.
SD card should be 32gb or less.

#### Upload Code:
Use the Arduino IDE to upload the code to your Arduino board.
Ensure all the libraries listed above are installed in the Arduino IDE.

#### Operation:
Power up the system.
The OLED will display system status such as "Equipment Activated" and "Ready to Scan."
Scan an RFID tag, and if the tag is recognized, the corresponding audio file will play. If the tag is not recognized, an error message will appear on the OLED.

## License:
This project is open source and licensed under the MIT License. Contributions are welcome!

## Audio Files:
Included example audio files in the audio folder. I do not own any of the sounds included in the folder.

## Examples:
The original idea for this project was to be implimented as a mobile device to scan prop audio survalence devices in an escape room. 

## Future Improvements
Implement additional features such as multi-language support for the OLED messages.
Add support for multiple audio files per tag, cycling through tracks.
Improve error handling and reporting.

Feel free to customize further or reach out with questions.
