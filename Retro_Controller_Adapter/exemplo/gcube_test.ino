#include <Arduino.h>
#include "NGCController.h"

#define pinLed LED_BUILTIN
#define GC_DAT_PIN 4

NGCController GamecubeController(GC_DAT_PIN);

static void print_gc_state(const ControllerState &gc_state);

void setup() {
  pinMode(pinLed, OUTPUT);
  Serial.begin(115200);

  GamecubeController.begin();

  Serial.println(F("Iniciando leitura do controle Gamecube no ESP32-S3..."));
  Serial.println();
}

void loop() {
  ControllerState gc_state = GamecubeController.read();
  print_gc_state(gc_state);

  digitalWrite(pinLed, gc_state.buttons != 0 ? HIGH : LOW);
  delay(100);
}

static void print_gc_state(const ControllerState &gc_state) {
  Serial.println();
  Serial.println(F("--- Teste Gamecube ---"));

  Serial.print(F("Buttons:\t0x")); Serial.println(gc_state.buttons, HEX);
  Serial.print(F("Hat:\t")); Serial.println(gc_state.hat);
  Serial.print(F("x:\t")); Serial.println(gc_state.x);
  Serial.print(F("y:\t")); Serial.println(gc_state.y);
  Serial.print(F("z:\t")); Serial.println(gc_state.z);
  Serial.print(F("rz:\t")); Serial.println(gc_state.rz);
  Serial.print(F("rx:\t")); Serial.println(gc_state.rx);
  Serial.print(F("ry:\t")); Serial.println(gc_state.ry);

  Serial.println(F("--------------------------------------"));
}