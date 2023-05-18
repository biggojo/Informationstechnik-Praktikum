/*
 * Segway.cpp
 *
 *    Author: Henri Hornburg
 *     Email: henri.hornburg@student.kit.edu
 * Co-author: Max Zuidberg
 *     Email: m.zuidberg@icloud.com
 *
 * The segway class contains all the code needed drive a segway.
 */

#include <Segway.h>

Segway::Segway()
{
    /*
     * Default empty constructor
     */
}

Segway::~Segway()
{
    /*
     * Default empty destructor
     */
}

//Objekte definieren
Timer timerBatterie;
PWM duty;
bool timerOn = false;

void BatterieISR(){

    //Interrupt Flag löschen
    timerBatterie.clearInterruptFlag();

    //Motor stoppen
    duty.setDuty(0);

}

void Segway::init(System *sys)
{
    /*  Initializes all the objects used in the segway class.
     *  Needs to be called before any components can be used
     *
     * sys: Pointer to the current System instance. Needed to get CPU clock
     *      frequency.
     */

    // Create private reference to the given System object.
    this->sys = sys;

    // Initialize all objects with the given parameters and the parameters from
    // the Config header file.
    leftMotor.init(sys,
                   CFG_LM_PORT,
                   CFG_LM_PIN1,
                   CFG_LM_PIN2,
                   CFG_PWM_INVERT,
                   CFG_LM_FREQ);
    rightMotor.init(sys,
                    CFG_RM_PORT,
                    CFG_RM_PIN1,
                    CFG_RM_PIN2,
                    CFG_PWM_INVERT,
                    CFG_RM_FREQ);
    enableMotors.init(sys,
                      CFG_EM_PORT,
                      CFG_EM_PIN,
                      CFG_EM_DIR);
    footSwitch.init(sys,
                    CFG_FS_PORT,
                    CFG_FS_PIN,
                    CFG_FS_DIR,
                    CFG_FS_PULLUP);
    steering.init(sys,
                  CFG_STEERING_BASE,
                  CFG_STEERING_SSEQ,
                  CFG_STEERING_AIN);
    controller.init(sys,
                    CFG_CTLR_MAX_SPEED);
    batteryVoltage.init(sys,
                        CFG_BATT_BASE,
                        CFG_BATT_SSEQ,
                        CFG_BATT_AIN);
    sensor.init(sys,
                CFG_SENSOR_I2C_MODULE,
                CFG_SENSOR_ADRESSBIT);

    // Configure sensor orientation
    sensor.setWheelAxis(CFG_SENSOR_WHEEL_AXIS);
    sensor.setHorAxis(CFG_SENSOR_HOR_AXIS);
    sensor.accelHorInvertSign(CFG_SENSOR_INVERT_HOR);
    sensor.accelVerInvertSign(CFG_SENSOR_INVERT_VER);
    sensor.angleRateInvertSign(CFG_SENSOR_INVERT_ANGLE_RATE);

    // This Enable Motors Pin is only needed for compatibility with the TivSeg
    // Hardware. It is not used at any other place in the code.
    enableMotors.write(CFG_EM_ACTIVE_STATE);

    // We use floats, therefore we want to profit from the FPU.
    sys->enableFPU();

    /*Warten bis Lenkung kalibriert ist*/
    while(calibrated == false)
    {
        steering.calibrateSteering();

        calibrated = true;
    }

    // Initializing done, segway is ready but not active yet.
    standby = true;
}

void Segway::update()
{
    /*
     * Read all sensor inputs, hand them to the controller and apply the
     * resulting motor duty cycles.
     */

    // Get state of the foot switch.
    bool footSwitchPressed = (footSwitch.read() == CFG_FS_ACTIVE_STATE);

    /*
     * Standby Mode controls whether the segway runs or not. The segway leaves
     * standby when the food switch is pressed and returns to standby if it's
     * released.
     * This is the polling variant, the other variant would be rising and
     * falling edge interrupts, which would be more complicated.
     */
    if (standby == true)
    {
        if (footSwitchPressed)
        {
            // Someone stepped on the segway, so we can leave standby and
            // start driving.
            standby = false;
        }
    }
    else
    {
        if (footSwitchPressed)
        {
            // User is still standing on the foot switch and we're not in
            // standby but in driving mode.

            // Variables for debugging purpose only

            float steeringValue = steering.getValue();

            // Get current angle rate in rad from the gyro
            float angleRateRad = sensor.getAngleRate() * 3.14159265358979f / 180.0f;

            // Get current accelerations in g from the accelerometer
            float accelHor = sensor.getAccelHor();
            float accelVer = sensor.getAccelVer();

            // Feed the new sensor data into the controller
            controller.updateValuesRad(steeringValue, angleRateRad, accelHor, accelVer);

            float leftMotorDuty = controller.getLeftSpeed();
            float rightMotorDuty = controller.getRightSpeed();

            // Apply the new duty cycles to the motors
            leftMotor.setDuty(leftMotorDuty);
            rightMotor.setDuty(rightMotorDuty);

            // Monitor the most important values. Note: The current tilt angle
            // is calculated and monitored inside the Controller class.
            sys->setDebugVal("Ue", steering.getUe()*100);
            sys->setDebugVal("Steering_Value_[%]", steeringValue * 100);
            sys->setDebugVal("Left_Speed_[%]" , leftMotorDuty * 100);
            sys->setDebugVal("Right_Speed_[%]" , rightMotorDuty * 100);

            //Akkuspannung plotten um Batteriespannungsüberwachung zu testen
            sys->setDebugVal("Akkuspannung" , batteryVoltage.readVolt() * 100);
        }
        else
        {
            // User is not standing on the foot switch anymore (maybe not even
            // on the segway either)

            // Stop the motors and reset all speeds to 0 as the segway is not
            // moving in standby.
            controller.resetSpeeds();
            leftMotor.setDuty(0);
            rightMotor.setDuty(0);

            standby = true;
        }
    }

    // Successfully passed the update method.
    updateFlag = true;
}


void Segway::backgroundTasks()
{
    /*
     * While the Segway::update() method runs the time-critical controller
     * related code, this method can run in background for monitoring tasks.
     * Some tasks need to sync with the ISR/Segway::update method. That's what
     * the segwayUpdateFlag is for.
     */

    if (updateFlag == true)
        {
            /*Während Spannung unter 2,1 V ist wird nach oben gezählt*/
            if (batteryVoltage.readVolt() < CFG_BATT_MIN/10)
            {
                counter = counter + 1;
            }

            /*ansonsten wird Zähler = 0 gesetzt*/
            else
            {
            counter = 0;
            }

            /* Nach 5sec werden Motoren ausgeschaltet*/
            if (counter > 50)
            {
                controller.resetSpeeds();
                leftMotor.setDuty(0);
                rightMotor.setDuty(0);

                standby = true;
            }

        updateFlag = false;

        }
}
