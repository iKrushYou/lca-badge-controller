#include "gpio.h"
#include <Arduino.h>  // Include Arduino library for pinMode and digitalWrite
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel pixels(NUM_PIXELS, LED_SERIAL_PIN, NEO_GRBW + NEO_KHZ800);

void setupGpio() {
    pixels.begin();
    pixels.show(); // Initialize all pixels to 'off'
}

void setLEDs(const int ledNum) {
    pixels.clear();

    for (int i = 0; i < ledNum; i++) {
        pixels.setPixelColor(i, Adafruit_NeoPixel::Color(255, 255, 255, 255));
        pixels.show();
    }

    pixels.show();
}
