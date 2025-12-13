#ifndef CONFIG_H
#define CONFIG_H

/* ------------------------------------------
   CONFIGURACIÓN WIFI
------------------------------------------- */
const char* WIFI_SSID = "Nombre de la red, tal cual aparezca en la compu/celu";
const char* WIFI_PASS = "Contraseña";
const unsigned long WIFI_TIMEOUT = 15000;

/* ------------------------------------------
   CONFIGURACIÓN SENSORES
------------------------------------------- */
#define ONE_WIRE_BUS 4
#define MQ135_PIN 34
#define TEMP_RESOLUTION 10  // 10 bits = 0.25°C, lectura en ~188ms

/* ------------------------------------------
   CONFIGURACIÓN SERVOMOTOR
------------------------------------------- */
#define SERVO_PIN 22

const int MIN_US = 600;
const int MAX_US = 2500;
const int CENTER_ANGLE = 90;
const int ANGLE_RANGE = 25;
const int MIN_ANGLE = CENTER_ANGLE - ANGLE_RANGE;
const int MAX_ANGLE = CENTER_ANGLE + ANGLE_RANGE;
const unsigned long MIN_STEP_INTERVAL = 20; // ms

/* ------------------------------------------
   CONFIGURACIÓN SERVIDOR
------------------------------------------- */
const int SERVER_PORT = 80;
const char* MDNS_NAME = "esp-demo";

#endif // CONFIG_H