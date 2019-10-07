#ifndef VescData_h
#define VescData_h

#include "Arduino.h"

class VescData
{
  public:
    float batteryVoltage;
    bool moving;
    float ampHours;
    float odometer; // in kilometers
};

#endif