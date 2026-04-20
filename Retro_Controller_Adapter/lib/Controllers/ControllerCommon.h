#ifndef CONTROLLER_COMMON_H
#define CONTROLLER_COMMON_H

#include <Arduino.h>

struct ControllerState {
    uint32_t buttons;
    uint8_t hat;
    int16_t x;
    int16_t y;
    int16_t z;
    int16_t rz;
    int16_t rx;
    int16_t ry;
};

#endif