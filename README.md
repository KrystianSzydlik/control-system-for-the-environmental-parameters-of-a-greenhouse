# Environmental Control System for Growing Tent

## 📝 Project Description
An automated environmental control system for a growing tent based on the ESP8266 microcontroller. The project enables monitoring and control of key plant growth parameters such as soil moisture, air humidity, and temperature through an intuitive web interface.

### Key Features:
- Automatic soil moisture maintenance
- Air humidity control
- Temperature monitoring
- Responsive web interface for parameter control
- Automatic watering and air humidification
- Sensor readings updated every 10 seconds

## 🛠️ Hardware Configuration

### GPIO Pin Configuration:
- `GPIO5` (D1) - DHT22 Sensor
- `GPIO14` (D5) - Water Pump Relay
- `GPIO12` (D6) - Humidifier Relay
- `A0` - Analog Soil Moisture Sensor

### Components:
- ESP8266 NodeMCU V3
- DHT22 Sensor (temperature and air humidity)
- Analog soil moisture sensor
- 2-channel relay module
- Water pump
- Mist generator (humidifier)
- 12V power supply

## 💻 Required Libraries
```cpp
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <FS.h>
```

## ⚙️ Installation and Setup

1. Clone the repository
2. Install Arduino IDE
3. Add ESP8266 support in Arduino IDE:
   - Open Preferences
   - Add URL: `http://arduino.esp8266.com/stable/package_esp8266com_index.json`
   - Install ESP8266 board through Boards Manager

4. Install required libraries:
   - ESPAsyncWebServer
   - ESPAsyncTCP
   - Adafruit Sensor
   - DHT sensor library

5. Configure WiFi parameters in the code:
```cpp
const char* ssid = "your_wifi_name";
const char* password = "your_password";
```

6. Calibrate soil moisture sensor:
```cpp
int minADC = 38;  // Value for dry soil
int maxADC = 940; // Value for wet soil
```

## 🌐 Web Interface

The interface is built using HTML and JavaScript, offering:
- Current temperature display
- Air humidity display
- Soil moisture display
- Setting threshold for automatic watering
- Setting threshold for automatic humidification
- Automatic data refresh every 10 seconds

## 🔧 System Operation

### Watering Logic
```cpp
if (mappedValue < Podlewanko && Podlewanko != 0 ){ 
  digitalWrite(relay, LOW);  // Turn pump on
} else {
  digitalWrite(relay, HIGH); // Turn pump off
}
```

### Humidification Logic
```cpp
if (h < Nawilzanko && Nawilzanko != 0 ){
  digitalWrite(relay2, LOW);  // Turn humidifier on
} else {
  digitalWrite(relay2, HIGH); // Turn humidifier off
}
```

## 📊 Technical Specifications

### Sensors
- DHT22:
  - Temperature range: -40°C to 80°C (±0.5°C)
  - Humidity range: 0-100% RH (±2-5%)
  - Reading frequency: every 10 seconds

### Soil Moisture Sensor:
- Range: 0-100% (mapped from ADC values 38-940)
- Reading frequency: continuous

## 🚀 Future Development Possibilities

1. Adding day/night cycle automation
2. Implementing data logging to SPIFFS
3. Adding measurement history graphs
4. Implementing MQTT control
5. Adding email/SMS notifications for critical values

## 📄 Licenses

This project uses several open-source components:

- ESP8266WiFi - Licensed under LGPL-2.1
- ESPAsyncWebServer - Licensed under LGPL-2.1
- ESPAsyncTCP - Licensed under LGPL-2.1
- Adafruit_Sensor - Licensed under Apache License 2.0
- DHT library - Licensed under MIT

The code in this repository, unless otherwise noted, is available under LGPL-2.1 license to maintain compatibility with the ESP8266 core libraries.

## 👥 Author
Krystian Szydlik
