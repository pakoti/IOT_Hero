#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

// WiFi credentials for Access Point
const char* ap_ssid = "ESP8266_Serial_Monitor";
const char* ap_password = "12345678";

// Web server authentication
const char* www_username = "admin";
const char* www_password = "admin123";

// IP Configuration
IPAddress local_IP(192, 168, 10, 1);
IPAddress gateway(192, 168, 10, 1);
IPAddress subnet(255, 255, 255, 0);

ESP8266WebServer server(80);

// String to store serial logs
String serialLogs = "";
const unsigned int MAX_LOG_LENGTH = 5000; // Maximum characters to store

// HTML page with real-time serial monitor
const char* htmlPage = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <title>ESP8266 Web Serial Monitor</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body {
      font-family: 'Courier New', monospace;
      margin: 0;
      padding: 20px;
      background-color: #1e1e1e;
      color: #00ff00;
    }
    .container {
      max-width: 100%;
      margin: 0 auto;
    }
    .header {
      text-align: center;
      margin-bottom: 20px;
      padding: 10px;
      background-color: #2d2d2d;
      border-radius: 5px;
    }
    .serial-monitor {
      background-color: #000000;
      border: 2px solid #00ff00;
      border-radius: 5px;
      padding: 15px;
      height: 60vh;
      overflow-y: auto;
      font-size: 14px;
      line-height: 1.4;
    }
    .log-entry {
      margin-bottom: 2px;
      word-wrap: break-word;
    }
    .timestamp {
      color: #888;
    }
    .controls {
      margin: 15px 0;
      text-align: center;
    }
    .btn {
      padding: 10px 20px;
      margin: 5px;
      border: none;
      border-radius: 5px;
      cursor: pointer;
      font-size: 14px;
      background-color: #00ff00;
      color: #000;
      font-weight: bold;
    }
    .btn-clear {
      background-color: #ff4444;
      color: white;
    }
    .btn-refresh {
      background-color: #4444ff;
      color: white;
    }
    .btn:hover {
      opacity: 0.8;
    }
    .status {
      text-align: center;
      margin: 10px 0;
      padding: 10px;
      background-color: #2d2d2d;
      border-radius: 5px;
    }
    .input-group {
      margin: 15px 0;
      text-align: center;
    }
    .command-input {
      width: 70%;
      padding: 10px;
      background-color: #000;
      color: #00ff00;
      border: 1px solid #00ff00;
      border-radius: 5px;
      font-family: 'Courier New', monospace;
    }
    .send-btn {
      padding: 10px 20px;
      background-color: #0088ff;
      color: white;
      border: none;
      border-radius: 5px;
      cursor: pointer;
    }
  </style>
</head>
<body>
  <div class="container">
    <div class="header">
      <h1>ESP8266 Web Serial Monitor</h1>
      <p>Real-time serial output from ESP8266</p>
      <div class="status">
        <strong>IP:</strong> %IP% | 
        <strong>Uptime:</strong> <span id="uptime">0</span> seconds |
        <strong>Heap:</strong> <span id="heap">0</span> bytes |
        <strong>Log Count:</strong> <span id="logCount">0</span>
      </div>
    </div>

    <div class="controls">
      <button class="btn" onclick="clearSerial()">Clear Serial</button>
      <button class="btn btn-refresh" onclick="refreshSerial()">Refresh Now</button>
      <button class="btn" onclick="toggleAutoRefresh()" id="autoRefreshBtn">Auto Refresh: ON</button>
    </div>

    <div class="input-group">
      <input type="text" id="commandInput" class="command-input" placeholder="Enter command to send to serial..." onkeypress="handleKeyPress(event)">
      <button class="send-btn" onclick="sendCommand()">Send</button>
    </div>

    <div id="serialOutput" class="serial-monitor">
      %SERIAL_LOGS%
    </div>
  </div>

  <script>
    let autoRefresh = true;
    let refreshInterval = 2000; // Refresh every 2 seconds

    // Function to load serial logs
    function loadSerialLogs() {
      fetch('/logs')
        .then(response => response.text())
        .then(data => {
          document.getElementById('serialOutput').innerHTML = data;
          document.getElementById('logCount').textContent = document.getElementById('serialOutput').children.length;
          scrollToBottom();
        })
        .catch(error => console.error('Error loading logs:', error));
    }

    // Function to load system info
    function loadSystemInfo() {
      fetch('/systeminfo')
        .then(response => response.json())
        .then(data => {
          document.getElementById('uptime').textContent = Math.floor(data.uptime / 1000);
          document.getElementById('heap').textContent = data.heap;
        })
        .catch(error => console.error('Error loading system info:', error));
    }

    // Function to clear serial monitor
    function clearSerial() {
      if (confirm('Are you sure you want to clear all logs?')) {
        fetch('/clear', { method: 'POST' })
          .then(() => {
            loadSerialLogs();
          });
      }
    }

    // Function to refresh serial data
    function refreshSerial() {
      loadSerialLogs();
      loadSystemInfo();
    }

    // Function to toggle auto-refresh
    function toggleAutoRefresh() {
      autoRefresh = !autoRefresh;
      const btn = document.getElementById('autoRefreshBtn');
      btn.textContent = 'Auto Refresh: ' + (autoRefresh ? 'ON' : 'OFF');
    }

    // Function to send command to serial
    function sendCommand() {
      const commandInput = document.getElementById('commandInput');
      const command = commandInput.value.trim();
      
      if (command) {
        fetch('/command', {
          method: 'POST',
          headers: {
            'Content-Type': 'application/x-www-form-urlencoded',
          },
          body: 'cmd=' + encodeURIComponent(command)
        })
        .then(response => response.text())
        .then(data => {
          console.log('Command sent:', data);
          commandInput.value = '';
          // Add command to log
          addToLog('> ' + command, 'user-command');
        });
      }
    }

    // Handle Enter key in command input
    function handleKeyPress(event) {
      if (event.key === 'Enter') {
        sendCommand();
      }
    }

    // Add command to log visually
    function addToLog(message, className) {
      const serialOutput = document.getElementById('serialOutput');
      const logEntry = document.createElement('div');
      logEntry.className = 'log-entry ' + (className || '');
      logEntry.innerHTML = '<span class="timestamp">' + new Date().toLocaleTimeString() + '</span> ' + message;
      serialOutput.appendChild(logEntry);
      scrollToBottom();
    }

    // Scroll to bottom of serial monitor
    function scrollToBottom() {
      const serialOutput = document.getElementById('serialOutput');
      serialOutput.scrollTop = serialOutput.scrollHeight;
    }

    // Auto-refresh functionality
    setInterval(() => {
      if (autoRefresh) {
        refreshSerial();
      }
    }, refreshInterval);

    // Initial load
    document.addEventListener('DOMContentLoaded', function() {
      loadSerialLogs();
      loadSystemInfo();
      scrollToBottom();
    });
  </script>
</body>
</html>
)rawliteral";

// Function to add message to serial logs
void addToSerialLog(const String& message) {
  String timestamp = "[" + String(millis()) + "ms] ";
  serialLogs += "<div class='log-entry'>" + timestamp + message + "</div>\n";
  
  // Limit log size to prevent memory issues
  if (serialLogs.length() > MAX_LOG_LENGTH) {
    int cutPosition = serialLogs.indexOf("</div>", serialLogs.length() - MAX_LOG_LENGTH / 2);
    if (cutPosition != -1) {
      serialLogs = serialLogs.substring(cutPosition + 6);
    }
  }
}

// Override Serial.print to capture logs
class WebSerialMonitor : public Print {
public:
  size_t write(uint8_t character) override {
    Serial.write(character);
    
    static String currentLine = "";
    currentLine += (char)character;
    
    if (character == '\n') {
      addToSerialLog(currentLine);
      currentLine = "";
    }
    
    return 1;
  }
  
  size_t write(const uint8_t *buffer, size_t size) override {
    Serial.write(buffer, size);
    
    for (size_t i = 0; i < size; i++) {
      write(buffer[i]);
    }
    
    return size;
  }
};

WebSerialMonitor webSerial;

// Function prototypes
bool isAuthenticated();
void handleRoot();
void handleLogs();
void handleClear();
void handleCommand();
void handleSystemInfo();
void handleNotFound();

void setup() {
  // Start hardware serial
  Serial.begin(115200);
  delay(1000);
  
  // Create Access Point
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(ap_ssid, ap_password);
  
  // Setup web server routes
  server.on("/", handleRoot);
  server.on("/logs", handleLogs);
  server.on("/clear", HTTP_POST, handleClear);
  server.on("/command", HTTP_POST, handleCommand);
  server.on("/systeminfo", handleSystemInfo);
  server.onNotFound(handleNotFound);
  
  server.begin();
  
  // Add initial log messages
  addToSerialLog("=== ESP8266 Web Serial Monitor Started ===");
  addToSerialLog("IP Address: " + WiFi.softAPIP().toString());
  addToSerialLog("SSID: " + String(ap_ssid));
  addToSerialLog("Web server started on port 80");
  
  // Test messages
  webSerial.println("Hello from Web Serial Monitor!");
  webSerial.println("System initialized successfully");
  webSerial.print("Free Heap: ");
  webSerial.println(ESP.getFreeHeap());
}

void loop() {
  server.handleClient();
  
  // Simulate some serial output for demonstration
  static unsigned long lastDemoMessage = 0;
  if (millis() - lastDemoMessage > 10000) { // Every 10 seconds
    webSerial.print("Demo message - Uptime: ");
    webSerial.print(millis() / 1000);
    webSerial.println(" seconds");
    webSerial.print("Free Heap: ");
    webSerial.println(ESP.getFreeHeap());
    lastDemoMessage = millis();
  }
}

bool isAuthenticated() {
  return server.authenticate(www_username, www_password);
}

void handleRoot() {
  if (!isAuthenticated()) {
    return server.requestAuthentication();
  }
  
  String page = htmlPage;
  page.replace("%IP%", WiFi.softAPIP().toString());
  page.replace("%SERIAL_LOGS%", serialLogs);
  
  server.send(200, "text/html", page);
}

void handleLogs() {
  if (!isAuthenticated()) {
    server.send(401, "text/plain", "Authentication Required");
    return;
  }
  
  server.send(200, "text/html", serialLogs);
}

void handleClear() {
  if (!isAuthenticated()) {
    server.send(401, "text/plain", "Authentication Required");
    return;
  }
  
  serialLogs = "";
  addToSerialLog("=== Logs cleared manually ===");
  server.send(200, "text/plain", "OK");
}

void handleCommand() {
  if (!isAuthenticated()) {
    server.send(401, "text/plain", "Authentication Required");
    return;
  }
  
  if (server.hasArg("cmd")) {
    String command = server.arg("cmd");
    addToSerialLog("> " + command);
    
    // Process commands (you can add your own command handlers here)
    if (command == "reset") {
      webSerial.println("Resetting ESP...");
      server.send(200, "text/plain", "Resetting");
      delay(1000);
      ESP.restart();
    } else if (command == "info") {
      webSerial.println("=== System Information ===");
      webSerial.print("Chip ID: "); webSerial.println(ESP.getChipId());
      webSerial.print("Flash Size: "); webSerial.println(ESP.getFlashChipSize());
      webSerial.print("SDK Version: "); webSerial.println(ESP.getSdkVersion());
      server.send(200, "text/plain", "Info displayed");
    } else {
      // Echo the command (you can modify this to execute actual commands)
      webSerial.println("Command received: " + command);
      server.send(200, "text/plain", "Command sent: " + command);
    }
  } else {
    server.send(400, "text/plain", "No command provided");
  }
}

void handleSystemInfo() {
  if (!isAuthenticated()) {
    server.send(401, "text/plain", "Authentication Required");
    return;
  }
  
  String json = "{";
  json += "\"uptime\":" + String(millis()) + ",";
  json += "\"heap\":" + String(ESP.getFreeHeap()) + ",";
  json += "\"chipId\":" + String(ESP.getChipId());
  json += "}";
  
  server.send(200, "application/json", json);
}

void handleNotFound() {
  server.send(404, "text/plain", "File Not Found\n");
}