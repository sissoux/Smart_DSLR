#include "Button.h"


#define BUTTON_PIN 15
#define CLICK_DEBOUNCE 2000       // In milliseconds
#define DBL_CLICK_THRESHOLD 300   // In milliseconds
#define LONG_CLICK_THRESHOLD 1000 // In milliseconds

Button MyButton(BUTTON_PIN, DBL_CLICK_THRESHOLD, LONG_CLICK_THRESHOLD);
ClickType MyClick = NOCLICK;

void setup()
{
  Serial.begin(115200);
  while (!Serial);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(BUTTON_PIN, myButtonISR, CHANGE);     //Declaration of interrupt in main code is mandatory if we want to use Interrupts.

  Serial.println("Click detection librairy exemple");
}

void loop()
{
  switch (MyButton.clickCheck())    //clickCheck returns a ClickType variable
  {
    case SINGLE_CLICK : Serial.println("Single Click detected"); break;
    case DOUBLE_CLICK : Serial.println("Double Click detected"); break;
    case LONG_CLICK : Serial.println("Long Click detected"); break;
    case NOCLICK : break;
  }
  delay(100);
}


void myButtonISR()
{
  if (!digitalRead(MyButton._PIN))
  {
    if (MyButton.LastPushISRTimer > CLICK_DEBOUNCE)
    {
      MyButton.Pushed = 1;
      MyButton.LastValidPushTimer = 0;
    }
    MyButton.LastPushISRTimer = 0;
  }
  else
  {
    if ( MyButton.LastReleaseISRTimer > CLICK_DEBOUNCE)
    {
      MyButton.Released = 1;
    }
    MyButton.LastReleaseISRTimer = 0;
  }
}








