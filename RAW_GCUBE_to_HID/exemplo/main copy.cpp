#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Joystick.h" 
#include "display_assets.h"

// --- DISPLAY CONFIGURATION ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

// Your new I2C pins
#define SDA_PIN 7
#define SCL_PIN 15

// Create the display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- CONTROLLER CONFIGURATION ---
// Pins: X on 5, Y on 4, Button on 6 
Joystick controller(5, 4, 6);


// --- HELPER FUNCTION ---
// We use this so we don't have to repeat screen commands in the loop
void showOnScreen(String message) {
  display.clearDisplay();              // Clear the old text
  display.setTextSize(2);              // Size 2 makes it nice and big
  display.setTextColor(SSD1306_WHITE); // White text
  
  // Set cursor slightly down from the top to center it (x=0, y=8)
  display.setCursor(0, 8);             
  display.println(message);
  
  display.display();                   // Send the buffer to the screen
  
  // Keep sending to Serial Monitor just in case you need to debug
  Serial.println(message);             
}

void showBitmap(const unsigned char* bitmap, int width, int height) {
  display.clearDisplay();
  display.drawBitmap(0, 0, bitmap, width, height, SSD1306_WHITE);
  display.display();
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // 1. Start the Joystick
  controller.begin();

  // 2. Start the I2C bus with your specific pins
  Wire.begin(SDA_PIN, SCL_PIN);

  // 3. Start the OLED Display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 failed! Check wiring."));
    for(;;); // Freeze here if the screen doesn't connect
  }

  // Show an initial message
  showBitmap(coxa, 128, 32);
}

void loop() {
  // Update hardware states
  controller.update();

  // ----- MENU LOGIC -----
  
  if (controller.wentUp()) {
    showOnScreen("UP");
  }
  
  if (controller.wentDown()) {
    showOnScreen("DOWN");
  }
  
  if (controller.wentRight()) {
    showOnScreen("RIGHT");
  }
  
  if (controller.wentLeft()) {
    showOnScreen("LEFT");
  }
  
  if (controller.clicked()) {
    showOnScreen("SELECT");
  }

  delay(10); 
}