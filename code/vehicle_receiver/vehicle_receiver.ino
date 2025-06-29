#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define motor1Pin1 27
#define motor1Pin2 26
#define enable1Pin 14

const char* ssid = "XYZ";
const char* password = "12345678";
const int udpPort = 1234;

WiFiUDP udp;
char packetBuffer[255];
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int freq = 30000;
const int pwmChannel = 0;
const int resolution = 8;

int dutyCycle = 200;
String currentCommand = "stop";

void setup() {
    Serial.begin(115200);

    pinMode(motor1Pin1, OUTPUT);
    pinMode(motor1Pin2, OUTPUT);
    pinMode(enable1Pin, OUTPUT);
    ledcAttachChannel(enable1Pin, freq, resolution, pwmChannel);
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Connecting...");

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }

    Serial.println("Connected to WiFi");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Connected");
    udp.begin(udpPort);
}

void loop() {
    int packetSize = udp.parsePacket();
    if (packetSize) {
        int len = udp.read(packetBuffer, 255);
        if (len > 0) packetBuffer[len] = 0;
        String receivedData = String(packetBuffer);
        Serial.println("Received: " + receivedData);
        lcd.clear();

        if (receivedData == "Collision Detected!") {
            stopMotor();
            lcd.setCursor(0, 0);
            lcd.print("ALERT!");
            lcd.setCursor(0, 1);
            lcd.print("Collision Detected");
        } else if (receivedData.indexOf(',') != -1) {
            int commaIndex = receivedData.indexOf(',');
            String accel = receivedData.substring(0, commaIndex);
            String gyro = receivedData.substring(commaIndex + 1);
            lcd.setCursor(0, 0);
            lcd.print("Accel: " + accel);
            lcd.setCursor(0, 1);
            lcd.print("Gyro: " + gyro);
        } else {
            // handle movement commands
            int commaIndex = receivedData.indexOf(',');
            if (commaIndex != -1) {
                String action = receivedData.substring(0, commaIndex);
                int speed = receivedData.substring(commaIndex + 1).toInt();
                dutyCycle = speed;
                currentCommand = action;
                executeMovement();
            }
        }
    }
    delay(50);
}

void executeMovement() {
    if (currentCommand == "forward") {
        digitalWrite(motor1Pin1, HIGH);
        digitalWrite(motor1Pin2, LOW);
        ledcWrite(pwmChannel, dutyCycle);
        Serial.println("Moving Forward");
    } else if (currentCommand == "backward") {
        digitalWrite(motor1Pin1, LOW);
        digitalWrite(motor1Pin2, HIGH);
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
