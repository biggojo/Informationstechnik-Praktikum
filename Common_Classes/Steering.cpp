/*
 * Steering.cpp
 *
 *    Author:
 *     Email:
 */

#include "Steering.h"


Steering::Steering()
{
    /*
     * Default empty constructor
     */
}

Steering::~Steering()
{
    /*
     * Default empty destructor
     */
}

void Steering::init(System *sys, uint32_t base, uint32_t sampleSeq, uint32_t analogInput)
{
    this->sys = sys;

    // Input Poti on pin PE2
    InPoti.init(sys, base , sampleSeq, analogInput);

    // Switch SW1 on pin PF4
    sw1.init(sys, GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_DIR_MODE_IN);

    // Switch SW2 on pin PF0
    sw2.init(sys, GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_DIR_MODE_IN);


}

float Steering::getValue(void)
{
    //Aktuelle Spannung messen
    ue = InPoti.readVolt();

    //Berechnung der mittleren Spannung
    center = (umax + umin) / 2.0f;

    //Berechnung des Lenkeinschlags zw. [-1,1]
    steeringValue = (ue - center) / (center - umin);

    //Wenn steering Value außerhalb  [-1,1] dann korrektur
    if (steeringValue > 1.0f)
    {
        steeringValue = 1.0f ;
    }
    if (steeringValue < - 1.0f)
    {
        steeringValue = - 1.0f ;
    }

    return steeringValue ;

}


void Steering::calibrateSteering()
{

    sw1.setPullup(true);
    sw2.setPullup(true);


    //Schleife wartet bis umin und umax festgelegt sind
    while(umincal == false || umaxcal==false)
    {
        //Wenn sw1 gedrückt wird wird umin auf gemessene Spannung gesetzt
        if (!sw1.read())
        {
        sys->delayUS(50000);
        while (!sw1.read());

            umin = InPoti.readVolt();

            umincal = true;
        }

        //Wenn sw2 gedrückt wird wird umax auf gemessene Spannung gesetzt
        if (!sw2.read())
        {
        sys->delayUS(50000);
        while (!sw2.read());

        umax = InPoti.readVolt();

        umaxcal = true;

        }

    }

}

//getMethode um Spannung in Segwayklasse plotten zu können
float Steering::getUe()
{
    return ue;
}


