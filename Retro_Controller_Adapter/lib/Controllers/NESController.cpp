#include "NESController.h"

NESController::NESController(uint8_t latch, uint8_t pulse, uint8_t data)
    : _latch(latch), _pulse(pulse), _data(data) {}

void NESController::begin() {
    pinMode(_latch, OUTPUT);
    pinMode(_pulse, OUTPUT);
    digitalWrite(_latch, LOW);
    digitalWrite(_pulse, LOW);
    pinMode(_data, INPUT_PULLUP);
}

ControllerState NESController::read() {
    ControllerState state = {0, 0};
    bool btns[8];

    // leitura dos botões
    digitalWrite(_pulse, LOW);
    digitalWrite(_latch, HIGH);
    delayMicroseconds(delayTimeMicroseconds * 2);
    digitalWrite(_latch, LOW);
    delayMicroseconds(delayTimeMicroseconds);

    for (int i = 0; i < btnsCount; i++) {
        btns[i] = !digitalRead(_data);
        digitalWrite(_pulse, HIGH);
        delayMicroseconds(delayTimeMicroseconds);
        digitalWrite(_pulse, LOW);
        delayMicroseconds(delayTimeMicroseconds);
    }

    // Mapeamento de botões (bitmask)
    if (btns[0]) state.buttons |= (1 << 0); // A
    if (btns[1]) state.buttons |= (1 << 1); // B
    if (btns[2]) state.buttons |= (1 << 6); // Select
    if (btns[3]) state.buttons |= (1 << 7); // Start

    // HAT (direcional)
    if (btns[4]) state.hat = 1; // up
    if (btns[5]) state.hat = 5; // down
    if (btns[6]) state.hat = 7; // left
    if (btns[7]) state.hat = 3; // right

    // diagonais
    if (btns[4] && btns[7]) state.hat = 2;
    if (btns[5] && btns[7]) state.hat = 4;
    if (btns[5] && btns[6]) state.hat = 6;
    if (btns[4] && btns[6]) state.hat = 8;

    return state;
}