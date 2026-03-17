# Retro OLED Game - ESP32

A retro-style arcade game built with **ESP32**, featuring a 0.96" OLED display and physical controller inputs.

> [!IMPORTANT]  
> **Development Status:** This project is a **Work in Progress (WIP)**.  
> **Language Note:** The source code and comments are written in **Portuguese (PT-BR)**.

---

## Components Needed

* **ESP32C3 mini(or similar) ** (NodeMCU/DevKit)
* **0.96" OLED I2C** (SSD1306)
* **Analog Joystick** (X, Y and Switch)
* **Tactile Button** (Action/Shoot)
* **Passive Buzzer** (SFX)
* **Jumper Wires** & Breadboard

---

## 🔌 Pinout Configuration

| Component | ESP32C3 mini Pin | Function |
| :--- | :--- | :--- |
| **OLED SDA** | GPIO 6 | Data Line |
| **OLED SCL** | GPIO 7 | Clock Line |
| **Joystick X** | GPIO 2 | Analog Input (Movement) |
| **Joystick Y** | GPIO 3 | Analog Input (Movement) |
| **Joystick BTN** | GPIO 4 | Digital Input (Movement) |
| **Tactile Button**| GPIO 5 | Digital Input (Action) |
| **Buzzer** | GPIO 20 | PWM Output (Audio) |

---

## Project Status & Roadmap

I am using the following legend to track progress:
✅ *Implemented* | ⚠️ *WIP / Needs Improvement* | ❌ *Not yet implemented*

### **Core Mechanics**
* ✅ **Collision System:** Solid hitboxes for entities.
* ✅ **Levels/Stages:** Initial level design is functional but expanding.
* ✅ **Animations & SFX:** Basic visual effects and buzzer sounds are being refined.
* ✅ **Limited Ammo:** Resource management system.
* ✅ **Power-ups:** Special items to boost player stats.

### **Combat & Enemies**
* ❌ **Different Shot Types:** Multiple firing patterns and projectiles.
* ❌ **Bosses:** Unique large-scale encounters with specific patterns.

### **Features**
* ❌ **Local Multiplayer:** Support for a second controller/player.

---

## How to Run

1.  Ensure you have the **ESP32 board library** installed in your Arduino IDE or PlatformIO.
2.  Install the required libraries: 
    * `Adafruit SSD1306`
    * `Adafruit GFX`
3.  Connect the hardware according to the **Pinout** section above.
4.  Upload the sketch and enjoy!

---
Developed by [JaPode] <3
=======
