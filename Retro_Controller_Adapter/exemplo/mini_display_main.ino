#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Joystick.h"
#include "USB.h"
#include "USBHIDGamepad.h"
#include "GenesisController.h"
#include "NESController.h"
#include "NGCController.h"

// --- DISPLAY CONFIGURATION ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

// I2C pins for your display
#define SDA_PIN 7
#define SCL_PIN 15

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// HID + controllers (same logic as src/main.cpp)
USBHIDGamepad gamepad;
GenesisController genesis(4, 5, 6, 7, 15, 17, 16);
NESController nes(18, 9, 8);
NGCController ngc(4);

// --- CONTROLLER CONFIGURATION ---
// Pins: X on 5, Y on 4, Button on 12
Joystick controller(10, 11, 12);

int current_item = 0;
bool is_selected = false;

const char* menuItems[] = {"NES", "GEN", "NGC"};
const int menuSize = 3;

void showOnScreen(const String& message) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);

  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(message, 0, 0, &x1, &y1, &w, &h);

  int16_t x = (SCREEN_WIDTH - w) / 2;
  int16_t y = (SCREEN_HEIGHT - h) / 2;

  display.setCursor(x, y);
  display.println(message);
  display.display();

  Serial.println(message);
}

void showMenuSelection() {
  String text = menuItems[current_item];

  if (is_selected) {
    text += " *";
  }

  showOnScreen(text);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // 1. Start the Joystick and controllers
  controller.begin();
  genesis.begin();
  nes.begin();
  ngc.begin();
  USB.productName("Retro USB Adapter");
  USB.begin();
  gamepad.begin();

  // 2. Start I2C for OLED
  Wire.begin(SDA_PIN, SCL_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 failed! Check wiring."));
    for (;;) {}
  }

  showOnScreen("READY");
  delay(500);
  showMenuSelection();
}

void loop() {
  controller.update();

  // Navegação de menu (igual ao main.cpp)
  if (!is_selected) {
    if (controller.wentUp()) {
      current_item = (current_item + 1) % menuSize;
      showMenuSelection();
    }

    if (controller.wentDown()) {
      current_item = (current_item - 1 + menuSize) % menuSize;
      showMenuSelection();
    }
  }

  if (controller.clicked()) {
    is_selected = !is_selected;
    showMenuSelection();
  }

  // Read selected controller and send HID (same behavior as main.cpp)
  if (is_selected) {
    ControllerState s = {0, 0, 0, 0, 0, 0, 0, 0};

    if (current_item == 0) {
      s = nes.read();
    } else if (current_item == 1) {
      s = genesis.read();
    } else if (current_item == 2) {
      s = ngc.read();
    }

    gamepad.send(
      s.x, s.y, s.z,
      s.rz, s.rx, s.ry,
      s.hat,
      s.buttons
    );
  } else {
    // Neutral HID when menu is active
    gamepad.send(0, 0, 0, 0, 0, 0, 0, 0);
  }

  delay(10);
}

