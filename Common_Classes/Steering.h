/*
 * Steering.h
 *
 *    Author:Abraham Gassama
 *     Email:uqetv@student.kit.edu
 */

#ifndef STEERING_H_
#define STEERING_H_
#include "GPIO.h"
#include "System.h"
#include "ADC.h"


class Steering
{
public:
    Steering();
    ~Steering();
    void  init(System* sys, uint32_t base , uint32_t sampleSeq, uint32_t analogInput);
    float getValue(void);
    void  calibrateSteering();
    float getUe();

private:
    System *sys;
    float umax ;    // maximale Spannung, Poti ganz rechts
    float umin ;    // minimale Spannung, Poti ganz links
    float ue ;      // Messung
    float center;   // Nulllage
    float steeringValue ;// Auslenkung zwischen -1 und 1
    bool umincal = false; // Kalibrierung von umin
    bool umaxcal = false; // Kalibrierung von umax

    ADC InPoti ;
    GPIO sw1, sw2 ;
};

#endif /* STEERING_H_ */
