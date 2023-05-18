/*
 * Timer.h
 *
 *    Author: Ferdinand Hermann
 *     Email: uuzfi@student.kit.edu
 */

#ifndef TIMER_H_
#define TIMER_H_


/*
 * stdbool.h:               Boolean definitions for the C99 standard
 * stdint.h:                Variable definitions for the C99 standard
 * System.h:                Access to current CPU clock and other functions.
 * driverlib/timer.h        API declarations for use by applications (Treiberbib. S.531)
 * driverlib/interrupt.h    For Interrupt Handling
 */
#include <stdbool.h>
#include <stdint.h>
#include "System.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"



class Timer
{
public:
    Timer();
    virtual ~Timer();
    void init(System* sys, uint32_t base, void (*ISR)(void), uint32_t freq = 0);
    void start();
    void stop();
    void clearInterruptFlag();
    void setPeriodUS(uint32_t periodUS);
    void setFreq(uint32_t frequency);
    uint32_t getFreq();
    uint32_t getPeriodUS();

private:
    /*
     * The following array is needed to let the compiler know that the
     * precompiled class library needs space for its private variables.
     * Or in shorter terms: simply ignore it.
     */
    uint32_t spaceForLib[13];

    System* sys;
    uint32_t base, freq, periodUs, loadValue;

};

#endif /* TIMER_H_ */
