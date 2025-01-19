#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

// Define the relay pins
const int relayPin1 = 12;  // D6
const int relayPin2 = 13;  // D7
const int relayPin3 = 14;  // D5
const int relayPin4 = 15;  // D8

// Define the relay states
bool relayState1 = false;
bool relayState2 = false;
bool relayState3 = false;
bool relayState4 = false;

// Define the WiFi credentials
const char* ssid = "your_ssid";
const char* password = "your_password";

// Create an instance of the web server
ESP8266WebServer server(80);

void setup() {
  // Initialize the serial console
  Serial.begin(115200);

  // Connect to the WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  // Print the IP address of the ESP8266
  Serial.println("Connected to WiFi. IP address: ");
  Serial.println(WiFi.localIP());

  // Initialize the relay pins as outputs
  pinMode(relayPin1, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  pinMode(relayPin3, OUTPUT);
  pinMode(relayPin4, OUTPUT);

  // Start the web server
  server.on("/", handleRoot);
  server.on("/relay1/on", handleRelay1On);
  server.on("/1/off", handleRelay1Off);
  server.on("/2/on", handleRelay2On);
  server.on("/2/off", handleRelay2Off);
  server.on("/3/on", handleRelay3On);
  server.on("/3/off", handleRelay3Off);
  server.on("/4/on", handleRelay4On);
  server.on("/4/off", handleRelay4Off);
  server.begin();
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  server.send(200, "text/html", "<html><body><h1>Relay Control</h1><a href=\"/1/on\">Relay 1 On</a><br><a href=\"/1/off\">Relay 1 Off</a><br><a href=\"/2/on\">Relay 2 On</a><br><a href=\"/2/off\">Relay 2 Off</a><br><a href=\"/3/on\">Relay 3 On</a><br><a href=\"/3/off\">Relay 3 Off</a><br><a href=\"/4/on\">Relay 4 On</a><br><a href=\"/4/off\">Relay 4 Off</a></body></html>");
}

void handleRelay1On() {
  relayState1 = true;
  digitalWrite(relayPin1, HIGH);
  server.send(200, "text/plain", "Relay 1 On");
}

void handleRelay1Off() {
  relayState1 = false;
  digitalWrite(relayPin1, LOW);
  server.send(200, "text/plain", "Relay 1 Off");
}

void handleRelay2On() {
  relayState2 = true;
  digitalWrite(relayPin2, HIGH);
  server.send(200, "text/plain", "Relay 2 On");
}

void handleRelay2Off() {
  relayState2 = false;
  digitalWrite(relayPin2, LOW);
  server.send(200, "text/plain", "Relay 2 Off");
}

void handleRelay3On() {
  relayState3 = true;
  digitalWrite(relayPin3, HIGH);
  server.send(200, "text/plain", "Relay 3 On");
}

void handleRelay3Off() {
  relayState3 = false;
  digitalWrite(relayPin3, LOW);
  server.send(200, "text/plain", "Relay 3 Off");
}

void handleRelay4On() {
  relayState4 = true;
  digitalWrite(relayPin4, HIGH);
  server.send(200, "text/plain", "Relay 4 On");
}

void handleRelay4Off() {
  relayState4 = false;
  digitalWrite(relayPin4, LOW);
  server.send(200, "text/plain", "Relay 4 Off");
}