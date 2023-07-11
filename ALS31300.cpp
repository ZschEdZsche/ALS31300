#include <Arduino.h>
#include <Wire.h>

#include "ALS31300.h"

/****************************************
 *  Public section
****************************************/

ALS31300::ALS31300() {
    // initialize data struct
    this->_data.x = 0;
    this->_data.y = 0;
    this->_data.z = 0;
    this->_data.temp = 0;
}

int8_t ALS31300::init() {
    this->_addr = ADDRESS;
    return 0;
}

int8_t ALS31300::init(uint8_t addr) {
    this->_addr = addr;
    return 0;
}

void ALS31300::setWritePermission() {
    // set write Access to EEPROM
    Wire.beginTransmission(this->_addr);
    Wire.write(0x35);
    Wire.write(0x2C);
    Wire.write(0x41);
    Wire.write(0x35);
    Wire.write(0x34);
    Wire.endTransmission(this->_addr);
}

void ALS31300::readEEPROM() {
    uint8_t addr = 0x02;
    uint32_t data = this->_readReg(addr);
    Serial.print("Addr ");
    Serial.print(addr);
    Serial.print(": ");
    Serial.println(data, BIN);

    addr = 0x03;
    data = this->_readReg(addr);
    Serial.print("Addr ");
    Serial.print(addr);
    Serial.print(": ");
    Serial.println(data, BIN);

    for(uint8_t i = 0x0D; i < 0x10; i++) {
        data = this->_readReg(i);
        Serial.print("Addr ");
        Serial.print(i);
        Serial.print(": ");
        Serial.println(data, BIN);
    }
}

void ALS31300::readData() {
    /*
        IC sends data for all 3 axis + temperature stacked in 2 32-bit registers
        this function extracts the data and returns it in an array
    */
    uint32_t msbData = this->_readReg(DATAMSB_ADDR);
    uint32_t lsbData = this->_readReg(DATALSB_ADDR);

    // sensor stores data in signed 12-bit integer
    // arduino uses signed 16-bit integer
    // the data is shifted to set sign on bit 16
    // by dividing by 16 it goes to 12-bit resolution
    int16_t value;
    uint8_t conversionFactor = 16;

    //Serial.println("X Achse");
    value = (msbData & 0xFF000000) >> 16 | (lsbData & 0xF0000) >> 12;
    this->_data.x = value / conversionFactor;

    //Serial.println("Y Achse");
    value = (msbData  & 0xFF0000) >> 8 | (lsbData & 0xF000) >> 8;
    this->_data.y = value / conversionFactor;

    //Serial.println("Z Achse");
    value = (msbData  & 0xFF00) | (lsbData & 0xF00) >> 4;
    this->_data.z = value / conversionFactor;

    //Serial.println("Temperatur");
    uint16_t rawTemp = (msbData  & 0x3F) << 6 | (lsbData & 0x3F);
    // formel für °C -> siehe Datenblatt
    // (uint16_t) (302 * (value - 1708) / 4096);
    this->_data.temp = 302 * ( (float) (rawTemp - 1708) / 4096 );
}

int16_t ALS31300::getX() {
    return this->_data.x;
}

int16_t ALS31300::getY() {
    return this->_data.y;
}

int16_t ALS31300::getZ() {
    return this->_data.z;
}

float ALS31300::getTemp() {
    return this->_data.temp;
}

float ALS31300::getXinT() {
    return this->_convertToTesla(this->_data.x);
}

float ALS31300::getYinT() {
    return this->_convertToTesla(this->_data.y);
}

float ALS31300::getZinT() {
    return this->_convertToTesla(this->_data.z);
}


/****************************************
 *  Private section
****************************************/

float ALS31300::_convertToTesla(int16_t data) {
    return 0.0001 * (float) data / SENSITIVITY;
}

uint32_t ALS31300::_readReg(byte regAddr) {
    // send read request
    Wire.beginTransmission(this->_addr);
    // send address to read from
    Wire.write(regAddr);
    Wire.endTransmission(this->_addr);

    uint8_t i;
    uint32_t recvData = 0;
    Wire.requestFrom(this->_addr, (uint8_t) 4);

    // wait until 4 bytes received
    while(Wire.available() < 4);
    // construct 32-bit integer
    for(i = 0; i < 4; i++) {
        recvData += Wire.read();
        if(i < 3) {
            recvData = recvData << 8;
        }
    }

    return recvData;
}
