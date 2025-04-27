#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>
#include "eeprom_logger.h"
#include "timezone.h"

WebServer server(80);

void handleRoot() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta charset='UTF-8'>";
  html += "<title>ESP32 Weather Logger</title>";
  html += "<link href='https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css' rel='stylesheet'>";
  
  html += "<script>";
  html += "async function fetchData() {";
  html += "  try {";
  html += "    const response = await fetch('/data');";
  html += "    const data = await response.json();";
  html += "    const table = document.getElementById('logTable').getElementsByTagName('tbody')[0];";
  html += "    table.innerHTML = '';"; // Clear table first
  html += "    data.forEach(item => {";
  html += "      const row = table.insertRow();";
  html += "      row.innerHTML = `<td>${item.time}</td><td>${item.temperature} &deg;C</td><td>${item.humidity} %</td>`;";
  html += "    });";
  html += "  } catch (e) { console.error(e); }";
  html += "}";
  html += "window.onload = fetchData;";
  html += "setInterval(fetchData, 5000);"; // Refresh every 5 seconds
  html += "</script>";

  html += "</head><body class='bg-light'>";
  html += "<div class='container py-4'>";
  html += "<h1 class='mb-4'>🌡️ ESP32 Weather Logger</h1>";

  html += "<table class='table table-striped table-bordered' id='logTable'>";
  html += "<thead class='table-dark'><tr><th>Time</th><th>Temperature</th><th>Humidity</th></tr></thead>";
  html += "<tbody></tbody>";
  html += "</table>";

  html += "</div></body></html>";

  server.send(200, "text/html; charset=UTF-8", html);
}

void handleData() {
  int next = getNextAddress();
  if (next == 0) {
    server.send(200, "application/json", "[]");
    return;
  }

  String json = "[";
  int addr = next - LOG_SIZE;
  if (addr < 0) addr = EEPROM_SIZE - LOG_SIZE;

  for (int i = 0; i < 5; i++) {  // Try to send last 5 logs
    if (addr < 0) addr = EEPROM_SIZE - LOG_SIZE;

    LogEntry entry;
    uint8_t* p = (uint8_t*)&entry;
    for (unsigned int j = 0; j < LOG_SIZE; j++) {
      p[j] = EEPROM.read(addr + j);
    }

    DateTime dt(entry.timestamp);
    char buffer[32];
    sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d",
            dt.year(), dt.month(), dt.day(),
            dt.hour() - 7 < 0 ? dt.hour() + 17 : dt.hour() - 7,
            dt.minute(), dt.second());

    if (i > 0) json += ",";  // Add comma between entries
    json += "{";
    json += "\"time\":\"" + String(buffer) + "\",";
    json += "\"temperature\":" + String(entry.temperature, 1) + ",";
    json += "\"humidity\":" + String(entry.humidity, 1);
    json += "}";

    addr -= LOG_SIZE;
    if (addr < 0) addr = EEPROM_SIZE - LOG_SIZE;
  }

  json += "]";
  server.send(200, "application/json; charset=UTF-8", json);
}

void startWebServer() {
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
  Serial.println("✅ Web server started on port 80");
}

void handleClient() {
  server.handleClient();
}
