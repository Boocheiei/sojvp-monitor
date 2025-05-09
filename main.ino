#include <SoftwareSerial.h>
#include "Ucglib.h"
#include <Wire.h>
#include "Adafruit_CCS811.h"

// CCS811 (VOC Sensor)
Adafruit_CCS811 ccs;

// SoftwareSerial for particulate matter (PM) sensor (ZPH02/PMS7003)
SoftwareSerial mySerial(5, 6);  // RX, TX

// ST7735 1.8-inch TFT display
Ucglib_ST7735_18x128x160_SWSPI ucg(13, 11, 9, 10, 8);

// Particulate matter (PM) sensor readings
unsigned int pm1 = 0, last_pm1 = 0;
unsigned int pm2_5 = 0, last_pm2_5 = 0;
unsigned int pm10 = 0, last_pm10 = 0;

// Volatile Organic Compound (VOC) sensor reading
int voc = 0;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  Wire.begin();  // I2C communication for CCS811

  // Initialize the TFT display
  ucg.begin(UCG_FONT_MODE_TRANSPARENT);
  ucg.clearScreen();
  ucg.setColor(0, 0, 0);
  ucg.drawBox(0, 0, 128, 160);  // Draw a background box
  drawAirQualityIcon(54, 15);   // Draw an air quality icon
  displayStaticText();          // Display static labels for PM and VOC readings

  // Initialize the CCS811 sensor (VOC sensor)
  if (!ccs.begin()) {
    Serial.println("Failed to start CCS811. Please check wiring.");
    while (1);  // Halt if sensor initialization fails
  }

  // Wait for the CCS811 sensor to be ready
  while (!ccs.available()) delay(100);
}

void loop() {
  // Continuously read sensor data
  readSensorData();
  
  // Display the particulate matter (PM) values and VOC readings
  displayPMValues();
  
  // Delay to reduce screen update frequency (1 second)
  delay(1000);
}

void readSensorData() {
  int index = 0;
  char value, previousValue;

  // Read data from PM sensor (ZPH02/PMS7003)
  while (mySerial.available()) {
    value = mySerial.read();
    // Validate the data header for PM sensor
    if ((index == 0 && value != 0x42) || (index == 1 && value != 0x4D)) break;

    // Parse PM sensor data
    if (index == 4 || index == 6 || index == 8) {
      previousValue = value;
    } else if (index == 5) {
      pm1 = 256 * previousValue + value;
    } else if (index == 7) {
      pm2_5 = 256 * previousValue + value;
    } else if (index == 9) {
      pm10 = 256 * previousValue + value;
    } else if (index > 15) break;
    index++;
  }
  while (mySerial.available()) mySerial.read();  // Clear remaining data

  // Read data from the CCS811 VOC sensor
  if (ccs.available()) {
    if (!ccs.readData()) {
      voc = ccs.getTVOC();  // Get VOC concentration in ppb
    }
  }
}

void displayStaticText() {
  ucg.setColor(255, 255, 255);  // Set text color to white
  ucg.setFont(ucg_font_helvR08_tr);  // Set font size
  ucg.setPrintPos(5, 50); ucg.print("PM1   :");  // Display static label for PM1
  ucg.setPrintPos(5, 70); ucg.print("PM2.5 :");  // Display static label for PM2.5
  ucg.setPrintPos(5, 90); ucg.print("PM10  :");  // Display static label for PM10
  ucg.setPrintPos(5, 110); ucg.print("VOC   :");  // Display static label for VOC
}

void displayPMValues() {
  ucg.setFont(ucg_font_helvR10_tr);  // Set font size for PM values display

  // Display PM1.0 value with dynamic animation
  if (pm1 != last_pm1) {
    animateValueChange(60, 50, last_pm1, pm1);
    last_pm1 = pm1;
  }
  
  // Display PM2.5 value with dynamic animation
  if (pm2_5 != last_pm2_5) {
    animateValueChange(60, 70, last_pm2_5, pm2_5);
    last_pm2_5 = pm2_5;
  }
  
  // Display PM10 value with dynamic animation
  if (pm10 != last_pm10) {
    animateValueChange(60, 90, last_pm10, pm10);
    last_pm10 = pm10;
  }

  // Display VOC value
  ucg.setColor(0, 0, 0);  // Set background color to black for VOC display
  ucg.drawBox(60, 100, 70, 15);  // Draw a box for the VOC value
  ucg.setColor(255, 255, 255);  // Set text color to white
  ucg.setPrintPos(60, 110);     // Set position to display VOC
  ucg.print(voc);               // Display VOC value
  ucg.print(" ppb");            // Display VOC unit (ppb)
}

void animateValueChange(int x, int y, int oldVal, int newVal) {
  // Animate the change in values for PM readings
  for (int i = oldVal; i != newVal; i += (oldVal < newVal ? 1 : -1)) {
    ucg.setColor(0, 0, 0);  // Set background color to black
    ucg.drawBox(x, y - 10, 70, 15);  // Clear the old value box
    ucg.setColor(255, 255, 255);  // Set text color to white
    ucg.setPrintPos(x, y);  // Set print position
    ucg.print(i);           // Display intermediate value
    ucg.print(" ug/m3");    // Display PM unit (ug/m3)
    delay(10);              // Delay for smooth animation
  }

  // Display the final value after animation
  ucg.setColor(0, 0, 0);    // Set background color to black
  ucg.drawBox(x, y - 10, 70, 15);  // Clear the old value box
  ucg.setColor(255, 255, 255);     // Set text color to white
  ucg.setPrintPos(x, y);           // Set print position
  ucg.print(newVal);               // Display the final value
  ucg.print(" ug/m3");             // Display PM unit (ug/m3)
}

void drawAirQualityIcon(int x, int y) {
  // Draw an air quality indicator icon on the display
  ucg.setColor(153, 204, 255);  // Set color to light blue for the icon
  ucg.drawDisc(x, y, 12, UCG_DRAW_ALL);  // Draw a filled circle for the icon's background
  ucg.setColor(0, 0, 0);  // Set color to black for icon details
  ucg.drawDisc(x - 4, y - 3, 1, UCG_DRAW_ALL);  // Draw eyes for the icon
  ucg.drawDisc(x + 4, y - 3, 1, UCG_DRAW_ALL);  // Draw eyes for the icon
  
  // Draw mouth for the icon
  for (int i = -5; i <= 5; i++) {
    ucg.drawPixel(x + i, y + 4 + i * i / 10);
  }
  
  // Draw the nose for the icon
  ucg.setColor(0, 200, 0);  // Set color to green for the nose
  ucg.drawDisc(x - 6, y - 12, 3, UCG_DRAW_ALL);  // Draw left nostril
  ucg.drawDisc(x + 6, y - 12, 3, UCG_DRAW_ALL);  // Draw right nostril
  ucg.drawLine(x - 6, y - 12, x, y - 8);         // Draw lines to represent the nose
  ucg.drawLine(x + 6, y - 12, x, y - 8);         // Draw lines to represent the nose
}
