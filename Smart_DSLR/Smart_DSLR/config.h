
#ifndef CONFIG
#define CONFIG
#include "define.h"

 /**
 * Battery management configuration
 */
#define HIGH_BATTERY_V_THRESH 795     // 4.25V
#define LOW_BATTERY_V_THRESH 599      // 3.2V
#define NOMINAL_BATTERY_V 692         // 3.2V
#define CHARGED_BATTERY_V 785         // 3.2V
#define VALUE_AT_FIVE_V 935.0
#define TO_VOLTS(a) (float)(a*5)/VALUE_AT_FIVE_V
#define BATTERY_CHECK_RATE 10000

/**
 * Click management configuration
 */
#define CLICK_DEBOUNCE 2000           // In milliseconds
#define DBL_CLICK_THRESHOLD 300       // In milliseconds
#define LONG_CLICK_THRESHOLD 1000     // In milliseconds


/**
 * Display configuration
 */
#define DISPLAY_REFRESH_RATE 25       // In milliseconds
#define OLED_RESET 4
#define BAT_LOGO_POS_X 111
#define BAT_LOGO_POS_Y 0              
//#define STARTUP_SEQ
#define MAX_MENU_LINE 15              // Maximum allowed lines on a single screen, for memory size limitation
#define NUMBER_OF_SCREENS 3           // Number of different screens to be displayed (Modes + Setting screens)
#define BLINK_RATE 500                //in ms


/**
 * Analog comparator configuration
 */
//#define USE_INTERNAL_CMP_LIGHT
//#define USE_INTERNAL_CMP_SOUND

#endif
