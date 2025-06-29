#include <WiFi.h>
#include <WiFiUdp.h>
#include <WebServer.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#define TRIG_PIN 12
#define ECHO_PIN 13
#define motor1Pin1 27
#define motor1Pin2 26
#define enable1Pin 14

const char* ssid = "XYZ";
const char* password = "12345678";
const char* receiverIP = "192.168.185.252"; // Update if needed
const int udpPort = 1234;
const int freq = 30000;
const int pwmChannel = 0;
const int resolution = 8;

int dutyCycle = 200;
String currentCommand = "stop";

WiFiUDP udp;
Adafruit_MPU6050 mpu;
WebServer server(80);
bool motorRunning = false;

void setup() {
    Serial.begin(115200);

    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(motor1Pin1, OUTPUT);
    pinMode(motor1Pin2, OUTPUT);
    pinMode(enable1Pin, OUTPUT);
    ledcAttachChannel(enable1Pin, freq, resolution, pwmChannel);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    Serial.println(WiFi.localIP());
    udp.begin(udpPort);

    if (!mpu.begin()) {
        Serial.println("Failed to find MPU6050 chip");
        while (1) { delay(10); }
    }
    Serial.println("MPU6050 Found!");
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
}

void loop() {
    handleUDPCommand();

    float distance = getDistance();
    Serial.print("Distance: ");
    Serial.println(distance);

    if (distance < 10.0) {
        stopMotor();
        sendWarning();
    } else {
        executeMovement();
    }

    sendSensorData();
    delay(50);
}

float getDistance() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(5);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    long duration = pulseIn(ECHO_PIN, HIGH, 30000);
    if (duration == 0) {
        Serial.println("Warning: No Echo Received!");
        return -1;
    }
    return duration * 0.034 / 2;
}

void executeMovement() {
    if (currentCommand == "forward") {
        digitalWrite(motor1Pin1, LOW);
        digitalWrite(motor1Pin2, HIGH);
        ledcWrite(pwmChannel, dutyCycle);
        Serial.println("Moving Forward");
    } else if (currentCommand == "backward") {
        digitalWrite(motor1Pin1, HIGH);
        digitalWrite(motor1Pin2, LOW);
        ledcWrite(pwmChannel, dutyCycle);
        Serial.println("Moving Backward");
    } else if (currentCommand == "left" || currentCommand == "right") {
        digitalWrite(motor1Pin1, HIGH);
        digitalWrite(motor1Pin2, HIGH);
        ledcWrite(pwmChannel, dutyCycle / 2);
        Serial.println("Turning");
    } else {
        stopMotor();
    }
}

void stopMotor() {
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, LOW);
    ledcWrite(pwmChannel, 0);
    Serial.println("Motor Stopped!");
}

void sendWarning() {
    Serial.println("Sending Collision Warning...");
    udp.beginPacket(receiverIP, udpPort);
    udp.print("Collision Detected!");
    udp.endPacket();
}

void sendSensorData() {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    float avgAccel = (abs(a.acceleration.x) + abs(a.acceleration.y) + abs(a.acceleration.z)) / 3;
    float avgGyro = (abs(g.gyro.x) + abs(g.gyro.y) + abs(g.gyro.z)) / 3;

    String sensorData = String(avgAccel) + "," + String(avgGyro);
    udp.beginPacket(receiverIP, udpPort);
    udp.print(sensorData);
    udp.endPacket();
}

void handleUDPCommand() {
    int packetSize = udp.parsePacket();
    if (packetSize) {
        char packetBuffer[255];
        int len = udp.read(packetBuffer, 255);
        if (len > 0) packetBuffer[len] = 0;
        String command = String(packetBuffer);
        Serial.println("UDP Command: " + command);

        int commaIndex = command.indexOf(',');
        if (commaIndex != -1) {
            String action = command.substring(0, commaIndex);
            int speed = command.substring(commaIndex + 1).toInt();
            dutyCycle = speed;
            currentCommand = action;
        }
    }
}
// Code for vehicle sending data via ESP32
