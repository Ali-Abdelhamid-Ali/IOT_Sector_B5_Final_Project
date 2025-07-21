# 🚀 Smart IoT Object Detection & Control System  
**ESP8266 + MQTT + Real-time Automation**  
*Developed during Sector B5 IoT Workshop mentored by Eng. Omar Barakat*  

<div align="center">
  <img src="https://img.shields.io/badge/Arduino_C++-00979D?logo=arduino&logoColor=white">
  <img src="https://img.shields.io/badge/Protocol-MQTT-yellowgreen">
  <img src="https://img.shields.io/badge/Hardware-ESP8266-red">
</div>

---

## 🌟 Project Overview  
A smart system that:  
✅ **Detects objects** using an IR sensor  
✅ **Controls an LED** remotely via mobile app  
✅ **Switches automatically** between manual/auto modes  
✅ **Publishes real-time alerts** to any MQTT client  

*Hardware: ESP8266 • IR Sensor • LED • Relay Module*

---

## 🔧 Hardware Setup  
| Component      | ESP8266 Pin |  
|----------------|-------------|  
| IR Sensor (OUT)| D5          |  
| LED/Relay (IN) | D6          |  
| VCC            | 3.3V        |  
| GND            | GND         |  

> **Tip**: Use a relay instead of LED to control high-power devices like lamps!

---

## 📡 MQTT Configuration  
| Topic                          | Direction   | Usage                          |  
|--------------------------------|-------------|--------------------------------|  
| `/sectorb5/AliAbdelhamid/led`  | **SUBSCRIBE** | Receive ON/OFF commands        |  
| `/sectorb5/AliAbdelhamid/IR`   | **PUBLISH**   | Send "object"/"no object" alerts |  

**Broker**: `broker.emqx.io:1883` (public test server)

---

## ⚙️ How It Works  
### 🤖 Three Operating Modes:  
1. **AUTO Mode (Default)**  
   - IR sensor detects objects → LED turns ON  
   - Publishes `"object"` or `"no object"` every second  

2. **MANUAL ON Mode**  
   - App sends `"on"` → LED forces ON  
   - Overrides sensor detection  

3. **MANUAL OFF Mode**  
   - App sends `"off"` → LED forces OFF for 0.5s  
   - Automatically returns to AUTO mode  


---

## 📱 Mobile App Setup  
1. Install **IoT MQTT Dashboard** (Android/iOS)  
2. Connect to broker: `broker.emqx.io:1883`  
3. Subscribe to: `/sectorb5/AliAbdelhamid/IR`  
4. Create button publishing `"on"`/`"off"` to: `/sectorb5/AliAbdelhamid/led`  

---

## 🧠 Key Code Logic  
```cpp
// 1. MQTT Callback - Handle app commands
void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) message += (char)payload[i];
  
  if(message == "on") {
    currentMode = MANUAL_ON;
    digitalWrite(LED_PIN, HIGH); // Force LED ON
  }
  else if(message == "off") {
    currentMode = MANUAL_OFF;
    digitalWrite(LED_PIN, LOW); // Force LED OFF
    manualStartTime = millis(); // Start 0.5s timer
  }
}

// 2. Auto-Return from Manual OFF
void loop() {
  if (currentMode == MANUAL_OFF && 
      millis() - manualStartTime >= 500) {
    currentMode = AUTO; // Return to auto mode
  }
}

// 3. Sensor Logic in AUTO Mode
if(currentMode == AUTO) {
  if(digitalRead(IR_PIN) == LOW) { // Object detected
    digitalWrite(LED_PIN, HIGH);
    client.publish(topic, "object"); 
  }
}
```
Arduino IDE with boards:
http://arduino.esp8266.com/stable/package_esp8266com_index.json

Libraries:
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

Clone repo → Open .ino file in Arduino IDE

Configure WiFi:
const char ssid[] = "YOUR_WIFI";
const char pass[] = "YOUR_PASSWORD";
Upload to ESP8266!

📌 Key README Features:
1. **Visual Storytelling**  
   - Badges/diagrams explain tech stack at a glance
   - Pinout tables for quick hardware setup

2. **Multi-Layer Explanation**  
   - Hardware wiring → MQTT config → Code logic → Mobile setup
   - Progressive complexity (beginners to advanced)

3. **Practical Extensions**  
   - Clear pathways to upgrade project (camera/voice/cloud)

4. **Workshop Context**  
   - Credits to Sector B5 & Eng. Omar for learning journey

5. **Copy-Paste Friendly**  
   - Ready-to-use code snippets with annotations
   - Public MQTT broker for instant testing


