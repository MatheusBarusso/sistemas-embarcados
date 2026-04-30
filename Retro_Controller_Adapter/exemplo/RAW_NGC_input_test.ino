#include <Arduino.h>
#include "NGCController.h"

// Troque aqui para input do pin DATA
#define NGC_DATA_PIN 4

NGCController ngc(NGC_DATA_PIN);

static void printState(const ControllerState &s) {
  Serial.print("BTN=0x");
  Serial.print((unsigned long)s.buttons, HEX);
  Serial.print(" HAT=");
  Serial.print(s.hat);

  Serial.print(" | LX=");
  Serial.print(s.x);
  Serial.print(" LY=");
  Serial.print(s.y);
  Serial.print(" CX=");
  Serial.print(s.z);
  Serial.print(" CY=");
  Serial.print(s.rz);
  Serial.print(" TL=");
  Serial.print(s.rx);
  Serial.print(" TR=");
  Serial.println(s.ry);
}

void setup() {
  Serial.begin(115200);
  delay(1500);
  Serial.println("[NGC TEST] Iniciando leitura do controle...");

  ngc.begin();
}

void loop() {
  ControllerState s = ngc.read();
  printState(s);
  delay(16); // ~60Hz
}

