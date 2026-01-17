# Smart Fish Feeder 

## General Description
This project is an automatic and smart fish feeding system based on an ESP32 microcontroller.  
The device can dispense food at scheduled times, allows configuration through a web interface, and detects when the food container is almost empty using an ultrasonic sensor.

Main features:
- Scheduled feeding (configurable from a web page)
- Servo-controlled food dispenser
- Local web interface hosted on ESP32
- Ultrasonic sensor for food level detection
- Low-food warning when the container is almost empty

The system is designed for home aquariums and helps automate daily feeding while ensuring the fish are not overfed or forgotten.

---

## System Architecture
- ESP32 hosts a web server for configuration.
- A servo motor controls the dispensing mechanism.
- An ultrasonic sensor measures the distance to the food surface.
- The ESP32 compares the measured distance with a threshold to detect low food level.
- Time synchronization is done using NTP over WiFi.

---

## Bill of Materials (BOM)

| Component | Quantity | Description |
|------------|----------|-------------|
| ESP32 | 1 | Main controller with WiFi |
| Servo Motor  | 1 | Controls food dispensing |
| Ultrasonic Sensor ) | 1 | Measures food level |
| 5V Power Supply | 1 | Power for ESP32 and servo |
| Breadboard & Jumper Wires | - | Prototyping |
| Food Container + Mechanical Parts | 1 | Dispensing mechanism |


---

## Course Questions

### Q1 – What is the system boundary?
The system includes the ESP32, servo motor, ultrasonic sensor, power supply, and web interface.  
The external environment includes the aquarium, the user, and the WiFi network.

### Q2 – Where does intelligence live?
The intelligence resides in the ESP32, which handles:
- Scheduling logic
- Servo control
- Sensor processing
- Web server and user interface
- Decision making for low food alerts

### Q3 – What is the hardest technical problem?
Ensuring reliable and repeatable food dosing and accurate low-level detection using the ultrasonic sensor, considering the irregular shape and movement of fish food.

### Q4 – What is the minimum demo?
- ESP32 connects to WiFi
- Web page allows setting feeding time
- Servo dispenses food at the scheduled time
- Ultrasonic sensor reports food level

### Q5 – Why is this not just a tutorial?
- Real-time scheduling
- A complete web UI
- Sensor-based monitoring
- Alert logic
- Custom system integration and calibration

---

## Do we need an ESP32?
Yes. The ESP32 is required to:
- Host the web interface
- Connect to WiFi
- Synchronize time via NTP
- Enable future notification features
