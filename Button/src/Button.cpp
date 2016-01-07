#include "Button.h"
#include "Arduino.h"



Button::Button(int buttonPin, int _DoubleClickThreshold, int _LongClickThreshold)
{
  this->_PIN = buttonPin;
  this->_DoubleClickThreshold = _DoubleClickThreshold;
  this->_LongClickThreshold = _LongClickThreshold;
}


ClickType Button::clickCheck()
{
  ClickType CLICK = NOCLICK;

  if (Released)                                             // If we detected a rising edge, button has been released. If we still have the Pushed Value then it's a click identification.
  { // If it's the first (ClickCounter = 0) then we just set ClickCounter and wait _DoubleClickThreshold if another click happens.
    if (Pushed)                                             // If ClickCounter is already set and second click happened in less than _DoubleClickThreshold then it's a double click.
    {
      if (ClickCounter && LastValidReleaseTimer < _DoubleClickThreshold)
      {
        CLICK = DOUBLE_CLICK;
        ClickCounter = 0;
      }
      else if (!ClickCounter)
      {
        ClickCounter = 1;
        LastValidReleaseTimer = 0;
      }
      Pushed = 0;
      Released = 0;
    }
    else                                                          // Release = 1 push = 0 should only happen in case of a long click which is already handled. We just ignore an reset.
    {
      Released = 0;
    }
  }
  else if (Pushed && LastValidPushTimer >= _LongClickThreshold)  // If button is not release, we have only the push event and time since this event is above _LongClickThreshold then it's a long click
  {
    CLICK = LONG_CLICK;
    Pushed = 0;
  }
  else if (ClickCounter && LastValidReleaseTimer >= _DoubleClickThreshold)  //If there is not a second push/release in the _DoubleClickThreshold, we just validate the first click as a single click.
  {
    CLICK = SINGLE_CLICK;
    ClickCounter = 0;
  }
  return CLICK;
}


