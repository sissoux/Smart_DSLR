#ifndef FRAME
#define FRAME

static const unsigned char PROGMEM EmptyBatteryFrame[] =
{ B11111111, B11111100,
  B10000000, B00000100,
  B10000000, B00000111,
  B10000000, B00000111,
  B10000000, B00000111,
  B10000000, B00000100,
  B11111111, B11111100,
};

static const unsigned char PROGMEM BatteryLineFrame[] =
{ B11111111, B11111111,
  B11111111, B11111111,
  B11111111, B11111111,
};


static String Screen[NUMBER_OF_SCREENS][MAX_MENU_LINE] = {
  {
    "Time lapse",
    "",
    "Count: ",
    "Time interval: ",
    "Exposure: ",
    "",
    "",
    "Device:        start",
  },
  {
    "Triggered shoot",
    "",
    "bla: ",
    "bli: ",
    "blou: ",
    "blu: ",
    "",
    "Device:        start",
    "test1",
    "test1",
    "test1"
  },
  {
    "Settings",
    "",
    "Screen intensity: ",
    "Auto shutdown: ",
    "Exposure: ",
    "Duration: ",
    "",
    "Device:        start",
  }
};

static String Unit[NUMBER_OF_SCREENS][MAX_MENU_LINE] = {
  {
    "",
    "",
    "",
    "s",
    "s",
    "",
    "",
    ""
  },
  {
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    ""
  },
  {
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    ""
  },
};

static uint16_t Value[NUMBER_OF_SCREENS][MAX_MENU_LINE] = {
  {
    0,
    0,
    100,
    1,
    60,
    0,
    0,
    0
  },
  {
    0,
    0,
    100,
    3,
    1,
    10,
    0,
    0,
    0
  },
  {
    0,
    0,
    100,
    3,
    1,
    10,
    0,
    0,
    0
  },
};


static uint8_t ScreenNumberOfLine[NUMBER_OF_SCREENS] = {6, 9, 6};

typedef enum {
  TIME_LAPSE,
  TRIGGERED_SHOOT,
  SETTINGS
} screenName;

#endif
