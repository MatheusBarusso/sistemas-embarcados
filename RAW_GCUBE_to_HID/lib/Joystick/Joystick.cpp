#include "Joystick.h"

Joystick::Joystick(int px, int py, int pb, int cx, int cy, int dz) {
  pinX = px;
  pinY = py;
  pinBtn = pb;
  centerX = cx;
  centerY = cy;
  deadzone = dz;
  
  // Initialize all locks as false
  lockX = false;
  lockY = false;
  lockBtn = false;
}

void Joystick::begin() {
  pinMode(pinX, INPUT);
  pinMode(pinY, INPUT);
  pinMode(pinBtn, INPUT_PULLUP);
}

void Joystick::update() {
  int rawX = analogRead(pinX);
  int rawY = analogRead(pinY);
  bool readingBtn = (digitalRead(pinBtn) == LOW); 

  // --- X AXIS CALCULATION ---
  valueX = 0;
  if (abs(rawX - centerX) > deadzone) {
    if (rawX > centerX) valueX = map(rawX, centerX + deadzone, 4095, 0, 100);
    else valueX = map(rawX, centerX - deadzone, 0, 0, -100);
  }
  valueX = constrain(valueX, -100, 100);

  // --- Y AXIS CALCULATION ---
  valueY = 0;
  if (abs(rawY - centerY) > deadzone) {
    if (rawY > centerY) valueY = map(rawY, centerY + deadzone, 4095, 0, 100);
    else valueY = map(rawY, centerY - deadzone, 0, 0, -100);
  }
  valueY = constrain(valueY, -100, 100);

  // ========================================================
  // LOCK LOGIC (Hidden from main.cpp)
  // ========================================================
  
  // Reset all events to false at the start of the cycle
  evtUp = false;
  evtDown = false;
  evtLeft = false;
  evtRight = false;
  evtBtn = false;

  // Y Axis Logic
  if (valueY > 50 && !lockY) { evtUp = true; lockY = true; }
  else if (valueY < -50 && !lockY) { evtDown = true; lockY = true; }
  else if (valueY >= -50 && valueY <= 50) { lockY = false; } // Unlock

  // X Axis Logic
  if (valueX > 50 && !lockX) { evtRight = true; lockX = true; }
  else if (valueX < -50 && !lockX) { evtLeft = true; lockX = true; }
  else if (valueX >= -50 && valueX <= 50) { lockX = false; } // Unlock

  // Button Logic
  if (readingBtn && !lockBtn) { evtBtn = true; lockBtn = true; }
  else if (!readingBtn) { lockBtn = false; } // Unlock
}

// Return raw mapped values
int Joystick::getX() { return valueX; }
int Joystick::getY() { return valueY; }

// Return single-click events
bool Joystick::wentUp() { return evtUp; }
bool Joystick::wentDown() { return evtDown; }
bool Joystick::wentLeft() { return evtLeft; }
bool Joystick::wentRight() { return evtRight; }
bool Joystick::clicked() { return evtBtn; }