#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <Arduino.h>

class Joystick {
  private:
    int pinX, pinY, pinBtn;
    int centerX, centerY, deadzone;
    int valueX, valueY;
    
    // Lock variables to prevent multiple inputs (debounce/state change)
    bool lockX, lockY, lockBtn;
    
    // Variables holding the single-click event
    bool evtUp, evtDown, evtLeft, evtRight, evtBtn;

  public:
    Joystick(int pinX, int pinY, int pinBtn, int centerX = 1970, int centerY = 1915, int deadzone = 150);
    
    void begin();
    void update();
    
    // Returns continuous values (from -100 to 100)
    int getX();
    int getY();
    
    // Menu Event Functions (Returns true ONLY ONCE per movement)
    bool wentUp();
    bool wentDown();
    bool wentLeft();
    bool wentRight();
    bool clicked();
};

#endif