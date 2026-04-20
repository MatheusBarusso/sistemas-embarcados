#ifndef GENESIS_CONTROLLER_H
#define GENESIS_CONTROLLER_H

#include "ControllerCommon.h"

class GenesisController {
public:
    GenesisController(uint8_t up, uint8_t down, uint8_t left, uint8_t right, uint8_t b, uint8_t c, uint8_t sel);
    void begin();
    ControllerState read();

private:
    uint8_t _up, _down, _left, _right, _b, _c, _sel;
};

#endif