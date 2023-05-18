/*
 * GPIO.h
 *
 *    Author: Max Zuidberg
 *     Email: m.zuidberg@icloud.com
 */

#ifndef GPIO_H_
#define GPIO_H_


/*
 * stdbool.h:               Boolean definitions for the C99 standard
 * stdint.h:                Variable definitions for the C99 standard
 * inc/hw_types.h:          Macros for hardware access, both direct and via the
 *                          bit-band region.
 * inc/hw_memmap.h:         Macros defining the memory map of the Tiva C Series
 *                          device. This includes defines such as peripheral
 *                          base address locations such as GPIO_PORTF_BASE.
 * driverlib/sysctl.h:      Defines and macros for the System Control API of
 *                          DriverLib. This includes API functions such as
 *                          SysCtlClockSet.
 * driverlib/gpio.h:        Defines and macros for GPIO API of DriverLib. This
 *                          includes API functions such as GPIOPinWrite.
 * System.h:                Access to current CPU clock and other functions.
 */
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "System.h"


class GPIO
{
public:
    GPIO();
    virtual ~GPIO();
    void init(System *sys, uint32_t portBase, uint32_t pin, uint32_t dir,
              bool pullup = false);
    bool read();
    void write(bool state);
    uint32_t getCurrent();
    void setCurrent(uint32_t current);
    void setPullup(bool enabled);
    void setPulldown(bool enabled);

private:
    System *sys;
    uint32_t portBase, pin, dir, current, pinType;

    // Note: Index 0 should not occur; any value could be here.
    const uint32_t HALF_CURRENT_TO_PARAM[7] = {0,
                                                GPIO_STRENGTH_2MA,
                                                GPIO_STRENGTH_4MA,
                                                GPIO_STRENGTH_6MA,
                                                GPIO_STRENGTH_8MA,
                                                GPIO_STRENGTH_10MA,
                                                GPIO_STRENGTH_12MA};
    void refreshConfig();
};

#endif /* GPIO_H_ */
