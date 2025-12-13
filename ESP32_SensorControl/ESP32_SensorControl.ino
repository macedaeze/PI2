/*
 * ESP32 Sensor & Servo Control System
 * 
 * Proyecto Integrador 2 - Ingeniería en Sistemas Espaciales
 * Team Pipinas: Ailin Ferrari, Emiliano Vitale, Ezequiel Maceda, Johanna Olivera
 * 
 * Estructura modular profesional:
 * - Config.h: Configuración centralizada
 * - Sensors.h/.cpp: Gestión de DS18B20 y MQ135
 * - ServoControl.h/.cpp: Control del servomotor MG996R
 * - WebServerManager.h/.cpp: Servidor web y API REST
 * - WebUI.h: Interfaz HTML embebida
 */

#include "config.h"
#include "sensores.h"
#include "ServoControl.h"
#include "WebServerManager.h"
#include <ESPmDNS.h>

// Instancias de los componentes
SensorManager sensors(ONE_WIRE_BUS, MQ135_PIN);
ServoController servoCtrl(SERVO_PIN);
WebServerManager webServer(SERVER_PORT, &sensors, &servoCtrl);

void setup() {
  Serial.begin(115200);
  delay(500);
  
  Serial.println("\n=================================");
  Serial.println("ESP32 Sensor & Servo Control");
  Serial.println("=================================\n");

  // Inicializar sensores
  Serial.println("Inicializando sensores...");
  sensors.begin();
  
  // Inicializar servo
  Serial.println("Inicializando servomotor...");
  servoCtrl.begin();
  
  // Conectar WiFi
  if (WebServerManager::connectWiFi(WIFI_SSID, WIFI_PASS, WIFI_TIMEOUT)) {
    // Configurar mDNS
    if (MDNS.begin(MDNS_NAME)) {
      Serial.print("mDNS iniciado: http://");
      Serial.print(MDNS_NAME);
      Serial.println(".local/");
    }
    
    // Iniciar servidor web
    webServer.begin();
    Serial.println("\n✓ Sistema listo!");
    Serial.println("Acceso web:");
    Serial.print("  - http://");
    Serial.println(WiFi.localIP());
    Serial.print("  - http://");
    Serial.print(MDNS_NAME);
    Serial.println(".local/");
  } else {
    Serial.println("⚠ Sistema en modo offline");
  }
  
  Serial.println("\n=================================\n");
}

void loop() {
  // Procesar peticiones web
  webServer.handleClient();
  
  // Actualizar posición del servo (si está en movimiento)
  servoCtrl.update();
  
  // Mantener mDNS activo (opcional pero recomendado)
  MDNS.update();
}