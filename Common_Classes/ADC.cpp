/*
 * ADC.cpp
 *
 *    Author: Ozeina Kansoh
 *     Email: uysyf@student.kit.edu
 */


// Uncomment following #define to use the precompiled ADC library instead of
// the code in this file.
#define USE_ADC_LIBRARY

#ifndef USE_ADC_LIBRARY


#include <ADC.h>


ADC::ADC()
{
    /*
     * Default empty constructor
     */
}

ADC::~ADC()
{
    /*
     * Default empty destructor
     */
}

void ADC::init(System *sys, uint32_t base, uint32_t sampleSeq, uint32_t analogInput)
{
    // Übergabeparameter festlegen
    this->sys = sys;
    this->base = base;
    this->sampleSeq = sampleSeq;
    this->analogInput = analogInput;


    //Uebergebe Base um Peripherigeraete zu aktivieren oder Fehlermeldung
    switch(base)
    {
    case ADC0_BASE:
        SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
        break;
    case ADC1_BASE:
        SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
        break;
    default:
        sys->error(ADCWrongConfig, &base, &sampleSeq, &analogInput);
    }

    // Je nach analogInput den Pin aktivieren
    // siehe Datenblatt S. 801
    switch(analogInput)
    {
    case(ADC_CTL_CH0):
            //PE3 aktivieren
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
            GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);
            break;
    case(ADC_CTL_CH1):
            //PE2 aktivieren
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
            GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_2);
            break;
    case(ADC_CTL_CH2):
            //PE1 aktivieren
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
            GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_1);
            break;
    case(ADC_CTL_CH3):
            //PE0 aktivieren
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
            GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_0);
            break;
    case(ADC_CTL_CH4):
            //PD3 aktivieren
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
            GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_3);
            break;
    case(ADC_CTL_CH5):
            //PD2 aktivieren
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
            GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_2);
            break;
    case(ADC_CTL_CH6):
            //PD1 aktivieren
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
            GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_1);
            break;
    case(ADC_CTL_CH7):
            //PD0 aktivieren
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
            GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_0);
            break;
    case(ADC_CTL_CH8):
            //PE5 aktivieren
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
            GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_5);
            break;
    case(ADC_CTL_CH9):
            //PE4 aktivieren
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
            GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_4);
            break;
    case(ADC_CTL_CH10):
            //PB4 aktivieren
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
            GPIOPinTypeADC(GPIO_PORTB_BASE, GPIO_PIN_4);
            break;
    case(ADC_CTL_CH11):
            //PB5 aktivieren
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
            GPIOPinTypeADC(GPIO_PORTB_BASE, GPIO_PIN_5);
            break;
    }

    //ADCSequenz Konfigurieren -> Start durch den Prozessor generiert
    ADCSequenceConfigure(base, sampleSeq, ADC_TRIGGER_PROCESSOR, sampleSeq);

    /*
     * Konfiguriere ADCSequence fuer einen Schritt
     * ADC_CTL_END: sagt der ADC logik, dass dies der letzte Schritt vom sequencer ist
     * Prioritaet einfach auf 0, da keine weiteren Prozesse
     */
    ADCSequenceStepConfigure(base, sampleSeq, 0, analogInput | ADC_CTL_END);

    // Sequenzierer aktivieren
    ADCSequenceEnable(base, sampleSeq);

}


void ADC::setHWAveraging(uint32_t averaging)
{
    // averaging entspricht der Anzahl Messwerten -> jede 2er Potenz bis einschließlich 64
        ADCHardwareOversampleConfigure(base, averaging);
}

uint32_t ADC::read()
{
    //Interrupts während dem Auslesen deaktivieren
    IntMasterDisable();

    //Interrupts löschen
    ADCIntClear(base, sampleSeq);

    //Prozessor triggern
    ADCProcessorTrigger(base, sampleSeq);

    //Aus "TivaC Launchpad Workshop" S.118
    while(!ADCIntStatus(base, sampleSeq, false))
     {
     }

    //Daten holen
    ADCSequenceDataGet(base, sampleSeq, &readValue);

    //Interrupts nach dem Auslesen wieder aktivieren
    IntMasterEnable();
    return readValue;
}

float ADC::readVolt()
{
    read();
    voltage = readValue / 4095.0f * 3.3f;
    return voltage;
}

#endif
