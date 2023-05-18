/*
 * PWM.h
 *
 *    Author: Abraham Gassama
 *     Email:uqetv@student.kit.edu
 */

#ifndef PWM_H_
#define PWM_H_


/*
 * stdbool.h:               Boolean definitions for the C99 standard
 * stdint.h:                Variable definitions for the C99 standard
 * System.h:                Access to current CPU clock and other functions.
 */
#include <stdbool.h>
#include <stdint.h>
#include "System.h"
#include "GPIO.h"
#include "driverlib/pwm.h"
#include<cmath>

// define für Absolutbetrag
#define abs(x) ((x>0)?(x):(-x))

class PWM
{
public:
    PWM();
    virtual ~PWM();
    void init(System *sys,uint32_t portBase, uint32_t pin1, uint32_t pin2,
              bool invert = false, uint32_t freq = 5000);
    void setFreq(uint32_t freq);
    void setDuty(float duty);

private:
    /*
     * The following array is needed to let the compiler know that the
     * precompiled class library needs space for its private variables.
     * Or in shorter terms: simply ignore it.
     */
    uint32_t spaceForLib[92];

    System *pwmSys;
        uint32_t pwmBase, pwmPortBase, pwmPin1, pwmPin2, pwmLoadValue, pwmInvert, pwmFreq, pwmGen,
                 newpwmfreq, pwmPinOutV, pwmPinOutBitV, pwmPinOutR, pwmPinOutBitR;

        uint32_t test1, test2;

        uint32_t newFreq;

        // array mit pin combination für Abfrage
        uint32_t array[8][13] = {        {GPIO_PORTB_BASE, GPIO_PIN_6, GPIO_PIN_7, SYSCTL_PERIPH_GPIOB, SYSCTL_PERIPH_PWM0, GPIO_PB6_M0PWM0, GPIO_PB7_M0PWM1, PWM0_BASE, PWM_GEN_0, PWM_OUT_0_BIT, PWM_OUT_1_BIT, PWM_OUT_0, PWM_OUT_1},
                                         {GPIO_PORTB_BASE, GPIO_PIN_4, GPIO_PIN_5, SYSCTL_PERIPH_GPIOB, SYSCTL_PERIPH_PWM0, GPIO_PB4_M0PWM2, GPIO_PB5_M0PWM3, PWM0_BASE, PWM_GEN_1, PWM_OUT_2_BIT, PWM_OUT_3_BIT, PWM_OUT_2, PWM_OUT_3},
                                         {GPIO_PORTE_BASE, GPIO_PIN_4, GPIO_PIN_5, SYSCTL_PERIPH_GPIOE, SYSCTL_PERIPH_PWM0, GPIO_PE4_M0PWM4, GPIO_PE5_M0PWM5, PWM0_BASE, PWM_GEN_2, PWM_OUT_4_BIT, PWM_OUT_5_BIT, PWM_OUT_4, PWM_OUT_5},
                                         {GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_PIN_5, SYSCTL_PERIPH_GPIOC, SYSCTL_PERIPH_PWM0, GPIO_PC4_M0PWM6, GPIO_PC5_M0PWM7, PWM0_BASE, PWM_GEN_3, PWM_OUT_6_BIT, PWM_OUT_7_BIT, PWM_OUT_6, PWM_OUT_7},
                                         {GPIO_PORTD_BASE, GPIO_PIN_0, GPIO_PIN_1, SYSCTL_PERIPH_GPIOD, SYSCTL_PERIPH_PWM1, GPIO_PD0_M1PWM0, GPIO_PD1_M1PWM1, PWM1_BASE, PWM_GEN_0, PWM_OUT_0_BIT, PWM_OUT_1_BIT, PWM_OUT_0, PWM_OUT_1},
                                         {GPIO_PORTA_BASE, GPIO_PIN_6, GPIO_PIN_7, SYSCTL_PERIPH_GPIOA, SYSCTL_PERIPH_PWM1, GPIO_PA6_M1PWM2, GPIO_PA7_M1PWM3, PWM1_BASE, PWM_GEN_1, PWM_OUT_2_BIT, PWM_OUT_3_BIT, PWM_OUT_2, PWM_OUT_3},
                                         {GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_PIN_1, SYSCTL_PERIPH_GPIOF, SYSCTL_PERIPH_PWM1, GPIO_PF0_M1PWM4, GPIO_PF1_M1PWM5, PWM1_BASE, PWM_GEN_2, PWM_OUT_4_BIT, PWM_OUT_5_BIT, PWM_OUT_4, PWM_OUT_5},
                                         {GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_3, SYSCTL_PERIPH_GPIOF, SYSCTL_PERIPH_PWM1, GPIO_PF2_M1PWM6, GPIO_PF3_M1PWM7, PWM1_BASE, PWM_GEN_3, PWM_OUT_6_BIT, PWM_OUT_7_BIT, PWM_OUT_6, PWM_OUT_7}
                                        };

        float newDuty;

};

#endif /* PWM_H_ */
