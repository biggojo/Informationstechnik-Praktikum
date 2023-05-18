/*
 * MPU6050.cpp
 *
 *    Author: Max Zuidberg
 *     Email: m.zuidberg@icloud.com
 */

#include "MPU6050.h"


MPU6050::MPU6050()
{
    /*
     * Default empty constructor
     */
}

MPU6050::~MPU6050()
{
    /*
     * Default empty destructor
     */
}

void MPU6050::init(System *sys, uint32_t I2CBase, bool addressBit,
                   char wheelAxis, char horAxis)
{
    /*
     * Initialize the I2C bus for communicating with an MPU6050 sensor and
     * initialize this sensor.
     *
     * sys:        Pointer to the current System instance. Needed for error
     *             handling.
     * I2CBase:    base address of the I2C module the sensor is connected to
     * addressBit: value of the address pin (AD0) on the sensor determining
     *             the I2C slave address of it.
     * wheelAxis:  Sensor axis which is parallel to the axis of the wheels.
     * horAxis:    Sensor axis which - apart from the wheelAxis - is horizontal
     *             (in calibration position).
     */

    // Create private reference to the given System object.
    this->sys = sys;

    // Save I2C Module base address
    this->i2cBase = I2CBase;

    // Determine which I2C module is used (hw_memmap.h line 69).
    this->i2cModuleNum = (i2cBase - I2C0_BASE) / 0x1000;

    // Determine I2C Address (MPU-6050 datasheet page 15)
    this->address = 0b1101000 + addressBit;

    // We use floats
    sys->enableFPU();

    // Enable the GPIO port with the I2C pins.
    SysCtlPeripheralEnable(I2C_CONSTANTS[i2cModuleNum][GPIO_PERIPH]);
    SysCtlPeripheralEnable(I2C_CONSTANTS[i2cModuleNum][I2C_PERIPH]);

    // Wait until peripheral is enabled ("TivaWare(TM) Treiberbibliothek"
    // page 502, "TivaC Launchpad Workshop" page 78)
    sys->delayCycles(5);

    // Set pin types to I2C SCL/SDA pins.
    GPIOPinTypeI2CSCL(I2C_CONSTANTS[i2cModuleNum][GPIO_BASE],
                      I2C_CONSTANTS[i2cModuleNum][SCL_PIN]);
    GPIOPinTypeI2C(I2C_CONSTANTS[i2cModuleNum][GPIO_BASE],
                   I2C_CONSTANTS[i2cModuleNum][SDA_PIN]);

    // Configure these two pins.
    GPIOPinConfigure(I2C_CONSTANTS[i2cModuleNum][SCL_PIN_CFG]);
    GPIOPinConfigure(I2C_CONSTANTS[i2cModuleNum][SDA_PIN_CFG]);

    // Initialize Master and Slave to communicate at 400kb/s
    // (set fast-mode (400kb/s) to true).
    I2CMasterInitExpClk(i2cBase, sys->getClockFreq(), true);

    /*
     * Check whether communicating with the MPU6050 works.
     *
     * Note: The MPU-6050s WHO_AM_I register stores the I2C address without
     *       address bit. However there's a variant of the MPU-6050, the
     *       MPU-6052C whose WHO_AM_I value seems to be 0b01110010 (although
     *       both variants have the same I2C address).
     *       Unfortunately no Register Map could be found for this variant.
     */
    uint8_t whoAmI = getRegister(MPU_REG_WHO_AM_I);
    if (whoAmI + addressBit != address && whoAmI != 0b1110010)
    {
        sys->error(MPUCommunicationError);
    }

    /*
     * Initialize MPU6050.
     * For more information see MPU6050 Register Map and Descriptions
     */
    // Set the clock source to PLL with X axis gyroscope reference.
    setRegister(MPU_REG_PWR_MGMT_1, 0x01);

    // Configure the MPU6050 to disable external Frame Synchronization, to let
    // the accelerometer run unfiltered at 1kHz and the gyroscope at 8kHz.
    setRegister(MPU_REG_CONFIG, 0x00);

    // Set the gyroscope sample rate divider to 1.
    setRegister(MPU_REG_SMPLRT_DIV, 0x00);

    // Configure the gyro to the appropriate full scale range and do not
    // perform a self-test.
    uint8_t range;
    switch (GYRO_RANGE)
    {
    case 2000:
        range = 0b11000;
        break;
    case 1000:
        range = 0b10000;
        break;
    case 500:
        range = 0b01000;
        break;
    case 250:
        range = 0b00000;
        break;
    default:
        sys->error(MPUWrongGyroRange);
    }
    setRegister(MPU_REG_GYRO_CONFIG, range);

    // Configure the accelerometer to the appropriate full scale range and do
    // not perform self-test.
    switch (ACCEL_RANGE)
    {
    case 16:
        range = 0b11000;
        break;
    case 8:
        range = 0b10000;
        break;
    case 4:
        range = 0b01000;
        break;
    case 2:
        range = 0b00000;
        break;
    default:
        sys->error(MPUWrongAccelRange);
    }
    setRegister(MPU_REG_ACCEL_CONFIG, range);

    // Configure sensor orientation
    setWheelAxis(wheelAxis);
    setHorAxis(horAxis);
}

void MPU6050::setWheelAxis(char axis)
{
    /*
     * Tell the MPU6050 object which one of the sensor's axes is parallel
     * to the axis of the wheels. This is the axis of which the gyrometer
     * measures the angle rate.
     * Note: this method also resets the horizontal and vertical axis
     *       (as changing the wheel axis necessitates a change to one of
     *       these). Remember to call MPU6050::setHorAxis afterwards.
     *
     * axis: the character 'x' ('X'), 'y' ('Y') or 'z' ('Z')
     */

    // Convert to lower case (see ASCII table)
    this->axis = (axis < 'a') ? (axis + 0x20) : axis;

    // set the gyrometer register we want to read to the right axis.
    switch (this->axis)
    {
    case 'x':
        angleRateRegister = MPU_REG_GYRO_XOUT_H;
        accelHorRegister = MPU_REG_ACCEL_YOUT_H;
        accelVerRegister = MPU_REG_ACCEL_ZOUT_H;
        break;
    case 'y':
        angleRateRegister = MPU_REG_GYRO_YOUT_H;
        accelHorRegister = MPU_REG_ACCEL_XOUT_H;
        accelVerRegister = MPU_REG_ACCEL_ZOUT_H;
        break;
    case 'z':
        angleRateRegister = MPU_REG_GYRO_ZOUT_H;
        accelHorRegister = MPU_REG_ACCEL_XOUT_H;
        accelVerRegister = MPU_REG_ACCEL_YOUT_H;
        break;
    default:
        sys->error(MPUWrongWheelAxis, &axis);
    }
}

void MPU6050::setHorAxis(char hor)
{
    /*
     * Set horizontal axis (in calibration position) of the sensor which must
     * not be the wheel axis.
     *
     * hor: x, y, or z (lower or upper case)
     */

    // Convert to lower case
    hor = (hor < 'a') ? (hor + 0x20) : hor;

    //
    if (hor == axis)
    {
        sys->error(MPUHorEqualsWheelAxis, &hor);
    }

    switch (hor)
    {
        case 'x':
        accelHorRegister = MPU_REG_ACCEL_XOUT_H;
        if (axis == 'z')
        {
            accelVerRegister = MPU_REG_ACCEL_YOUT_H;
        }
        else
        {
            accelVerRegister = MPU_REG_ACCEL_ZOUT_H;
        }
        break;
        case 'y':
        accelHorRegister = MPU_REG_ACCEL_YOUT_H;
        if (axis == 'x')
        {
            accelVerRegister = MPU_REG_ACCEL_ZOUT_H;
        }
        else
        {
            accelVerRegister = MPU_REG_ACCEL_XOUT_H;
        }
        break;
        case 'z':
        accelHorRegister = MPU_REG_ACCEL_ZOUT_H;
        if (axis == 'y')
        {
            accelVerRegister = MPU_REG_ACCEL_XOUT_H;
        }
        else
        {
            accelVerRegister = MPU_REG_ACCEL_YOUT_H;
        }
        break;
        default:
        sys->error(MPUWrongHorAxis, &hor);
    }
}

void MPU6050::angleRateInvertSign(bool invertSign)
{
    /*
     * Depending on the orientation of the MPU-6050 sensor you have to invert
     * the sign.
     *
     * invertSign: whether to invert the sign or not.
     */

    if (invertSign)
    {
        angleRateSign = -1.0f;
    }
    else
    {
        angleRateSign = 1.0f;
    }
}

void MPU6050::accelHorInvertSign(bool invertSign)
{
    /*
     * Depending on the orientation of the MPU-6050 sensor you have to
     * invert the sign.
     * invertSign: whether to invert the sign or not.
     */

    if (invertSign)
    {
        accelHorSign = -1.0f;
    }
    else
    {
        accelHorSign = 1.0f;
    }
}

void MPU6050::accelVerInvertSign(bool invertSign)
{
    /*
     * Depending on the orientation of the MPU-6050 sensor you have to
     * invert the sign.
     *
     * invertSign: whether to invert the sign or not.
     */

    if (invertSign)
    {
        accelVerSign = -1.0f;
    }
    else
    {
        accelVerSign = 1.0f;
    }
}

float MPU6050::getAngleRate()
{
    /*
     * Return the angle rate in °/s from the corresponding gyro.
     */

    int16_t rawAngleRate = (getRegister(angleRateRegister) << 8)
                           | getRegister(angleRateRegister + 1);
    return (angleRateSign * rawAngleRate * GYRO_RANGE) / (1 << 15);
}

float MPU6050::getAccelHor()
{
    /*
     * Return the horizontal acceleration in g from the corresponding
     * accelerometer.
     */

    int16_t rawAccelHor = (getRegister(accelHorRegister) << 8)
                          | getRegister(accelHorRegister + 1);
    return (accelHorSign * rawAccelHor * ACCEL_RANGE) / (1 << 15);
}

float MPU6050::getAccelVer()
{
    /*
     * Return the vertical acceleration in g from the corresponding
     * accelerometer.
     */

    int16_t rawAccelVer = (getRegister(accelVerRegister) << 8)
                          | getRegister(accelHorRegister + 1);
    return (accelVerSign * rawAccelVer * ACCEL_RANGE) / (1 << 15);
}

void MPU6050::setRegister(uint8_t reg, uint8_t val)
{
    /*
     * Set a register reg from the MPU to the value val.
     *
     * reg: Address of the register to be overwritten
     * val: Value to be written to the register
     */

    // Tell the master module what address it will place on the bus when
    // communicating with the slave.
    I2CMasterSlaveAddrSet(i2cBase, address, false);

    // Tell the MPU6050 which register is modified.
    I2CMasterDataPut(i2cBase, reg);

    // Initiate send of data from the MCU
    I2CMasterControl(i2cBase, I2C_MASTER_CMD_BURST_SEND_START);

    // Wait until MCU is done transferring.
    while (I2CMasterBusy(i2cBase));

    // Put value to be written in the data register
    I2CMasterDataPut(i2cBase, val);

    // End transmission
    I2CMasterControl(i2cBase, I2C_MASTER_CMD_BURST_SEND_FINISH);

    // Wait until MCU is done transferring.
    while (I2CMasterBusy(i2cBase));
}

uint32_t MPU6050::getRegister(uint8_t reg)
{
    /*
     * Read a register from the MPU.
     * reg: Address of the register to be read
     * Return value: value of this register
     *
     * This code is based on the example from https://www.digikey.com/eewiki/
     * display/microcontroller/I2C+Communication+with+the+TI+Tiva+TM4C123GXL
     */

    // specify that we are writing (a register address) to the slave device
    I2CMasterSlaveAddrSet(i2cBase, address, false);

    // specify register to be read
    I2CMasterDataPut(i2cBase, reg);

    // send control byte and register address byte to slave device
    I2CMasterControl(i2cBase, I2C_MASTER_CMD_SINGLE_SEND);

    // wait for MCU to finish transaction with a timeout of 1000us
    waitWithTimeoutUS(1000);

    // specify that we are going to read from slave device
    I2CMasterSlaveAddrSet(i2cBase, address, true);

    // send control byte and read from the register we specified
    I2CMasterControl(i2cBase, I2C_MASTER_CMD_SINGLE_RECEIVE);

    // wait for MCU to finish transaction with a timeout of 1000us
    waitWithTimeoutUS(1000);

    // return data pulled from the specified register
    return I2CMasterDataGet(i2cBase);
}

void MPU6050::waitWithTimeoutUS(uint32_t timeoutUS)
{
    /*
     * Wait until I2C bus isn't busy anymore. If this takes longer than the
     * specified timeout, a MPUCommunicationError is thrown.
     *
     * timeoutUS: Optional timeout in us. Default is 1000us.
     */
    uint32_t timeUS = 0;
    const uint32_t timeStepUS = 10;
    while (I2CMasterBusy(i2cBase))
    {
        if (timeUS >= timeoutUS)
        {
            sys->error(MPUCommunicationError);
        }
        sys->delayUS(timeStepUS);
        timeUS += timeStepUS;
    }
}
