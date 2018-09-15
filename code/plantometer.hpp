#ifndef plantometer
#define plantometer

#include "Arduino.h"

class Plant
{
public:
  bool tracker(long interval);

private:
  long interval;
  unsigned long previousTime = 0;
};

#endif
