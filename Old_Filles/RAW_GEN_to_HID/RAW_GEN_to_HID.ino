#include "USB.h"
#include "USBHIDGamepad.h"

USBHIDGamepad gamepad;

#define PIN_UP     4
#define PIN_DOWN   5
#define PIN_LEFT   6
#define PIN_RIGHT  7
#define PIN_B      15
#define PIN_C      17
#define PIN_SEL    16

void setup() {
  pinMode(PIN_UP, INPUT);
  pinMode(PIN_DOWN, INPUT);
  pinMode(PIN_LEFT, INPUT);
  pinMode(PIN_RIGHT, INPUT);
  pinMode(PIN_B, INPUT);
  pinMode(PIN_C, INPUT);

  pinMode(PIN_SEL, OUTPUT);

  USB.begin();
  gamepad.begin();
}

void loop() {
  bool up, down, left, right;
  bool a, b, c, start;
  
  // Leitura com SEL HIGH (Lê Cima, Baixo, Esquerda, Direita, B e C)
  digitalWrite(PIN_SEL, HIGH);
  delayMicroseconds(50);

  up    = !digitalRead(PIN_UP);
  down  = !digitalRead(PIN_DOWN);
  left  = !digitalRead(PIN_LEFT);
  right = !digitalRead(PIN_RIGHT);
  b     = !digitalRead(PIN_B);
  c     = !digitalRead(PIN_C);

  // Leitura com SEL LOW (Lê A e Start nos pinos correspondentes a B e C)
  digitalWrite(PIN_SEL, LOW);
  delayMicroseconds(50);

  a     = !digitalRead(PIN_B);
  start = !digitalRead(PIN_C);


  // Mapeamento de Botões (Bitmask)
  uint32_t buttons = 0; 
  if (a) buttons |= (1 << 0);     // Botão 1 (A)
  if (b) buttons |= (1 << 1);     // Botão 2 (B)
  if (c) buttons |= (1 << 2);     // Botão 3 (C)
  if (start) buttons |= (1 << 7); // Botão 8 (Start)

  // Mapeamento do D-PAD (HAT)
  uint8_t hat = 0; // 0 = Centro


  if (up) hat = 1;
  if (down) hat = 5;
  if (left) hat = 7;
  if (right) hat = 3;


  if (up && right) hat = 2;
  if (down && right) hat = 4;
  if (down && left) hat = 6;
  if (up && left) hat = 8;


  gamepad.send(
    0, 0, 0,   
    0, 0, 0,   
    hat,       
    buttons    
  );

  delay(10);
}