#ifndef NGC_CONTROLLER_H
#define NGC_CONTROLLER_H

#include "ControllerCommon.h"

class NGCController {
public:
    explicit NGCController(uint8_t dataPin);
    void begin();
    ControllerState read();

private:
    uint8_t _dataPin;
    uint32_t _cpuFreqMHz;

    static inline bool btnPressed(uint8_t byte, uint8_t bit) {
        return (((byte >> bit) & 0x01) == 1);
    }

    inline void delayCycles(uint32_t cycles);
    inline void sendBit(bool bit);
    void sendByte(uint8_t b);
    int readBit();
};

#endif
