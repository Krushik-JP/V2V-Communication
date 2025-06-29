#include <WiFi.h>
#include <WiFiUdp.h>
#include <WebServer.h>

const char* ssid = "XYZ";         // WiFi SSID
const char* password = "12345678"; // WiFi Password
const char* rsuIP = "192.168.200.144";        // RSU ESP32 IP
const int udpPort = 1234;

WiFiUDP udp;
WebServer server(80);  // Web server on port 80

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi network!");
  Serial.println(WiFi.localIP());

  // Setup web server routes
  server.on("/", handleRootPage);
  server.on("/sendEmergency", handleEmergencySend);

  server.begin();
  Serial.println("Web Server Started.");
}

void loop() {
  server.handleClient();
}

void handleRootPage() {
  String html = "<html><head><title>Emergency Control</title></head><body>";
  html += "<h2>Emergency Vehicle Control</h2>";
  html += "<form action=\"/sendEmergency\" method=\"GET\">";
  html += "<button style='height:100px;width:200px;font-size:24px;'>Send Emergency Alert</button>";
  html += "</form></body></html>";

  server.send(200, "text/html", html);
}

void handleEmergencySend() {
  Serial.println("Sending Emergency Alert to RSU...");

  udp.beginPacket(rsuIP, udpPort);
  udp.print("emergency_alert");
  udp.endPacket();

  server.sendHeader("Location", "/"); // Redirect back to home page
  server.send(303);
}
