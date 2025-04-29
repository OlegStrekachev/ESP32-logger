#include <WiFi.h>             // WiFi functions
#include <WebServer.h>        // ESP32 simple web server
#include <EEPROM.h>           // EEPROM access for saved logs
#include "eeprom_logger.h"    // Access to EEPROM reading functions
#include "timezone.h"         // Timezone conversion utilities
#include <LittleFS.h>         // Filesystem for serving Bootstrap locally

WebServer server(80);         // Create a web server running on port 80 (standard HTTP port)

// --- Handle Root Web Page (HTML page) ---

// Function to serve the main web page
void handleRoot() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta charset='UTF-8'>";
  html += "<title>ESP32 Weather Logger</title>";

  // Bootstrap CSS served locally from ESP32 filesystem
  html += "<link href='/bootstrap.min.css' rel='stylesheet'>";

  // JavaScript block to fetch latest sensor data and update the table every 5 seconds
  html += "<script>";
  html += "async function fetchData() {";
  html += "  try {";
  html += "    const response = await fetch('/data');";       // Call ESP32 to get latest data
  html += "    const data = await response.json();";          // Parse JSON response
  html += "    const table = document.getElementById('logTable').getElementsByTagName('tbody')[0];";
  html += "    table.innerHTML = '';";                        // Clear old table rows
  html += "    data.forEach(item => {";                       // Insert new rows
  html += "      const row = table.insertRow();";
  html += "      row.innerHTML = `<td>${item.time}</td><td>${item.temperature} &deg;C</td><td>${item.humidity} %</td>`;";
  html += "    });";
  html += "  } catch (e) { console.error(e); }";              // Error handling
  html += "}";
  html += "window.onload = fetchData;";                       // Fetch data once on page load
  html += "setInterval(fetchData, 5000);";                    // Fetch data every 5 seconds
  html += "</script>";

  // HTML body with Bootstrap styling
  html += "</head><body class='bg-light'>";
  html += "<div class='container py-4'>";
  html += "<h1 class='mb-4'>🌡️ ESP32 Weather Logger</h1>";

  // Table for showing logged sensor data
  html += "<table class='table table-striped table-bordered' id='logTable'>";
  html += "<thead class='table-dark'><tr><th>Time</th><th>Temperature</th><th>Humidity</th></tr></thead>";
  html += "<tbody></tbody>";      // Dynamic table body will be filled by JavaScript
  html += "</table>";

  html += "</div></body></html>";

  // Send complete HTML page
  server.send(200, "text/html; charset=UTF-8", html);
}

// --- Handle /data (return recent sensor readings as JSON) ---

// Function to serve the latest 5 EEPROM entries as a JSON array
void handleData() {
  int next = getNextAddress();  // Find where the next EEPROM log will go (for backtracking)

  if (next == 0) {
    // No data available, send empty array
    server.send(200, "application/json", "[]");
    return;
  }

  String json = "[";
  int numLogs = next / LOG_SIZE;               // Calculate how many logs are currently stored
  int logsToSend = min(5, numLogs);             // Send at most 5 entries

  int addr = next - LOG_SIZE;                   // Start from the most recent log
  if (addr < 0) addr = EEPROM_SIZE - LOG_SIZE;  // Wrap around if negative

  // Build JSON array
  for (int i = 0; i < logsToSend; i++) {
    if (addr < 0) addr = EEPROM_SIZE - LOG_SIZE;  // Wrap around again if needed

    LogEntry entry;
    uint8_t* p = (uint8_t*)&entry;
    for (unsigned int j = 0; j < LOG_SIZE; j++) {
      p[j] = EEPROM.read(addr + j);              // Read bytes into LogEntry struct
    }

    // Format timestamp into readable local time
    DateTime dt(entry.timestamp);
    String timestampStr = getLocalTimestampString(dt);  // Use smart timezone handling!

    // Add comma if not the first element
    if (i > 0) json += ",";

    // Append one log as JSON object
    json += "{";
    json += "\"time\":\"" + timestampStr + "\",";
    json += "\"temperature\":" + String(entry.temperature, 1) + ",";
    json += "\"humidity\":" + String(entry.humidity, 1);
    json += "}";

    addr -= LOG_SIZE;   // Move to previous log entry
    if (addr < 0) addr = EEPROM_SIZE - LOG_SIZE;  // Wrap around
  }

  json += "]";

  // Send final JSON array
  server.send(200, "application/json; charset=UTF-8", json);
}

// --- Start the Web Server and Route Handlers ---

void startWebServer() {
  server.on("/", handleRoot);            // Serve main page at "/"
  server.on("/data", handleData);         // Serve latest sensor readings at "/data"

  // Serve Bootstrap CSS file from ESP32 LittleFS
  server.on("/bootstrap.min.css", HTTP_GET, []() {
    File file = LittleFS.open("/bootstrap.min.css", "r");  // Open file from filesystem
    server.streamFile(file, "text/css");                   // Send it as "text/css"
    file.close();                                          // Close the file after sending
  });

  server.begin();                     // Start the server
  Serial.println("✅ Web server started on port 80");
}

// --- Handle Clients in Loop ---

// Regularly called from `loop()` to handle incoming HTTP requests
void handleClient() {
  server.handleClient();  // Processes one incoming request at a time
}
