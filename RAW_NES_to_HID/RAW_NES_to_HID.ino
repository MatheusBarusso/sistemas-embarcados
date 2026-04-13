#include "USB.h"
#include "USBHIDGamepad.h"

USBHIDGamepad gamepad;

const int latchPin = 4;
const int pulsePin = 6;
const int dataPin = 8;

const unsigned int delayTimeMicroseconds = 6;

const int btnsCount = 8;
bool btns[8];

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(pulsePin, OUTPUT);
  digitalWrite(latchPin, LOW);
  digitalWrite(pulsePin, LOW);

  pinMode(dataPin, INPUT_PULLUP);

  USB.begin();
  gamepad.begin();
}

void loop() {
  // leitura dos botões
  digitalWrite(pulsePin, LOW);
  digitalWrite(latchPin, HIGH);
  delayMicroseconds(delayTimeMicroseconds * 2);
  digitalWrite(latchPin, LOW);
  delayMicroseconds(delayTimeMicroseconds);

  for (int i = 0; i < btnsCount; i++) {
    btns[i] = !digitalRead(dataPin);
    digitalWrite(pulsePin, HIGH);
    delayMicroseconds(delayTimeMicroseconds);
    digitalWrite(pulsePin, LOW);
    delayMicroseconds(delayTimeMicroseconds);
  }

  uint32_t buttons = 0; //bitmask

  if (btns[0]) buttons |= (1 << 0); // A
  if (btns[1]) buttons |= (1 << 1); // B
  if (btns[2]) buttons |= (1 << 6); // Select
  if (btns[3]) buttons |= (1 << 7); // Start

  // HAT (direcional)

  uint8_t hat = 0; 

  if (btns[4]) hat = 1; // up
  if (btns[5]) hat = 5; // down
  if (btns[6]) hat = 7; // left
  if (btns[7]) hat = 3; // right

  // diagonais
  if (btns[4] && btns[7]) hat = 2;
  if (btns[5] && btns[7]) hat = 4;
  if (btns[5] && btns[6]) hat = 6;
  if (btns[4] && btns[6]) hat = 8;

  // ENVIA HID
  gamepad.send(
    0, 0, 0,   // x, y, z
    0, 0, 0,   // rz, rx, ry
    hat,
    buttons
  );

  delay(10);
}