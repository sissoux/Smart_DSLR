#ifndef BUTTON
#define BUTTON

#include "Arduino.h"

typedef enum {
  SINGLE_CLICK,
  DOUBLE_CLICK,
  LONG_CLICK,
  NOCLICK
} ClickType;

class Button
{
  private:
    int _DoubleClickThreshold;
    int _LongClickThreshold;
    uint8_t ClickCounter = 0;

  public:
    boolean Pushed = 0;
    boolean Released = 0;
    int _PIN;
    elapsedMicros LastPushISRTimer = 0;
    elapsedMicros LastReleaseISRTimer = 0;
    elapsedMillis LastValidPushTimer = 0;
    elapsedMillis LastValidReleaseTimer = 0;

    Button(int buttonPin, int dblClickThreshold, int longClickThreshold);
    ClickType clickCheck();
};

#endif
