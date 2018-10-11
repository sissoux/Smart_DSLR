/**

   \file Smart_DSLR.ino
   \brief Main firmware for Smart DSLR Remote
   \version 0.1
   \date 2/10/2016

*/


#include "define.h"
#include "config.h"
#include <Button.h>
#include <Encoder.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "frames.h"
#include <ADC.h>

Encoder myEnc(ENC_A_PIN, ENC_B_PIN);

long oldPosition  = 0;
int UpDown = 0;
int16_t Step = 0;
Button EncoderButton(ENC_E_PIN, DBL_CLICK_THRESHOLD, LONG_CLICK_THRESHOLD);   //Instantiate a button object to detect clicks on encoder.
ClickType EncoderClick = NOCLICK;                                             //ClickType variable, for click identification result

Adafruit_SSD1306 display(OLED_RESET);
int8_t ActiveLine = 0;                                                        //Store the current highlighted line
uint8_t LineOffset = 0;                                                       //Store the Screen offset, when number of menu is bigger than screen lines ==> for scrolling
screenName ActiveScreen = TIME_LAPSE;                                         //Store the active screen

typedef enum {
  INIT,
  DONE,
  CHARGING,
  ERROR
} chargeState;

chargeState ChargerStatus = INIT;
uint16_t BatteryLevel = 0;
uint16_t TriggerThreshold = 0;
elapsedMillis BatteryCheckTimer = 0;
elapsedMillis DisplayRefreshTimer = 0;

elapsedMillis BlinkTimer = 0;
uint8_t Blink = 0;
uint8_t BatBlinkCounter = 0;


ADC *adc = new ADC(); // adc object;

typedef enum {
  UP,
  DOWN,
  RIGHT,
  LEFT,
  STBY
} navigator;

navigator MyNavigator = STBY;
uint8_t ItemSelected = 0;

void setup()
{
  Serial.begin(115200);
  init_IO();
  init_ISR();
  init_Display();
  LED_Sequence();
  pinMode(BAT_VOLTAGE_PIN, INPUT);
  //adc->setReference(ADC_REF_1V2, ADC_0); // change all 3.3 to 1.2 if you change the reference to 1V2

  adc->setAveraging(4); // set number of averages
  adc->setResolution(12); // set bits of resolution

  // it can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED_16BITS, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED
  // see the documentation for more information
  //adc->setConversionSpeed(ADC_LOW_SPEED); // change the conversion speed
  // it can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED
  //adc->setSamplingSpeed(ADC_LOW_SPEED); // change the sampling speed
}

void loop()
{
  navigation();
  timersManagement();

  //digitalWrite(D_Status_PIN, HIGH);
  //digitalWrite(D_Trig_PIN, HIGH);
}

void timersManagement()
{
  //Blink rate "thread", change Blink state eache time BlinkTimer reached BLINK_RATE
  if (BlinkTimer >= BLINK_RATE)
  {
    BlinkTimer = 0;
    Blink = !Blink;
    BatBlinkCounter = (BatBlinkCounter + 1) % 12;
  }

  //Launch Battery state update each time BatteryCheckTimer reaches BATTERY_CHECK_RATE
  if (BatteryCheckTimer >= BATTERY_CHECK_RATE)
  {
    BatteryCheckTimer = 0;
    chargeCheck(&ChargerStatus, &BatteryLevel);
  }

  //
  if (DisplayRefreshTimer >= DISPLAY_REFRESH_RATE)
  {
    DisplayRefreshTimer = 0;
    displayManagement();
  }
}

void navigation()
{
  switch (EncoderButton.clickCheck())    //clickCheck returns a ClickType variable
  {
    case SINGLE_CLICK :
      //Serial.println("Single Click detected");
      //audioCapture(4000, 1);
      if (ActiveLine == 0)
      {
        ActiveScreen = (screenName)((ActiveScreen + 1) % NUMBER_OF_SCREENS);    //For debug
        ActiveLine = 0;
      }
      else
      {
        if (!ItemSelected)
        {
          if (ActiveLine == 6)
            startTimeLapse();
          ItemSelected = ActiveLine + 1;
        }
        else
        {
          ItemSelected = 0;
        }
      }
      break;
    case DOUBLE_CLICK : Serial.println("Double Click detected"); break;
    case LONG_CLICK : Serial.println("Long Click detected"); break;
    case NOCLICK : break;
  }

  getEncoderStep();
  if (ItemSelected)
  {
    Value[ActiveScreen][ItemSelected] += Step;
    Step = 0;
  }
  else
  {
    if (Step > 0)
    {
      ActiveLine += Step;
      Step = 0;
    }
    else if (Step < 0)
    {
      ActiveLine += Step;
      Step = 0;
    }

    if (ActiveLine > ScreenNumberOfLine[ActiveScreen])
    {
      ActiveLine = ScreenNumberOfLine[ActiveScreen];
    }
    else if (ActiveLine < 0)
    {
      ActiveLine = 0;
    }
    if (ActiveLine >= 6 )
    {
      LineOffset = ActiveLine - 6;
    }
    else
    {
      LineOffset = 0;
    }

  }
}

void startTimeLapse()
{
  elapsedMillis LocalTimer = 0;
  uint16_t counter = 1;

  while (counter < Value[ActiveScreen][2])
  {
    digitalWrite(DSLR_FOCUS_PIN, HIGH);
    digitalWrite(DSLR_SHUTTER_PIN, HIGH);
    digitalWrite(10, HIGH);
    digitalWrite(11, LOW);
    while (LocalTimer < Value[ActiveScreen][4] * 1000)
    {
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);

      display.println("INTERVAL SHOOTING ");
      display.println(" ");

      display.print("Shoot ");
      display.print(counter);
      display.print(" on ");
      display.println(Value[ActiveScreen][2]);
      display.print("Time ");
      display.print(LocalTimer / 1000);
      display.print(" on ");
      display.println(Value[ActiveScreen][4]);
      display.display();
    }
    LocalTimer = 0;
    digitalWrite(DSLR_FOCUS_PIN, LOW);
    digitalWrite(DSLR_SHUTTER_PIN, LOW);
    digitalWrite(10, LOW);
    digitalWrite(11, HIGH);
    while (LocalTimer < Value[ActiveScreen][3] * 1000)
    {
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);

      display.println("INTERVAL SHOOTING ");
      display.println(" ");

      display.print("Waiting ");
      display.display();
    }
    LocalTimer = 0;
    counter++;
  }
}

/**
   \fn void displayManagement()
   \brief This function organizes all OLED screen update.
   Many things to be updated here
*/
void displayManagement()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);


  for (int line = 0 + LineOffset ; line < 8 + LineOffset; line++)
  {
    display.print(Screen[ActiveScreen][line]);
    if (line == ActiveLine + 1)
    {
      display.write(26);
    }
    else
    {
      display.print(" ");
    }
    if ( Value[ActiveScreen][line] != 0)display.print(Value[ActiveScreen][line]);
    display.println(Unit[ActiveScreen][line]);
  }


  batteryLevelDisplay(ChargerStatus, BatteryLevel, Blink);
  //display.drawFastHLine(56, 8*3-1, 7, BatBlink);

  //display.println(BatteryLevel);
  //display.println(TO_VOLTS(BatteryLevel));
  display.display();
}

/**
   \fn void batteryLevelDisplay()
   \brief Define the battery logo state in function of charger state.
   Remaining or current charging percentage is displayed in 12 different states
   with a linear interpolation of battery voltage between VBat Min (3.2V) and
   Vbat Nominal (3.7V) if not charging. VBat Min (3.2V) and Charged Vbat (4.2V
   if charging.
*/
void batteryLevelDisplay(chargeState Status, uint16_t Vbat, uint8_t BatBlink)
{
  if (Vbat <= LOW_BATTERY_V_THRESH)
  {
    display.drawBitmap(BAT_LOGO_POS_X, BAT_LOGO_POS_Y,  EmptyBatteryFrame, 16, 7, BatBlink);
  }
  else if (Vbat >= HIGH_BATTERY_V_THRESH)
  {
    display.drawBitmap(BAT_LOGO_POS_X, BAT_LOGO_POS_Y,  EmptyBatteryFrame, 16, 7, BatBlink);
    for (int j = 0; j < 12; j++)
    {
      display.drawBitmap(BAT_LOGO_POS_X + 1 + j, BAT_LOGO_POS_Y + 1,  BatteryLineFrame, 1, 5, BatBlink);
    }
  }
  else
  {
    display.drawBitmap(BAT_LOGO_POS_X, BAT_LOGO_POS_Y,  EmptyBatteryFrame, 16, 7, 1);
    if (Status != CHARGING)
    {
      int16_t VbatPercent = (((int32_t)(Vbat) - LOW_BATTERY_V_THRESH) * 100) / (NOMINAL_BATTERY_V - LOW_BATTERY_V_THRESH);
      VbatPercent = constrain(VbatPercent * 12 / 100, 0, 12);
      for (int j = 0; j < VbatPercent; j++)
      {
        display.drawBitmap(BAT_LOGO_POS_X + 1 + j, BAT_LOGO_POS_Y + 1,  BatteryLineFrame, 1, 5, 1);
      }
    }
    else
    {
      int16_t VbatPercent = (((int32_t)(Vbat) - LOW_BATTERY_V_THRESH) * 100) / (CHARGED_BATTERY_V - LOW_BATTERY_V_THRESH);
      VbatPercent = constrain(VbatPercent * 12 / 100, 0, 12);
      if (BatBlinkCounter > VbatPercent)
      {
        BatBlinkCounter = 0;
      }
      for (int j = 0; j < BatBlinkCounter; j++)
      {
        display.drawBitmap(BAT_LOGO_POS_X + 1 + j, BAT_LOGO_POS_Y + 1,  BatteryLineFrame, 1, 5, 1);
      }
    }
  }
}

/**
   \fn void chargeCheck(chargeState * Status, uint16_t * Vbat)
   \brief Measure Battery Voltage and Charging state

   \param Status pointer to charge State program variable
   \param Vbat pointer to Global Battery voltage
*/
void chargeCheck(chargeState *Status, uint16_t *Vbat)
{

  *Vbat = adc->analogRead(BAT_VOLTAGE_PIN);
  Serial.print("VBAT = ");
  Serial.print(*Vbat);
  Serial.print(" = ");
  Serial.print(TO_VOLTS(*Vbat));
  Serial.println("V");
  *Status =  (chargeState)(digitalRead(BAT_STAT1_PIN) + digitalRead(BAT_STAT2_PIN) * 2);
}

/**
   \fn void getEncoderStep()
   \brief Update Encoder position, store the result in global variable Step each 4 edges detected
*/
void getEncoderStep()
{
  long newPosition = myEnc.read();
  if (newPosition != 0)
  {
    while ( newPosition >= 4)
    {
      Step--;
      newPosition = newPosition - 4;
    }

    while ( newPosition <= -4)
    {
      Step++;
      newPosition = newPosition + 4;
    }
    myEnc.write(newPosition);
  }
}

/**
   \fn void LED_Sequence()
   \brief Startup LED Pattern

*/
void LED_Sequence()
{
#ifdef STARTUP_SEQ
  digitalWrite(D_Status_PIN, HIGH);
  digitalWrite(D_Trig_PIN, LOW);
  delay(200);
  digitalWrite(D_Status_PIN, LOW);
  digitalWrite(D_Trig_PIN, HIGH);
  delay(200);
  digitalWrite(D_Status_PIN, LOW);
  digitalWrite(D_Trig_PIN, LOW);
  delay(500);
  digitalWrite(D_Status_PIN, HIGH);
  digitalWrite(D_Trig_PIN, HIGH);
  delay(200);
  digitalWrite(D_Status_PIN, LOW);
  digitalWrite(D_Trig_PIN, LOW);
  delay(200);
  digitalWrite(D_Status_PIN, HIGH);
  digitalWrite(D_Trig_PIN, HIGH);
  delay(200);
  digitalWrite(D_Status_PIN, LOW);
  digitalWrite(D_Trig_PIN, LOW);
  delay(200);
#endif
}

/**
   \fn void init_IO()
   \brief Initialization function for all Input/Output using names defined in define.h


*/
void init_IO()
{
  pinMode(FLASH_2_PIN, OUTPUT);
  digitalWrite(FLASH_2_PIN, LOW);

  pinMode(FLASH_1_PIN, OUTPUT);
  digitalWrite(FLASH_1_PIN, LOW);

  pinMode(DSLR_FOCUS_PIN, OUTPUT);
  digitalWrite(DSLR_FOCUS_PIN, LOW);

  pinMode(D_Trig_PIN, OUTPUT);
  digitalWrite(D_Trig_PIN, LOW);

  pinMode(D_Status_PIN, OUTPUT);
  digitalWrite(D_Status_PIN, LOW);

  pinMode(DSLR_SHUTTER_PIN, OUTPUT);
  digitalWrite(DSLR_SHUTTER_PIN, LOW);

  pinMode(MIC_EN_PIN, OUTPUT);
  digitalWrite(MIC_EN_PIN, HIGH);

  pinMode(LIGHT_EN_PIN, OUTPUT);
  digitalWrite(LIGHT_EN_PIN, HIGH);

  //pinMode(ENC_A_PIN, INPUT);    //Need to HW Activate ENC input
  //pinMode(ENC_B_PIN, INPUT);    //Need to HW Activate ENC input
  pinMode(ENC_E_PIN, INPUT);


  pinMode(MIC_IN_PIN, INPUT);
  pinMode(LIGHT_IN_PIN, INPUT);
  pinMode(BAT_STAT1_PIN, INPUT);
  pinMode(BAT_STAT2_PIN, INPUT);
  pinMode(BAT_VOLTAGE_PIN, INPUT);
  digitalWrite(BAT_VOLTAGE_PIN, LOW);

}


/**
  \fn void init_ISR()
  \brief Set interruption vectors at startup


*/
void init_ISR()
{
  attachInterrupt(ENC_E_PIN, encoderButtonISR, CHANGE);     //Declaration of interrupt in main code is mandatory if we want to use Interrupts.
}


/**
  \fn void ButtonISR()
  \brief Function declared in main file, used to rise flag when button click happen.


*/
void encoderButtonISR()
{
  if (!digitalRead(EncoderButton._PIN))
  {
    if (EncoderButton.LastPushISRTimer > CLICK_DEBOUNCE)
    {
      EncoderButton.Pushed = 1;
      EncoderButton.LastValidPushTimer = 0;
    }
    EncoderButton.LastPushISRTimer = 0;
  }
  else
  {
    if ( EncoderButton.LastReleaseISRTimer > CLICK_DEBOUNCE)
    {
      EncoderButton.Released = 1;
    }
    EncoderButton.LastReleaseISRTimer = 0;
  }
}



void init_Display()
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.clearDisplay();
  display.drawBitmap(0, 0,  SmartDSLRRemote, 128, 64, 1);
  display.display();
  delay(800);
  display.clearDisplay();


}


//For debug
void audioCapture(uint16_t Duration, uint16_t SampleTime)
{
  digitalWrite(MIC_EN_PIN, LOW);
  delay(2000);
  uint16_t EncValue = 127;
  uint16_t Hyst = 0;
#define HYSTHERESIS 5
  while (true)
  {
    //getEncoderStep();
    EncValue += Step;
    Step = 0;
    if (EncValue > 511)
    {
      EncValue = 511;
    }
    else if (EncValue <= 1)
    {
      EncValue = 1;
    }
    if (abs(analogRead(MIC_IN_PIN) - 512) > EncValue - Hyst)
    {
      digitalWrite(D_Trig_PIN, HIGH);
      digitalWrite(FLASH_1_PIN, HIGH);
      delay(50);
      digitalWrite(D_Trig_PIN, LOW);
      digitalWrite(FLASH_1_PIN, LOW);
      Hyst = HYSTHERESIS;
    }
    else
    {
      digitalWrite(D_Trig_PIN, LOW);
      Hyst = 0;
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);

    display.print("Threshold: ");
    display.print(EncValue);
    display.display();
  }
}


