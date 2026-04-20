#ifndef CONTROLLER_COMMON_H
#define CONTROLLER_COMMON_H

#include <Arduino.h>

struct ControllerState {
    uint32_t buttons;
    uint8_t hat;
};

#endif