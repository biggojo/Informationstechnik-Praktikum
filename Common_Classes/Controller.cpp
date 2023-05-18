/*
 * Controller.cpp
 *
 *    Author: Henri Hornburg
 *     Email: henri.hornburg@student.kit.edu
 * Co-author: Max Zuidberg
 *     Email: m.zuidberg@icloud.com
 *
 * The controller class contains the algorithm which determines the motor
 * drive speeds based on an angle rate, an angle value and a steering position.
 */

#include "Controller.h"

Controller::Controller()
{
    /*
     * Default empty constructor
     */
}

Controller::~Controller()
{
    /*
     * Default empty destructor
     */
}

void Controller::init(System *sys, float maxSpeed)
{
    /*
     * Initialize the controller by configuring the working and behavior
     * variables.
     *
     * sys: Pointer to the current System instance.
     */

    // Create local reference to the given System object.
    this->sys = sys;

    // We use floats, therefore we want to profit from the FPU.
    // Check if it's already enabled and if not, enable it.
    sys->enableFPU();

    this->maxSpeed = maxSpeed;

    // Initialize speed values
    resetSpeeds();
}

void Controller::resetSpeeds()
{
    /*
     * Reset all speed values to 0
     */

    driveSpeed = 0.0f;
    leftSpeed  = 0.0f;
    rightSpeed = 0.0f;
}

void Controller::updateValuesRad(float steeringValue, float angleRateRad,
                                 float accelHor, float accelVer)
{
    /*
     * Feed current sensor values into the controller to generate new PWM
     * values for the left and right motor.
     * Controller based on the P(I)D controller by http://www.tlb.org/#scooter
     * Careful! he uses radians, we too.
     *
     * steeringValue: value from -1.0f (right) to 1.0f (left).
     * angleRateRad:  angle rate around the wheel axis in radian
     * accelHor:      horizontal acceleration in g
     * accelVer:      vertical acceleration in g
     */

    // Get angle from accelerometer and gyrometer. Factor by experiments.
    // Based on http://www.ups.bplaced.de/Dokumentation/Runner%207.38.pdf
    float angleAccelRad = atan2f(-accelHor, -accelVer);
    angleRad = compFilter(integrate(angleRad, angleRateRad),
                              angleAccelRad, CFG_CTLR_FILTER_FACT);

    // A low pass filter to prevent higher frequency oscillations (forward -
    // backward). Factor by experiments.
    angleRate = compFilter(angleRateRad, angleRate, CFG_CTLR_LOW_PASS_FACT);

    /*
     * Calculate torque needed for balance.
     * Note: the reduction of the factor 0.4 to 0.2 is needed to prevent
     * higher frequency oscillations (forward - backward). Factor by
     * experiments.
     */
    torque = 5.0f * (angleRad - angleStableRad) + 0.2f * angleRate;

    // Speed limiter
    float overspeed = driveSpeed - maxSpeed;
    if (overspeed > 0.0f)
    {
        // too fast
        overspeed = fminf(0.2f, overspeed + 0.05f);
        overspeedInt = fminf(0.4f, integrate(overspeedInt, overspeed));
    }
    else
    {
        overspeed = 0.0f;

        // stop speed limiter
        if (overspeedInt > 0.0f)
        {
            overspeedInt -= 0.04f / CFG_CTLR_UPDATE_FREQ;
        }
    }

    // New stable position
    angleStableRad = 0.4f * overspeed + 0.7f * overspeedInt;

    // Reduce steering when driving faster
    float steeringAdjusted = 0.07f / (0.3f + fabsf(driveSpeed)) * steeringValue;

    // Update current drive speed
    driveSpeed = integrate(driveSpeed, 1.2f * torque);

    // Apply steering. Note: *increasing* leftSpeed actually causes the segway
    // to turn to the *right*!
    leftSpeed  = torque + driveSpeed + steeringAdjusted;
    rightSpeed = torque + driveSpeed - steeringAdjusted;

    sys->setDebugVal("Angle_[0.1deg]", angleRad * 1800.0f / 3.14159f);
}

float Controller::getLeftSpeed()
{
    /*
     * Returns the duty cycle for the left motor as float. 
     * Note: Because the H-Bridges of the TivSeg can't be driven at a duty
     *       cycle of -1.0f or 1.0f the duty cycle is limited by the Config
     *       parameter CFG_CTLR_MAXDUTY. It is part of the Config.h and not of
     *       the controller class because this limit depends on the actual
     *       hardware (whose properties shall be defined in Config.h).
     */

    // The controller itself has no real limitation. Therefore it is done here.
    if (leftSpeed > CFG_CTLR_MAXDUTY)
    {
        return CFG_CTLR_MAXDUTY;
    }
    else if (leftSpeed < -CFG_CTLR_MAXDUTY)
    {
        return -CFG_CTLR_MAXDUTY;
    }

    return leftSpeed;
}

float Controller::getRightSpeed()
{
    /*
     * Returns the duty cycle for the right motor as float. 
     * Note: Because the H-Bridges of the TivSeg can't be driven at a duty
     *       cycle of -1.0 or 1.0 the duty cycle is limited by the Config
     *       parameter CFG_CTLR_MAXDUTY. It is part of the Config.h and not of
     *       the controller class because this limit depends on the actual
     *       hardware (whose properties shall be defined in Config.h).
     */

    // The controller itself has no real limitation. Therefore it is done here.
    if (rightSpeed > CFG_CTLR_MAXDUTY)
    {
        return CFG_CTLR_MAXDUTY;
    }
    else if (rightSpeed < -CFG_CTLR_MAXDUTY)
    {
        return -CFG_CTLR_MAXDUTY;
    }

    return rightSpeed;
}

float Controller::getMaxSpeed()
{
    return maxSpeed;
}

void Controller::setMaxSpeed(float speed)
{
    maxSpeed = speed;
}

float Controller::integrate(float last, float current)
{
    /*
     * Integrates numerically.
     * Example: position = integrate(position, velocity)
     *
     * last:       Result of the last iteration
     * current:    value to integrate
     */

    return (last + current / CFG_CTLR_UPDATE_FREQ);
}

float Controller::arcTanDeg(float a, float b)
{
    /*
     * Returns arctan(a/b) in degree.
     */
    return (atan2f(a, b) * 180.0f / 3.14159265358979f);
}

float Controller::compFilter(float a, float b, float filterFactor)
{
    /*
     * Applies a complementary filter.
     */

    return (filterFactor * a + (1.0f - filterFactor) * b);
}
