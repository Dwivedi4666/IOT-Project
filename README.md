# Smart Industrial Machine Guard 🛡️

A dual-mode IoT safety system designed to prevent industrial accidents caused by heavy machinery. This project utilizes an Arduino and ultrasonic distance sensing to trigger an Emergency Stop (E-Stop) when a hazard is detected, requiring a manual safety reset. 

Additionally, the system features a hidden **"Dinosaur Game"** mode for break times! 🦖

## 📖 Table of Contents
- [Problem Statement](#-problem-statement)
- [Solution Overview](#-solution-overview)
- [Key Features](#-key-features)
- [Hardware Required](#-hardware-required)
- [Circuit & Pin Configuration](#-circuit--pin-configuration)
- [How it Works](#-how-it-works)
- [The Dinosaur Game](#-bonus-the-dinosaur-game)
- [Team](#-team)

## ⚠️ Problem Statement
In factories, workers frequently suffer injuries from heavy machinery (like hydraulic presses or cutting blades) when hands or limbs accidentally enter the hazard zone while the machine is active.

## 💡 Solution Overview
The **Smart Industrial Machine Guard** solves this by implementing a **Multi-Stage Smart Guard with Manual Reset**. 

The system uses an Ultrasonic Sensor to monitor the distance between the worker and the machine. If a threshold is crossed, the system triggers an immediate lockout (E-Stop), cutting power to the machine via a relay (simulated here with LEDs/Buzzer). Critically, the machine **cannot auto-restart**; it requires a supervisor to physically inspect the area and press a Manual Reset button.

## ✨ Key Features
* **Non-Contact Sensing:** Uses HC-SR04 Ultrasonic sensor for precise distance measurement.
* **Immediate E-Stop:** Instantly triggers a lockout state when a hazard is detected.
* **Manual Restart Protocol:** Adheres to safety standards by requiring human intervention to reset the system after a stop.
* **Dual-Mode Functionality:** Switches between "Safety Guard" mode and "Dinosaur Game" mode using a push button.
* **Visual & Audio Feedback:** 16x2 LCD display, Buzzer alerts, and Status LEDs (Warning vs. Hazard).

## 🛠 Hardware Required
* **Microcontroller:** Arduino UNO
* **Sensors:** Ultrasonic Sensor (HC-SR04)
* **Display:** 16x2 LCD Display
* **Inputs:** * 2x Push Buttons (Mode Select & Game Control/Reset)
    * Potentiometer (for LCD contrast)
* **Outputs:** * Piezoelectric Buzzer
    * LEDs (Green/Red/Blue)
* **Misc:** Breadboard, Jumper Wires, Resistors (220Ω).

## 🔌 Circuit & Pin Configuration


### 1. Ultrasonic Sensor (HC-SR04)
| Pin | Function | Connection | Arduino Pin |
|:---:|:---:|:---:|:---:|
| VCC | Power | Red Wire | 5V |
| GND | Ground | Black Wire | GND |
| Trig | Input | Brown Wire | **D4** |
| Echo | Output | Purple Wire | **D3** |

### 2. Output Indicators
| Component | Function | Arduino Pin |
|:---:|:---:|:---:|
| **Buzzer** | Active Alarm | **D10** |
| **LED 1** | Warning Light | **D11** |
| **LED 2** | Hazard (Lockout) | **D12** |

### 3. Inputs (Buttons)
| Component | Function | Arduino Pin |
|:---:|:---:|:---:|
| **Button 1** | Game Jump / Action | **D2** |
| **Button 2** | Mode Toggle | **D5** |

### 4. LCD Display (4-bit Parallel Mode)
| LCD Pin | Function | Arduino Pin |
|:---:|:---:|:---:|
| RS | Register Select | **D?** (Usually D12 or configured in code) |
| E | Enable | **D?** (Usually D11 or configured in code) |
| D4 | Data Pin | **D6** |
| D5 | Data Pin | **D7** |
| D6 | Data Pin | **D8** |
| D7 | Data Pin | **D9** |
*(Note: VCC to 5V, GND to GND, V0 to Potentiometer center pin)*

---

## ⚙️ How it Works

### Safety Protocol
1.  **Monitoring:** The Ultrasonic sensor continuously measures distance.
2.  **Warning:** If an object is close, the Warning LED lights up.
3.  **Hazard Lockout:** If the object crosses the **Hazard Threshold**:
    * Buzzer rings.
    * Hazard LED flashes.
    * Machine Control Relay is deactivated (Machine Stops).
    * System enters **LOCKED OUT** state.
4.  **Reset:** The system waits. Even if the object moves away, the machine **will not start**.
5.  **Supervisor Action:** A button press is required to "Reset" the system, re-enabling the machine.

---

## 🦖 Bonus: The Dinosaur Game
The project includes a fun "Easter egg" inspired by the Chrome Dino game.

* **How to Access:** Press the **Mode Button (D5)** to toggle between Safety Mode and Game Mode.
* **How to Play:** * The dinosaur is drawn using custom characters on the LCD.
    * Obstacles (trees) scroll from right to left.
    * Press **Button 1 (D2)** to make the dinosaur jump from the bottom row to the top row to avoid collisions.
    * The game tracks your score and ends upon collision.

## 👥 Team
* Anshu Anand Jha
* Avinash Dwivedi
* Piyush Singh
* Shriya Sharma
* Abhay Khushwaha

---
*Disclaimer: This is an educational prototype. Always use certified safety equipment for real-world industrial applications.*
