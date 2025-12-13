#ifndef SENSORS_H
#define SENSORS_H

#include <OneWire.h>
#include <DallasTemperature.h>

class SensorManager {
private:
  OneWire oneWire;
  DallasTemperature tempSensor;
  
  float lastTemperature;
  unsigned long lastTempRequest;
  const unsigned long TEMP_READ_INTERVAL = 750;

public:
  SensorManager(int oneWirePin, int gasPin);
  
  void begin();
  
  // Sensor de temperatura
  bool readTemperature(float& temperature);
  
  // Sensor de gas
  void readGas(int& rawValue, float& voltage);
};

#endif // SENSORS_H