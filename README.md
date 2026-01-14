# Automatic Fish Feeder

An automatic fish feeding system based on **ESP32**, configurable directly from a **smartphone via Wi-Fi**, without requiring a mobile app or cloud services.

---

## Description
This project implements a **self-contained automatic fish feeder** that dispenses a controlled amount of food at a user-defined time.  
The feeding schedule is configured through a **web interface hosted on the ESP32**, accessible from any mobile browser.

The project focuses on **reliable timing, simple IoT interaction, and clear system boundaries**, following embedded systems best practices.

---

##  Features
-  Feeding time configuration from smartphone
-  ESP32 embedded web server
-  Scheduled automatic feeding
-  Servo-based food dispensing
-  No cloud, no mobile app required
-  All logic runs on the ESP32

---

##  System Architecture

### System Boundary
**Inside the system**
- ESP32 microcontroller  
- Servo motor  
- Timing & control logic  
- Embedded web server  

**Outside the system**
- User (smartphone browser)
- Aquarium
- Fish

The ESP32 is the single point of control and decision-making.

---

##  Hardware Requirements
- ESP32 development board
- Servo motor (e.g. SG90)
- Fish food container with rotating mechanism
- External 5V power supply
- Optional:
  - LED (status indication)
  - RTC module (DS3231)

---

##  Software Stack
- Arduino framework (ESP32)
- Embedded HTTP server
- HTML/CSS for configuration page
- Servo control library
- Time source:
  - NTP (Wi-Fi) or
  - External RTC module

---

##  Web Interface
The ESP32 operates as a **Wi-Fi Access Point**.

Steps:
1. Connect phone to ESP32 Wi-Fi network
2. Open browser at `192.168.4.1`
3. Set feeding hour and minute
4. Save configuration

---

##  Feeding Logic
1. User sets feeding time via web interface
2. ESP32 stores the schedule
3. Current time is continuously monitored
4. At the scheduled time:
   - Servo rotates to release food
   - Servo returns to initial position
5. System waits for the next feeding cycle

Food quantity is controlled by the servo rotation angle and duration.

---

##  Demo
**Minimum working demo:**
- Configure feeding time a few minutes ahead
- Wait for the scheduled time
- Servo activates and dispenses food

This demonstrates full system functionality.

---

##  Limitations
- Single feeding time per day
- Food quantity depends on mechanical calibration
- No food level detection

---

##  Possible Improvements
- Multiple feeding schedules
- Manual feed button
- OLED display
- Food level sensor
- Mobile notifications



