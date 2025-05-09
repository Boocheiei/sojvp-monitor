# SOJVP Air Monitor 🌫️📟

A compact real-time air quality monitor using:
- ZPH02 or PMS7003 (PM1.0, PM2.5, PM10)
- CCS811 (VOC in ppb)
- ST7735 1.8" TFT Display
- Animated transitions for smooth visual updates

## Features
- Displays PM and VOC values with animation
- Cute air quality icon on screen
- Real sensor data via UART (PM) and I2C (VOC)

## Wiring (ESP32/Arduino)
- PM Sensor (ZPH02): `D5 = RX`, `D6 = TX`
- VOC Sensor (CCS811): `I2C (SDA/SCL)`
- Display (ST7735): `13=SCK`, `11=MOSI`, `9=DC`, `10=CS`, `8=RESET`

## Libraries Required
- [Ucglib](https://github.com/olikraus/ucglib)
- [Adafruit_CCS811](https://github.com/adafruit/Adafruit_CCS811)
- Wire
- SoftwareSerial

## Preview
