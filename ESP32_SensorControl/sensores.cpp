#include "Sensores.h"
#include "Config.h"
#include <Arduino.h>

SensorManager::SensorManager(int oneWirePin, int gasPin) 
  : oneWire(oneWirePin), tempSensor(&oneWire) {
  lastTemperature = DEVICE_DISCONNECTED_C;
  lastTempRequest = 0;
  pinMode(gasPin, INPUT);
}

void SensorManager::begin() {
  tempSensor.begin();
  tempSensor.setResolution(TEMP_RESOLUTION);
  tempSensor.setWaitForConversion(false);
}

bool SensorManager::readTemperature(float& temperature) {
  unsigned long now = millis();
  
  // Usar cache si la lectura es reciente
  if (lastTemperature != DEVICE_DISCONNECTED_C && 
      (now - lastTempRequest) < TEMP_READ_INTERVAL) {
    temperature = lastTemperature;
    return true;
  }
  
  // Nueva lectura
  tempSensor.requestTemperatures();
  lastTemperature = tempSensor.getTempCByIndex(0);
  lastTempRequest = now;

  if (lastTemperature == DEVICE_DISCONNECTED_C) {
    return false;
  }

  temperature = lastTemperature;
  return true;
}

void SensorManager::readGas(int& rawValue, float& voltage) {
  rawValue = analogRead(MQ135_PIN);
  voltage = (3.3f * rawValue) / 4095.0f;
}