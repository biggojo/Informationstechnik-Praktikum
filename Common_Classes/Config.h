/*
 * Config.h
 *
 *    Author: Henri Hornburg
 *     Email: henri.hornburg@student.kit.edu
 * Co-author: Max Zuidberg
 *     Email: m.zuidberg@icloud.com
 *
 * Header file containing all configurable parameters of the segway, as for
 * example its pinout (which can be found in the file pinout.txt).
 * Notes: - all constants are prefixed by CFG_.
 *        - In this particular file comments in the same line are _highly_
 *          useful. This also means that the line limit is exceeded.
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/timer.h"


// Choose the device for which the code is compiled
#define MINISEG
// #define TIVSEG


// System
#define CFG_SYS_FREQ                     40000000           // CPU clock

#define CFG_DEBUG_TIMER_BASE             TIMER1_BASE        // Timer used to send debug data with a fixed frequency to the computer.
#define CFG_DEBUG_TIMER_FREQ             20                 // Frequency at which the computer receives new debug data.


// Main timer
#define CFG_MAIN_TIMER_BASE              TIMER0_BASE        // Timer used to run the segway code.


// Motors
#ifdef TIVSEG
#define CFG_PWM_INVERT                   true               // The TivSeg motor driver requires inverted PWM signals.
#endif
#ifdef MINISEG
#define CFG_PWM_INVERT                   false              // The MiniSeg motor driver requires non-inverted PWM signals.
#endif

#define CFG_LM_PORT                      GPIO_PORTE_BASE
#define CFG_LM_PIN1                      GPIO_PIN_4         // Left motor forward
#define CFG_LM_PIN2                      GPIO_PIN_5         // Left motor reverse
#define CFG_LM_FREQ                      2500               // Note: Due to the slow optocouplers of the TivSeg this frequency can't be much higher.

#define CFG_RM_PORT                      GPIO_PORTD_BASE
#define CFG_RM_PIN1                      GPIO_PIN_0         // Right motor forward
#define CFG_RM_PIN2                      GPIO_PIN_1         // Right motor reverse
#define CFG_RM_FREQ                      2500               // Note: Due to the slow optocouplers of the TivSeg this frequency can't be much higher.


// Enable Motors
#define CFG_EM_PORT                      GPIO_PORTD_BASE
#define CFG_EM_PIN                       GPIO_PIN_3         // PD5: Pin connected to the enable pin of the motor driver.
#define CFG_EM_DIR                       GPIO_DIR_MODE_OUT
#define CFG_EM_ACTIVE_STATE              1                  // Value to be written to activate motors (active high).


// Footswitch
#define CFG_FS_PORT                      GPIO_PORTB_BASE
#define CFG_FS_PIN                       GPIO_PIN_5
#define CFG_FS_DIR                       GPIO_DIR_MODE_IN
#define CFG_FS_PULLUP                    true               // active open between pin and GND
#define CFG_FS_ACTIVE_STATE              1                  // Read value which means it's pushed


// Angle rate and acceleration sensor
#define CFG_SENSOR_ADRESSBIT             0                  // State of the AD0 pin on the MPU6050
#define CFG_SENSOR_I2C_MODULE            I2C1_BASE          // I2C1: PA6 = SCL, PA7 = SDA
#define CFG_SENSOR_WHEEL_AXIS            'Y'
#define CFG_SENSOR_HOR_AXIS              'Z'
#define CFG_SENSOR_INVERT_ANGLE_RATE     false              // Rotating in driving direction is positive
#define CFG_SENSOR_INVERT_HOR            true               // Driving direction is positive
#define CFG_SENSOR_INVERT_VER            true               // Downwards is positive


// Battery voltage
#define CFG_BATT_BASE                    ADC0_BASE
#define CFG_BATT_SSEQ                    0
#define CFG_BATT_AIN                     ADC_CTL_CH1        // PE2
#define CFG_BATT_MIN                     21.0f
#define CFG_BATT_TIMEOUT                 5                  // Seconds until segway stops because of low battery.


// Steering
#define CFG_STEERING_BASE                ADC0_BASE
#define CFG_STEERING_SSEQ                1
#define CFG_STEERING_AIN                 ADC_CTL_CH2        // PE1


// Controller
#ifdef TIVSEG
#define CFG_CTLR_UPDATE_FREQ             100                // Setting for TivSeg
#define CFG_CTLR_FILTER_FACT             0.995f             // @100Hz update frequency. Determined by experiments.
#endif

#ifdef MINISEG
#define CFG_CTLR_UPDATE_FREQ             10                 // Setting for MiniSeg
#define CFG_CTLR_FILTER_FACT             0.95f              // Setting for MiniSeg
#endif

#define CFG_CTLR_MAX_SPEED               0.5f
#define CFG_CTLR_LOW_PASS_FACT           0.1f               // @100Hz update frequency. Determined by experiments.
#define CFG_CTLR_MAXDUTY                 0.9f               // Duty cycle needs to be limited to 0.9 for the motor driver of the TivSeg. Value could be 1.0 for MiniSeg

#endif /* CONFIG_H_ */
