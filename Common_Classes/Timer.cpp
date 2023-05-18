/*
 * Timer.cpp
 *
 *    Author: Ferdinand Hermann
 *     Email: uuzfi@student.kit.edu
 */


// Uncomment following #define to use the precompiled Timer library instead of
// the code in this file.
//#define USE_TIMER_LIBRARY

#ifndef USE_TIMER_LIBRARY


#include <Timer.h>


Timer::Timer()
{
    /*
     * Default empty constructor
     */
}

Timer::~Timer()
{
    /*
     * Default empty destructor
     */
}

void Timer::init(System* sys, uint32_t base, void (*ISR)(void), uint32_t freq)
{
    /*speichern der übergebenen Variablen
     sys: Zeiger auf die Systemklasse
     base: Basisadresse Timermodul
     freq: Frequenz
    */
    this->sys = sys;
    this->base = base;
    this->freq = freq;

    /*Aktivieren der Timer  für 6 Timermodule
      While Schleife um zu warten, bis Timer bereit ist */

    switch (base)
    {
    case TIMER0_BASE:
        SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
        while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0))
        {
        }
        break;
    case TIMER1_BASE:
        SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
        while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER1))
            {
            }
        break;
    case TIMER2_BASE:
        SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
        while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER2))
            {
            }
        break;
    case TIMER3_BASE:
        SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);
        while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER3))
            {
            }
        break;
    case TIMER4_BASE:
        SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER4);
        while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER4))
            {
            }
        break;
    case TIMER5_BASE:
        SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER5);
        while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER5))
            {
            }
        break;
    default:
        sys -> error(TimerWrongConfig);
    }


    /*Timer als periodischen Full_Width Timer konfigurieren (TreiberBib. S.535f) */

    TimerConfigure(base, TIMER_CFG_PERIODIC);

    /*Frequenz festlegen*/

    setFreq(freq);

    /* Interrupt Service Routine hinzufügen und aktivieren (TreiberBib. S.543) */

    TimerIntRegister(base, TIMER_A, ISR);

    TimerIntEnable(base, TIMER_TIMA_TIMEOUT);



}

void Timer::start()
{
    /* Timer aktivieren*/
    TimerEnable(base, TIMER_A);
}

void Timer::stop()
{
    /* Timer deaktivieren*/
    TimerDisable(base, TIMER_A);

    /* Interrupt Flag loeschen*/
    clearInterruptFlag();
}

/*Methode liefert aktuelle Frequenz in Hz*/
uint32_t Timer::getFreq()
{
    return freq;
}

/*Methode liefert aktuelle Periode in Microsekunden*/
uint32_t Timer::getPeriodUS()
{
    return periodUs;
}

/*Methode um Frequenz festzulegen*/
void Timer::setFreq(uint32_t freq)
{
    this ->freq = freq;

    /* Bei Freq=0 auch Timerstartwert und Periode zu 0 setzen*/
    if (freq ==0)
    {
        loadValue = 0;

        periodUs = 0;
    }
    else
    {

    /* Berechnen des Startwerts des Timers in Abhängigkeit der gewuenschten Frequenz*/
    loadValue = (sys->getClockFreq())/freq - 1;

    periodUs = 1000000/freq;
    }

    /*Einstellen des Startwerts und damit der Frequenz*/
    TimerLoadSet64(base, loadValue);

}

/*Methode um Periodendauer festzulegen*/
void Timer::setPeriodUS(uint32_t periodUs)
{


    /*Wenn Periode 0 ist wird auch Frequenz zu 0 gesetzt und Timer gestoppt */
    if (periodUs == 0)
    {
     stop();

     freq = 0;
    }
    else
    {

        this ->periodUs = periodUs;
        this ->freq = 1000000/periodUs;

    /*Berechnung des Loadvalues aus Periode*/
    loadValue = periodUs * (sys->getClockFreq()/1000000) -1 ;

    /*Einstellen des Startwerts und damit der Periodendauer*/
    TimerLoadSet64(base, loadValue);

    }
}

/*Methode um ISR Flag zu löschen, damit ISR nicht dauerhaft aufgerufen wird*/
void Timer::clearInterruptFlag()
{
    TimerIntClear(base, TIMER_TIMA_TIMEOUT);
}

#endif
