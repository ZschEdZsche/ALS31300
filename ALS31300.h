#ifndef ALS31300_H
#define ALS31300_H

#define ADDRESS 0x60

#define DATAMSB_ADDR 0x28
#define DATALSB_ADDR 0x29

#define SENSITIVITY 4.0 // sensor sensitivity in LSB/G

struct Sensor{
    int16_t x;
    int16_t y;
    int16_t z;
    float temp;
};

class ALS31300 {
    public:
        ALS31300();

        int8_t init();
        int8_t init(uint8_t addr);

        void setWritePermission();
        void readEEPROM();

        void readData();
        int16_t getX();
        int16_t getY();
        int16_t getZ();
        float getTemp();

        float getXinT();
        float getYinT();
        float getZinT();

    private:
        Sensor _data;
        uint8_t _addr;

        uint32_t _readReg(byte regAddr);
        float _convertToTesla(int16_t data);
};

#endif
