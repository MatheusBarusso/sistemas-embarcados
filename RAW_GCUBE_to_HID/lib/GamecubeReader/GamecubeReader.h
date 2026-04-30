#ifndef GAMECUBE_READER_H
#define GAMECUBE_READER_H

#include <Arduino.h>

// Estrutura que guarda todos os botões do Gamecube
struct Gamecube_Report_t {
    bool a, b, x, y, start;
    bool dUp, dDown, dLeft, dRight;
    bool z, l, r;
    uint8_t xAxis, yAxis;
    uint8_t cxAxis, cyAxis;
    uint8_t leftAnalog, rightAnalog;
};

class GamecubeReader {
private:
    uint8_t dataPin;
    uint32_t cpuFreqMHz;
    
    // Funções internas para controle ultra-rápido dos pinos
    void delayCycles(uint32_t cycles);
    void sendBit(bool bit);
    void sendByte(uint8_t byte);
    bool readBit();
    
public:
    Gamecube_Report_t report; 

    GamecubeReader(uint8_t pin);
    void begin();
    bool update();
    void debugPrint();
};

#endif