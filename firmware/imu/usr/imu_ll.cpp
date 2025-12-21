#include "imu_ll.h"

#include "tmath.h"

#define MPU6050_ADDR    ((uint8_t)(0x68<<1))
#define PWR_MGMT_1      ((uint8_t)0x6b)

#define ACC_XOUT_H     ((uint8_t)0x3B)
#define ACC_YOUT_H     ((uint8_t)0x3D)
#define ACC_ZOUT_H     ((uint8_t)0x3F)


#define GYRO_XOUT_H     ((uint8_t)0x43)
#define GYRO_YOUT_H     ((uint8_t)0x45)
#define GYRO_ZOUT_H     ((uint8_t)0x47)


#define ACC_RANGE       ((float)2.0f*9.80665f)
#define GYRO_RANGE      ((float)250.0f*2.0f*PI/360.0f)


void IMULL::init(I2C_Interface &i2c)
{
    this->i2c = &i2c;

    ax = 0;
    ay = 0;
    az = 0;

    gx = 0;
    gy = 0;
    gz = 0;


    // mpu6050 turn on
    i2c.write_reg(MPU6050_ADDR, PWR_MGMT_1, 0x00);
}

void IMULL::read()      
{
    // read accelerometer   
    int16_t ax = read_word(MPU6050_ADDR, ACC_XOUT_H);
    int16_t ay = read_word(MPU6050_ADDR, ACC_YOUT_H);
    int16_t az = read_word(MPU6050_ADDR, ACC_ZOUT_H);    

    // read gyro
    int16_t gx = read_word(MPU6050_ADDR, GYRO_XOUT_H);
    int16_t gy = read_word(MPU6050_ADDR, GYRO_YOUT_H);
    int16_t gz = read_word(MPU6050_ADDR, GYRO_ZOUT_H);   
    
    // convert accelerometer to m/sˆ2
    this->ax = (ax*ACC_RANGE)/32768;
    this->ay = (ay*ACC_RANGE)/32768;
    this->az = (az*ACC_RANGE)/32768;
    
    // convert gyroscope to rad/s   
    this->gx = (gx*GYRO_RANGE)/32768;
    this->gy = (gy*GYRO_RANGE)/32768;
    this->gz = (gz*GYRO_RANGE)/32768;
}


// Helper function to read 16-bit signed word (high + low byte)
int16_t IMULL::read_word(unsigned char dev_adr, unsigned char reg_adr) 
{
    uint8_t high = i2c->read_reg(dev_adr, reg_adr);
    uint8_t low  = i2c->read_reg(dev_adr, reg_adr + 1);

    return (int16_t)((high << 8) | low);
}

