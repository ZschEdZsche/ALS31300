/*******************************************************

    ALS31300 3-axis hallsensor with I2C-interface

    simple arduino sketch as an example for the library.
    Reads data from the sensor and displays it over serial.

    date: 11.07.2023
    name: Edwin Zschetzsche

*******************************************************/
#include <Wire.h>

#include "ALS31300.h"

#define RAW 0 // if 1 display raw data

ALS31300 sensor;

void setup() {
    Wire.begin();

    sensor.init();

    sensor.setWritePermission();
    sensor.readEEPROM();

    Serial.begin(115200);
}

void loop() {
    sensor.readData();

    // raw Data
    if(RAW) {
        Serial.print(sensor.getX());
        Serial.print(" | ");
        Serial.print(sensor.getY());
        Serial.print(" | ");
        Serial.print(sensor.getZ());
        Serial.print(" | ");
    }

    // in T
    if(!RAW) {
        Serial.print(sensor.getXinT(), 6);
        Serial.print(" | ");
        Serial.print(sensor.getYinT(), 6);
        Serial.print(" | ");
        Serial.print(sensor.getZinT(), 6);
        Serial.print(" | ");
    }

    Serial.println(sensor.getTemp());

    delay(500);
}
