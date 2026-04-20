#ifndef NES_CONTROLLER_H
#define NES_CONTROLLER_H

#include "ControllerCommon.h"

class NESController {
public:
    NESController(uint8_t latch, uint8_t pulse, uint8_t data);
    void begin();
    ControllerState read();

private:
    uint8_t _latch, _pulse, _data;
    const unsigned int delayTimeMicroseconds = 6;
    const int btnsCount = 8;
};

#endif