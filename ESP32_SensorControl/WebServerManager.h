#ifndef WEB_SERVER_MANAGER_H
#define WEB_SERVER_MANAGER_H

#include <WebServer.h>
#include "Sensors.h"
#include "ServoControl.h"

class WebServerManager {
private:
  WebServer server;
  SensorManager* sensors;
  ServoController* servoCtrl;
  
  // Handlers
  void handleRoot();
  void handleTemp();
  void handleGas();
  void handleStart();
  void handleStop();
  void handleSet();
  void handleStatus();

public:
  WebServerManager(int port, SensorManager* sensorMgr, ServoController* servo);
  
  void begin();
  void handleClient();
  
  // Helper para WiFi
  static bool connectWiFi(const char* ssid, const char* password, unsigned long timeout);
};

#endif // WEB_SERVER_MANAGER_H