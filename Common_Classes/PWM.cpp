/*
 * PWM.cpp
 *
 *    Author:Abraham Gassama
 *     Email:uqetv@student.kit.edu
 */


// Uncomment following #define to use the precompiled PWM library instead of
// the code in this file.
//#define USE_PWM_LIBRARY

#ifndef USE_PWM_LIBRARY


#include <PWM.h>


PWM::PWM()
{
    /*
     * Default empty constructor
     */
}

PWM::~PWM()
{
    /*
     * Default empty destructor
     */
}

void PWM::init(System* sys,uint32_t portBase, uint32_t pin1, uint32_t pin2, bool invert, uint32_t freq)
{
    /*
     * Initializes a PWM-modul and PWM-object. Every PWM-object belongs to one PWM-generator.
     *
     * sys:         Pointer to the current System instance. Needed for error
     *              handling.
     * portBase:    base address of the PWM port. Use the constants provided by
     *              the TivaWare API (inc/hw_memmap.h line 53-56, 73-76).
     * pin1 and 2:  pin number in the PWM port. Use the constants provided by
     *              the TivaWare API (driverlib/gpio.h line 60-67).
     * invert:      optional bool parameter to enable or disable the inverting of the signal
     * freq:        optional specification of the PWM-frequency in Hz. Standard is 5kHz
     * */


    // Speichern von system, frequency, pin1, pin2, portbase und bool invert

        pwmSys = sys;
        pwmFreq = freq;
        pwmPin1 = pin1;
        pwmPin2 = pin2;
        pwmPortBase = portBase;
        pwmInvert = invert;


        //Aktivieurng Floatrechnung
        pwmSys->enableFPU();

        // Berechnung des Ladewerts
        pwmLoadValue = (pwmSys->getClockFreq()/pwmSys->getPWMClockDiv())/pwmFreq;


        // array mit pin combination für Abfrage

        for (uint32_t i = 0; i < 8; i++ )
        {

            if ((pwmPortBase|pwmPin1|pwmPin2) == (array[i][0]|array[i][1]|array[i][2]))
            {

                //Adressen Speichern für setFreq und setDuty.
                pwmPinOutR = array[i][12];
                pwmPinOutV = array[i][11];
                pwmPinOutBitR = array[i][10];
                pwmPinOutBitV = array[i][9];
                pwmGen = array[i][8];
                pwmBase = array[i][7];

                //Freischalten der Basis
                SysCtlPeripheralEnable(array[i][3]);

                //Warten bis SysCtlPeripheral bereit ist
                while(!SysCtlPeripheralReady(array[i][3]));

                //PWM Modul aktivieren
                SysCtlPeripheralEnable(array[i][4]);

                //Warten
                while(!SysCtlPeripheralReady(array[i][4]));

                //Konfiguration der Pins als PWM Pins und zusätslich als outputs
                GPIOPinTypePWM(array[i][0], array[i][1]);
                GPIOPinTypePWM(array[i][0], array[i][2]);
                GPIOPinConfigure(array[i][5]);
                GPIOPinConfigure(array[i][6]);

                //Zählmodus (Count down modus sprich es wird runtergezählt)
                PWMGenConfigure(array[i][7], array[i][8], PWM_GEN_MODE_DOWN);

                //Feststellung der Periodendauer
                PWMGenPeriodSet(array[i][7], array[i][8], pwmLoadValue);

                //Outputs des Generator synchronisieren
                PWMOutputUpdateMode(array[i][7], array[i][9], PWM_OUTPUT_MODE_SYNC_LOCAL);
                PWMOutputUpdateMode(array[i][7], array[i][10], PWM_OUTPUT_MODE_SYNC_LOCAL);

                //Invertien der Outputs falls erwünscht
                PWMOutputInvert(array[i][7], array[i][9], pwmInvert);
                PWMOutputInvert(array[i][7], array[i][10], pwmInvert);

                //Schließung der Outputs um signal übertragungen zu vermeiden
                PWMOutputState(array[i][7], array[i][9], false);
                PWMOutputState(array[i][7], array[i][10], false);

                //Frequenz initialisieren
                setFreq(freq);

                //pwmGenerator starten
                PWMGenEnable(array[i][7], array[i][8]);

                //delay
                pwmSys->delayCycles(12);

                break;

            }
            if (i==7)
            {

                    //Falsche Kombination aus Basis und Pins.
                    pwmSys->error(GPIOWrongConfig, &portBase, &pin1, &pin2);

            }

        }

        };



void PWM::setFreq(uint32_t freq){

    newFreq = freq;

    //Nimmt die gewünschte neue Frequenz, berechnet dann den Loadwert
    pwmLoadValue = (pwmSys->getClockFreq()/pwmSys->getPWMClockDiv()) / newFreq;

    //neue Periode festlegen (Frequenz einsetzen)
    PWMGenPeriodSet(pwmBase, pwmGen, pwmLoadValue);

        }


void PWM::setDuty(float duty)
{

   newDuty = duty;
   //Compare value = (Period-1)*duty also ist der Compare value gleich null wenn die periode gleich 1 ist

   if (PWMGenPeriodGet(pwmBase, pwmGen) == 1)
   {

          PWMOutputState(pwmBase, pwmPinOutBitV, false);
          PWMOutputState(pwmBase, pwmPinOutBitR, false);

   }
   else
   {

       if(abs(newDuty)> 1 )
          {

              //Die Geschwindigkeit ist außerhalb des Bereichs [-1,1]
              pwmSys->error(PWMDutyOutOfRange, &duty);

          }
          else if (newDuty <=1.0f && newDuty >= 0.1f )
          {

              // Vorwärtsgang für 0.1 <= duty <= 1

              PWMPulseWidthSet(pwmBase, pwmPinOutV, newDuty * PWMGenPeriodGet(pwmBase, pwmGen));
              pwmSys->delayCycles(12);

              PWMOutputState(pwmBase, pwmPinOutBitV, true);
              PWMOutputState(pwmBase, pwmPinOutBitR, false);

          }
          else if (newDuty >= -1.0f && newDuty <= -0.1f)
          {

              //Rückwärtsgang für -1 <= duty <=-0.1

              PWMPulseWidthSet(pwmBase, pwmPinOutR, newDuty * PWMGenPeriodGet(pwmBase, pwmGen));
              pwmSys->delayCycles(12);

              PWMOutputState(pwmBase, pwmPinOutBitV, false);
              PWMOutputState(pwmBase, pwmPinOutBitR, true);

          }
          else if (newDuty < 0.1f && newDuty > - 0.1f )
          {
              PWMOutputState(pwmBase, pwmPinOutBitV, false);
              PWMOutputState(pwmBase, pwmPinOutBitR, false);

          }

   }


}

#endif
