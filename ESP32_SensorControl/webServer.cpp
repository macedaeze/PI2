#include "WebServerManager.h"
#include "WebUI.h"
#include "Config.h"
#include <WiFi.h>
#include <ESPmDNS.h>

WebServerManager::WebServerManager(int port, SensorManager* sensorMgr, ServoController* servo)
  : server(port) {
  sensors = sensorMgr;
  servoCtrl = servo;
}

void WebServerManager::begin() {
  // Registrar handlers usando lambda para acceso a miembros
  server.on("/", [this]() { this->handleRoot(); });
  server.on("/temp", [this]() { this->handleTemp(); });
  server.on("/gas", [this]() { this->handleGas(); });
  server.on("/start", [this]() { this->handleStart(); });
  server.on("/stop", [this]() { this->handleStop(); });
  server.on("/set", [this]() { this->handleSet(); });
  server.on("/status", [this]() { this->handleStatus(); });
  
  server.begin();
}

void WebServerManager::handleClient() {
  server.handleClient();
}

void WebServerManager::handleRoot() {
  server.send_P(200, "text/html", MAIN_page);
}

void WebServerManager::handleTemp() {
  float temperature;
  
  if (!sensors->readTemperature(temperature)) {
    server.send(200, "application/json", "{\"ok\":false}");
    return;
  }

  static char json[64];
  snprintf(json, sizeof(json), "{\"ok\":true,\"temperature\":%.1f}", temperature);
  server.send(200, "application/json", json);
}

void WebServerManager::handleGas() {
  int raw;
  float voltage;
  
  sensors->readGas(raw, voltage);
  
  static char json[64];
  snprintf(json, sizeof(json), "{\"raw\":%d,\"voltage\":%.3f}", raw, voltage);
  server.send(200, "application/json", json);
}

void WebServerManager::handleStart() {
  int speed = 60;
  
  if (server.hasArg("speed")) {
    speed = server.arg("speed").toInt();
  }

  servoCtrl->start(speed);
  server.send(200, "text/plain", "started");
}

void WebServerManager::handleStop() {
  servoCtrl->stop();
  server.send(200, "text/plain", "stopped");
}

void WebServerManager::handleSet() {
  if (!server.hasArg("angle")) {
    server.send(400, "text/plain", "missing angle");
    return;
  }
  
  int angle = server.arg("angle").toInt();
  servoCtrl->setAngle(angle);
  server.send(200, "text/plain", "ok");
}

void WebServerManager::handleStatus() {
  static char json[96];
  snprintf(json, sizeof(json), 
           "{\"angle\":%d,\"running\":%s,\"attached\":%s}",
           servoCtrl->getCurrentAngle(),
           servoCtrl->isRunning() ? "true" : "false",
           servoCtrl->isAttached() ? "true" : "false");
  
  server.send(200, "application/json", json);
}

bool WebServerManager::connectWiFi(const char* ssid, const char* password, unsigned long timeout) {
  Serial.println("\nConectando a WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(ssid, password);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < timeout) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Conectado! IP: ");
    Serial.println(WiFi.localIP());
    return true;
  } else {
    Serial.println("❌ No se pudo conectar a WiFi");
    return false;
  }
}