#ifndef BMP280_H
#define BMP280_H

#include <stdint.h>

class BMP280 {
public:
    BMP280(int i2c_bus, int address);
    ~BMP280();

    bool begin();
    float readTemperature();
    float readPressure();

private:
    int i2c_handle;
    int i2c_bus;
    int address;
    int32_t t_fine;
    uint16_t dig_T1;
    int16_t dig_T2, dig_T3;
    uint16_t dig_P1;
    int16_t dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;

    void writeRegister(uint8_t reg, uint8_t value);
    uint32_t read24(uint8_t reg);
    void readCalibrationData();
    int32_t compensateTemperature(int32_t adc_T);
    uint32_t compensatePressure(int32_t adc_P);
};

#endif // BMP280_H
