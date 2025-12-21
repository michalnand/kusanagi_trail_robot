#ifndef _IMU_LL_H_
#define _IMU_LL_H_

#include "drivers.h"


class IMULL
{
    public:
        void init(I2C_Interface &i2c);
        void read();

    private:
        int16_t read_word(unsigned char dev_adr, unsigned char reg_adr);


    public:
        float ax, ay, az;
        float gx, gy, gz;

    private:
        I2C_Interface *i2c;
};

#endif