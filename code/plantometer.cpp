#include "Arduino.h"
#include "plantometer.hpp"

bool Plant::tracker(long interval)
{
  if (millis() - previousTime >= interval)
  {
    previousTime = millis();
    return true;
  }
  return false;
}
