#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H

#include <ESP32Servo.h>

class ServoController {
private:
  Servo servo;
  
  volatile bool running;
  int currentAngle;
  int speedDps;
  int stepDeg;
  int dir;
  unsigned long lastStepMs;
  unsigned long stepInterval;
  
  void updateStepParams();

public:
  ServoController(int pin);
  
  void begin();
  void update();  // Llamar en loop()
  
  void start(int speed);
  void stop();
  void setAngle(int angle);
  
  int getCurrentAngle() const { return currentAngle; }
  bool isRunning() const { return running; }
  bool isAttached() const { return servo.attached(); }
};

#endif // SERVO_CONTROL_H