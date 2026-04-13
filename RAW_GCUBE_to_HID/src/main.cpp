#include <Arduino.h>
#include "Joystick.h" 

// Pins: X on 5, Y on 4, Button on 6 
Joystick controller(5, 4, 6);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  controller.begin();
  Serial.println("Ready! Menu navigation 100% optimized.");
}

void loop() {
  controller.update();

  
  if (controller.wentUp()) {
    Serial.println("Up ");
  }
  
  if (controller.wentDown()) {
    Serial.println("Down");
  }
  
  if (controller.wentRight()) {
    Serial.println("Right");
  }
  
  if (controller.wentLeft()) {
    Serial.println("Left");
  }
  
  if (controller.clicked()) {
    Serial.println("Select");
  }

  delay(10); 
}