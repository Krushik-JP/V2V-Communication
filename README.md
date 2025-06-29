
# 🚗 Real-time Vehicle-to-Vehicle and Vehicle-to-Infrastructure Communication System

This project implements a **real-time safety system** for smart transportation using **ESP32 microcontrollers**, sensors, and Wi-Fi. It consists of two major components:

- **Vehicle-to-Vehicle (V2V)** communication — vehicles alert each other about **obstacles**, **sudden braking**, or **motion hazards**
- **Vehicle-to-Infrastructure (V2I)** communication — emergency vehicles trigger **automatic green lights** for safe passage at traffic signals

---

## 🔧 How It Works

### 🔹 Vehicle-to-Vehicle (V2V)

Each vehicle is equipped with an ESP32, sensors, and LCD. The "sending" vehicle:
- Uses **Ultrasonic Sensor** to detect nearby obstacles
- Uses **MPU6050** to monitor acceleration and braking
- Sends alerts over **Wi-Fi** to the nearby vehicle

The "receiving" vehicle:
- Receives the alert
- Displays the warning on **LCD**
- Stops the motor via **L298N driver** if needed

### 🔹 Vehicle-to-Infrastructure (V2I)

Emergency vehicles (like ambulances or fire trucks) send a special signal to the **traffic light node**, which:
- Detects the emergency
- Turns the traffic signal **green**
- Allows quick and safe passage

---

## 🧰 Technologies Used

- **ESP32** – main controller
- **Ultrasonic Sensor (HC-SR04)** – obstacle detection
- **MPU6050** – motion tracking
- **L298N Motor Driver** – wheel/motor control
- **LCD Display (I2C)** – alerts to driver
- **Wi-Fi (UDP/HTTP)** – wireless communication
- **Arduino IDE** – development

---

## 🖼️ Outputs

### 🛑Sender Vehicle A
<img src="outputs/sender.jpg/>

> Vehicle B receives the alert and displays the message

---

### 🚨 Receiver Vehicle B
<img src="outputs/reciever.jpg/>

> Traffic light turns green when an emergency vehicle is nearby

---

### ⚙️ Trffic light
<img src="outputs/traffic.jpg/>

> ESP32 nodes communicating with each other and the traffic controller


## 📁 Project Structure

- `/code`: Arduino `.ino` files for each ESP32 node
- `/hardware`: Circuit and connection diagrams
- `/outputs`: Screenshots, result images, demo GIFs
- `/report`: Final project report PDF


## 📄 License
MIT License
