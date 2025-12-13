#include "ServoControl.h"
#include "Config.h"
#include <Arduino.h>

ServoController::ServoController(int pin) {
  running = false;
  currentAngle = CENTER_ANGLE;
  speedDps = 60;
  stepDeg = 1;
  dir = 1;
  lastStepMs = 0;
  stepInterval = 0;
}

void ServoController::begin() {
  servo.setPeriodHertz(50);
  servo.attach(SERVO_PIN, MIN_US, MAX_US);
  servo.write(currentAngle);
  updateStepParams();
}

void ServoController::updateStepParams() {
  // Determinar paso según velocidad
  if (speedDps <= 70)        stepDeg = 1;
  else if (speedDps <= 150)  stepDeg = 2;
  else if (speedDps <= 250)  stepDeg = 3;
  else                       stepDeg = 4;
  
  // Precalcular intervalo
  float ms = (1000.0f * stepDeg) / speedDps;
  stepInterval = (ms < MIN_STEP_INTERVAL) ? MIN_STEP_INTERVAL : (unsigned long)ms;
}

void ServoController::start(int speed) {
  speedDps = constrain(speed, 10, 350);
  updateStepParams();
  
  if (!servo.attached()) {
    servo.attach(SERVO_PIN, MIN_US, MAX_US);
  }
  
  running = true;
  dir = 1;
  lastStepMs = millis();
}

void ServoController::stop() {
  running = false;
}

void ServoController::setAngle(int angle) {
  int a = constrain(angle, MIN_ANGLE, MAX_ANGLE);
  running = false;

  if (!servo.attached()) {
    servo.attach(SERVO_PIN, MIN_US, MAX_US);
  }

  currentAngle = a;
  servo.write(currentAngle);
}

void ServoController::update() {
  if (!running || !servo.attached()) {
    return;
  }
  
  unsigned long now = millis();
  
  if (now - lastStepMs >= stepInterval) {
    lastStepMs = now;

    int next = currentAngle + dir * stepDeg;

    // Cambiar dirección en los límites
    if (next >= MAX_ANGLE) { 
      next = MAX_ANGLE; 
      dir = -1; 
    } else if (next <= MIN_ANGLE) { 
      next = MIN_ANGLE; 
      dir = 1; 
    }

    currentAngle = next;
    servo.write(currentAngle);
  }
}