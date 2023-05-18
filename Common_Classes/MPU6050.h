/*
 * MPU6050.h
 *
 *    Author: Max Zuidberg
 *     Email: m.zuidberg@icloud.com
 */

#ifndef MPU6050_H_
#define MPU6050_H_


/*
 * stdbool.h:               Boolean definitions for the C99 standard
 * stdint.h:                Variable definitions for the C99 standard
 * inc/hw_memmap.h:         Macros defining the memory map of the Tiva C Series
 *                          device. This includes defines such as peripheral
 *                          base address locations such as GPIO_PORTF_BASE.
 * inc/hw_pinmap.h:         Mapping of peripherals to pins for all parts.
 * driverlib/sysctl.h:      Defines and macros for the System Control API of
 *                          DriverLib. This includes API functions such as
 *                          SysCtlClockSet.
 * driverlib/i2c.h          Defines and macros for I2C API of DriverLib. This
 *                          includes API functions such as I2CMasterControl.
 * driverlib/gpio.h:        Defines and macros for GPIO API of DriverLib. This
 *                          includes API functions such as GPIOPinConfigure.
 * System.h:                Access to current CPU clock and other functions.
 */
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/i2c.h"
#include "driverlib/gpio.h"
#include "System.h"


class MPU6050
{
public:
    MPU6050();
    virtual ~MPU6050();
    void init(System *sys, uint32_t I2CBase, bool addressBit,
              char wheelAxis = 'x', char horAxis = 'y');
    void setWheelAxis(char axis);
    void setHorAxis(char hor);
    void angleRateInvertSign(bool invertSign);
    void accelHorInvertSign(bool invertSign);
    void accelVerInvertSign(bool invertSign);
    float getAngleRate();
    float getAccelHor();
    float getAccelVer();
private:
    uint32_t getRegister(uint8_t reg);
    void setRegister(uint8_t reg, uint8_t val);
    void waitWithTimeoutUS(uint32_t timeoutUS = 1000);
    System *sys;
    uint32_t i2cBase, address;
    uint8_t i2cModuleNum;
    float angleRateSign = 1.0f;
    float accelHorSign = 1.0f;
    float accelVerSign = 1.0f;
    uint8_t angleRateRegister, accelHorRegister, accelVerRegister;
    char axis;
    const uint16_t GYRO_RANGE = 250; // [deg/s]
    const uint8_t ACCEL_RANGE = 2;   // [g]
    const uint8_t I2C_PERIPH  = 0,
                  GPIO_PERIPH = 1,
                  GPIO_BASE   = 2,
                  SCL_PIN     = 3,
                  SDA_PIN     = 4,
                  SCL_PIN_CFG = 5,
                  SDA_PIN_CFG = 6;
    const uint32_t I2C_CONSTANTS[4][7] =
                 {{SYSCTL_PERIPH_I2C0, SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE,
                   GPIO_PIN_2, GPIO_PIN_3, GPIO_PB2_I2C0SCL, GPIO_PB3_I2C0SDA},
                  {SYSCTL_PERIPH_I2C1, SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE,
                   GPIO_PIN_6, GPIO_PIN_7, GPIO_PA6_I2C1SCL, GPIO_PA7_I2C1SDA},
                  {SYSCTL_PERIPH_I2C2, SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE,
                   GPIO_PIN_4, GPIO_PIN_5, GPIO_PE4_I2C2SCL, GPIO_PE5_I2C2SDA},
                  {SYSCTL_PERIPH_I2C3, SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE,
                   GPIO_PIN_0, GPIO_PIN_1, GPIO_PD0_I2C3SCL, GPIO_PD1_I2C3SDA}};

    // Register addresses of the MPU6050
    const uint8_t MPU_REG_SMPLRT_DIV   = 0x19;
    const uint8_t MPU_REG_CONFIG       = 0x1a;
    const uint8_t MPU_REG_GYRO_CONFIG  = 0x1b;
    const uint8_t MPU_REG_ACCEL_CONFIG = 0x1c;
    const uint8_t MPU_REG_WHO_AM_I     = 0x75;
    const uint8_t MPU_REG_PWR_MGMT_1   = 0x6b;

    // Addresses of the registers with the MSB part (xxx_H) of the sensor values
    // The LSB part is always stored in xxx_H + 1
    const uint8_t MPU_REG_ACCEL_XOUT_H = 0x3b;
    const uint8_t MPU_REG_ACCEL_YOUT_H = 0x3d;
    const uint8_t MPU_REG_ACCEL_ZOUT_H = 0x3f;
    const uint8_t MPU_REG_TEMP_OUT_H   = 0x41;
    const uint8_t MPU_REG_GYRO_XOUT_H  = 0x43;
    const uint8_t MPU_REG_GYRO_YOUT_H  = 0x45;
    const uint8_t MPU_REG_GYRO_ZOUT_H  = 0x47;
};

#endif /* MPU6050_H_ */
