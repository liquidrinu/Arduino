#include "Arduino.h"
#include "plantometer.hpp"
#include <LiquidCrystal_I2C.h>

bool Plant::tracker(long interval)
{
  if (millis() - previousTime >= interval)
  {
    previousTime = millis();
    return true;
  }
  return false;
}
