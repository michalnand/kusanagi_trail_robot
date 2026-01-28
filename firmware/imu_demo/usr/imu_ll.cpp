#include "imu_ll.h"

#include "tmath.h"

// IMU : LSM6DSMTR
#define LSM6DSMTR_ADDR      ((uint8_t)0xD4) //already 8bit address
#define WHO_AM_I            ((uint8_t)0x0F)

#define WHO_AM_I_VAL        ((uint8_t)0x6A)


#define CTRL1_XL            ((uint8_t)0x10)
#define CTRL2_G             ((uint8_t)0x11)


// gyroscope X axis
#define OUTX_L_G            ((uint8_t)0x22)
#define OUTX_H_G            ((uint8_t)0x23)

// gyroscope Y axis
#define OUTY_L_G            ((uint8_t)0x24)
#define OUTY_H_G            ((uint8_t)0x25)

// gyroscope Z axis
#define OUTZ_L_G            ((uint8_t)0x26)
#define OUTZ_H_G            ((uint8_t)0x27)


// accelerometer X axis
#define OUTX_L_XL            ((uint8_t)0x28)
#define OUTX_H_XL            ((uint8_t)0x29)

// accelerometer Y axis
#define OUTY_L_XL            ((uint8_t)0x2A)
#define OUTY_H_XL            ((uint8_t)0x2B)

// accelerometer Z axis
#define OUTZ_L_XL            ((uint8_t)0x2C)
#define OUTZ_H_XL            ((uint8_t)0x2D)

// +-2g range, is 0.061mg/LSB
#define ACC_RANGE            ((float)0.061)

// +-500dps range, is 17.5mdps/LSB
#define GYRO_RANGE           ((float)0.0175)    

int IMULL::init(I2C_Interface &i2c)
{
    this->i2c = &i2c;

    ax = 0;
    ay = 0;
    az = 0;

    gx = 0; 
    gy = 0;
    gz = 0; 


    // LSM6DSMTR connection check
    if (read_status() != 1)
    {
        return -1;
    }
    
    
    // accelerometer init
    // +-2g range
    // 208Hz data rate
    this->i2c->write_reg(LSM6DSMTR_ADDR, CTRL1_XL, (1<<6)|(1<<5));

    // gyroscope init
    // 500 dps range
    // 208Hz data rate
    this->i2c->write_reg(LSM6DSMTR_ADDR, CTRL2_G, (1<<6)|(1<<4)|(1<<2));


    delay_loops(10000);

    // estimate gyro offset

    uint32_t n_samples = 256;

    this->gx_ofs = 0.0f;
    this->gy_ofs = 0.0f;
    this->gz_ofs = 0.0f;

    float gx_ofs_tmp = 0.0f;
    float gy_ofs_tmp = 0.0f;
    float gz_ofs_tmp = 0.0f;

    for (unsigned int i = 0; i < n_samples; i++)
    {
        this->read();   

        gx_ofs_tmp+= this->gx;
        gy_ofs_tmp+= this->gy;
        gz_ofs_tmp+= this->gz;

        delay_loops(1000);
    }

    
    this->gx_ofs = gx_ofs_tmp/n_samples;
    this->gy_ofs = gy_ofs_tmp/n_samples;
    this->gz_ofs = gz_ofs_tmp/n_samples;

    this->read();

    return 0;
}

// return acc in mg and gyro in mdps
void IMULL::read()      
{
    // read accelerometer   
    int16_t ax = read_word(LSM6DSMTR_ADDR, OUTX_L_XL);
    int16_t ay = read_word(LSM6DSMTR_ADDR, OUTY_L_XL);
    int16_t az = read_word(LSM6DSMTR_ADDR, OUTZ_L_XL);    

    // read gyro
    int16_t gx = read_word(LSM6DSMTR_ADDR, OUTX_L_G);
    int16_t gy = read_word(LSM6DSMTR_ADDR, OUTY_L_G);
    int16_t gz = read_word(LSM6DSMTR_ADDR, OUTZ_L_G);   
    
    // convert accelerometer to m/sˆ2
    this->ax = ax*ACC_RANGE;
    this->ay = ay*ACC_RANGE;
    this->az = az*ACC_RANGE;
    
    // convert gyroscope to rad/s   
    this->gx = gx*GYRO_RANGE - this->gx_ofs;
    this->gy = gy*GYRO_RANGE - this->gy_ofs;
    this->gz = gz*GYRO_RANGE - this->gz_ofs;
}   


int IMULL::read_status()
{
    if (i2c->read_reg(LSM6DSMTR_ADDR, WHO_AM_I) != WHO_AM_I_VAL)
    {
        return 0;
    }
    
    return 1;
}


// Helper function to read 16-bit signed word (high + low byte)
int16_t IMULL::read_word(unsigned char dev_adr, unsigned char reg_adr) 
{
    uint8_t low   = i2c->read_reg(dev_adr, reg_adr);    
    uint8_t high  = i2c->read_reg(dev_adr, reg_adr + 1);

    return (int16_t)((high << 8) | low);
}

