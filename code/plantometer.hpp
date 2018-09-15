#ifndef plantometer
#define plantometer

#include "Arduino.h"
#include <LiquidCrystal_I2C.h>

class Plant
{
public:
  bool tracker(long interval);

private:
  long interval;
  unsigned long previousTime = 0;
};

#endif
