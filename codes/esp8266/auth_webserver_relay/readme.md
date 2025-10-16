I'll help you create an ESP8266 web server with authentication to control 4-channel relays. Here's a complete solution:

## Arduino Code

```cpp
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

// WiFi credentials
const char* ssid = "Your_WiFi_SSID";
const char* password = "Your_WiFi_Password";

// Create web server on port 80
ESP8266WebServer server(80);

// Relay control pins
const int relayPins[] = {D1, D2, D5, D6}; // GPIO pins for 4 relays
const int numRelays = 4;

// Authentication
const char* auth_username = "admin";
const char* auth_password = "password123";
bool isAuthenticated = false;

// HTML pages
const char* loginPage = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial; text-align: center; margin: 0 auto; padding-top: 50px; }
    .login-form { max-width: 300px; margin: 0 auto; padding: 20px; border: 1px solid #ccc; border-radius: 5px; }
    input[type="text"], input[type="password"] { width: 100%; padding: 10px; margin: 10px 0; }
    input[type="submit"] { background: #4CAF50; color: white; padding: 10px; border: none; border-radius: 5px; cursor: pointer; }
    .error { color: red; }
  </style>
</head>
<body>
  <h2>Relay Control Login</h2>
  <div class="login-form">
    <form action="/login" method="POST">
      <input type="text" name="username" placeholder="Username" required>
      <input type="password" name="password" placeholder="Password" required>
      <input type="submit" value="Login">
    </form>
    <div class="error">%ERROR%</div>
  </div>
</body>
</html>
)rawliteral";

const char* controlPage = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>4-Channel Relay Control</title>
  <style>
    body { font-family: Arial; text-align: center; margin: 0 auto; padding-top: 30px; }
    .relay-container { display: flex; flex-wrap: wrap; justify-content: center; gap: 20px; margin: 30px; }
    .relay { padding: 20px; border: 2px solid #333; border-radius: 10px; width: 150px; }
    .relay-on { background-color: #4CAF50; color: white; }
    .relay-off { background-color: #f44336; color: white; }
    button { padding: 10px 20px; font-size: 16px; margin: 5px; cursor: pointer; border: none; border-radius: 5px; }
    .on-btn { background-color: #4CAF50; color: white; }
    .off-btn { background-color: #f44336; color: white; }
    .all-btn { background-color: #2196F3; color: white; }
    .logout-btn { background-color: #ff9800; color: white; position: absolute; top: 10px; right: 10px; }
    .status { margin: 10px 0; font-weight: bold; }
  </style>
</head>
<body>
  <h2>4-Channel Relay Control</h2>
  <button class="logout-btn" onclick="logout()">Logout</button>
  
  <div>
    <button class="all-btn" onclick="controlAll('on')">All ON</button>
    <button class="all-btn" onclick="controlAll('off')">All OFF</button>
  </div>
  
  <div class="relay-container">
    <div class="relay" id="relay1">
      <h3>Relay 1</h3>
      <div class="status" id="status1">OFF</div>
      <button class="on-btn" onclick="controlRelay(1, 'on')">ON</button>
      <button class="off-btn" onclick="controlRelay(1, 'off')">OFF</button>
    </div>
    
    <div class="relay" id="relay2">
      <h3>Relay 2</h3>
      <div class="status" id="status2">OFF</div>
      <button class="on-btn" onclick="controlRelay(2, 'on')">ON</button>
      <button class="off-btn" onclick="controlRelay(2, 'off')">OFF</button>
    </div>
    
    <div class="relay" id="relay3">
      <h3>Relay 3</h3>
      <div class="status" id="status3">OFF</div>
      <button class="on-btn" onclick="controlRelay(3, 'on')">ON</button>
      <button class="off-btn" onclick="controlRelay(3, 'off')">OFF</button>
    </div>
    
    <div class="relay" id="relay4">
      <h3>Relay 4</h3>
      <div class="status" id="status4">OFF</div>
      <button class="on-btn" onclick="controlRelay(4, 'on')">ON</button>
      <button class="off-btn" onclick="controlRelay(4, 'off')">OFF</button>
    </div>
  </div>

  <script>
    function controlRelay(relayNum, action) {
      fetch(`/control?relay=${relayNum}&action=${action}`)
        .then(response => response.json())
        .then(data => {
          updateRelayStatus(relayNum, data.status);
        });
    }
    
    function controlAll(action) {
      for(let i = 1; i <= 4; i++) {
        fetch(`/control?relay=${i}&action=${action}`)
          .then(response => response.json())
          .then(data => {
            updateRelayStatus(i, data.status);
          });
      }
    }
    
    function updateRelayStatus(relayNum, status) {
      const relayElement = document.getElementById(`relay${relayNum}`);
      const statusElement = document.getElementById(`status${relayNum}`);
      
      statusElement.textContent = status.toUpperCase();
      
      if(status === 'on') {
        relayElement.className = 'relay relay-on';
      } else {
        relayElement.className = 'relay relay-off';
      }
    }
    
    function logout() {
      fetch('/logout')
        .then(() => {
          window.location.href = '/';
        });
    }
    
    // Update status on page load
    window.onload = function() {
      for(let i = 1; i <= 4; i++) {
        fetch(`/status?relay=${i}`)
          .then(response => response.json())
          .then(data => {
            updateRelayStatus(i, data.status);
          });
      }
    }
  </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  
  // Initialize EEPROM
  EEPROM.begin(512);
  
  // Initialize relay pins
  for (int i = 0; i < numRelays; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], HIGH); // Start with relays OFF (assuming active LOW)
  }
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Setup server routes
  server.on("/", handleRoot);
  server.on("/login", handleLogin);
  server.on("/control", handleControl);
  server.on("/status", handleStatus);
  server.on("/logout", handleLogout);
  
  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  if (!isAuthenticated) {
    String page = loginPage;
    page.replace("%ERROR%", "");
    server.send(200, "text/html", page);
  } else {
    server.send(200, "text/html", controlPage);
  }
}

void handleLogin() {
  if (server.method() == HTTP_POST) {
    String username = server.arg("username");
    String password = server.arg("password");
    
    if (username == auth_username && password == auth_password) {
      isAuthenticated = true;
      server.sendHeader("Location", "/");
      server.send(303);
    } else {
      String page = loginPage;
      page.replace("%ERROR%", "Invalid username or password!");
      server.send(200, "text/html", page);
    }
  }
}

void handleControl() {
  if (!isAuthenticated) {
    server.send(401, "text/plain", "Unauthorized");
    return;
  }
  
  if (server.hasArg("relay") && server.hasArg("action")) {
    int relayNum = server.arg("relay").toInt() - 1; // Convert to 0-based index
    String action = server.arg("action");
    
    if (relayNum >= 0 && relayNum < numRelays) {
      if (action == "on") {
        digitalWrite(relayPins[relayNum], LOW); // Turn relay ON
      } else if (action == "off") {
        digitalWrite(relayPins[relayNum], HIGH); // Turn relay OFF
      }
      
      // Return JSON response
      String status = (digitalRead(relayPins[relayNum]) == LOW) ? "on" : "off";
      String json = "{\"relay\":" + String(relayNum + 1) + ",\"status\":\"" + status + "\"}";
      server.send(200, "application/json", json);
      return;
    }
  }
  server.send(400, "text/plain", "Bad Request");
}

void handleStatus() {
  if (!isAuthenticated) {
    server.send(401, "text/plain", "Unauthorized");
    return;
  }
  
  if (server.hasArg("relay")) {
    int relayNum = server.arg("relay").toInt() - 1;
    
    if (relayNum >= 0 && relayNum < numRelays) {
      String status = (digitalRead(relayPins[relayNum]) == LOW) ? "on" : "off";
      String json = "{\"relay\":" + String(relayNum + 1) + ",\"status\":\"" + status + "\"}";
      server.send(200, "application/json", json);
      return;
    }
  }
  server.send(400, "text/plain", "Bad Request");
}

void handleLogout() {
  isAuthenticated = false;
  server.sendHeader("Location", "/");
  server.send(303);
}
```

## Circuit Connection

Connect your 4-channel relay module to ESP8266 as follows:

```
Relay 1 -> GPIO 5 (D1)
Relay 2 -> GPIO 4 (D2)
Relay 3 -> GPIO 14 (D5)
Relay 4 -> GPIO 12 (D6)

VCC -> 5V
GND -> GND
```

**Note:** Most relay modules are active LOW, meaning they turn ON when the control pin is LOW. If your relay module is active HIGH, change `LOW` to `HIGH` and `HIGH` to `LOW` in the code.

## Features

1. **Authentication System**: Username and password protection
2. **Responsive Design**: Works on both desktop and mobile
3. **Individual Control**: Control each relay separately
4. **Bulk Control**: Turn all relays ON/OFF at once
5. **Real-time Status**: Visual feedback of relay states
6. **Logout Functionality**: Secure session management

## Customization

1. **Change Credentials**:
   ```cpp
   const char* auth_username = "your_username";
   const char* auth_password = "your_password";
   ```

2. **Change Relay Pins**:
   ```cpp
   const int relayPins[] = {D0, D3, D4, D7}; // Your preferred GPIO pins
   ```

3. **Change WiFi Credentials**:
   ```cpp
   const char* ssid = "Your_SSID";
   const char* password = "Your_WiFi_Password";
   ```

## Security Notes

- Change the default username and password
- Consider using HTTPS for production (requires additional setup)
- The authentication is session-based but doesn't persist after ESP8266 restart

Upload this code to your ESP8266, and you'll have a secure web interface to control your 4-channel relays!



ESP8266 GPIO → 1K resistor → Base of 2N2222
Emitter of 2N2222 → GND
Collector of 2N2222 → Relay IN pin
Relay VCC → External 5V
Relay GND → External GND