#include "GenesisController.h"

GenesisController::GenesisController(uint8_t up, uint8_t down, uint8_t left, uint8_t right, uint8_t b, uint8_t c, uint8_t sel) 
    : _up(up), _down(down), _left(left), _right(right), _b(b), _c(c), _sel(sel) {}

void GenesisController::begin() {
    pinMode(_up, INPUT);
    pinMode(_down, INPUT);
    pinMode(_left, INPUT);
    pinMode(_right, INPUT);
    pinMode(_b, INPUT);
    pinMode(_c, INPUT);
    pinMode(_sel, OUTPUT);
}

ControllerState GenesisController::read() {
    ControllerState state = {0, 0};
    bool up, down, left, right;
    bool a, b, c, start;
    
    // Leitura com SEL HIGH (Lê Cima, Baixo, Esquerda, Direita, B e C)
    digitalWrite(_sel, HIGH);
    delayMicroseconds(50);
    up    = !digitalRead(_up);
    down  = !digitalRead(_down);
    left  = !digitalRead(_left);
    right = !digitalRead(_right);
    b     = !digitalRead(_b);
    c     = !digitalRead(_c);

    // Leitura com SEL LOW (Lê A e Start nos pinos correspondentes a B e C)
    digitalWrite(_sel, LOW);
    delayMicroseconds(50);
    a     = !digitalRead(_b);
    start = !digitalRead(_c);

    // Mapeamento de Botões (Bitmask)
    if (a) state.buttons |= (1 << 0);     // Botão 1 (A)
    if (b) state.buttons |= (1 << 1);     // Botão 2 (B)
    if (c) state.buttons |= (1 << 2);     // Botão 3 (C)
    if (start) state.buttons |= (1 << 7); // Botão 8 (Start)

    // Mapeamento do D-PAD (HAT)
    if (up) state.hat = 1;
    if (down) state.hat = 5;
    if (left) state.hat = 7;
    if (right) state.hat = 3;

    if (up && right) state.hat = 2;
    if (down && right) state.hat = 4;
    if (down && left) state.hat = 6;
    if (up && left) state.hat = 8;

    return state;
}