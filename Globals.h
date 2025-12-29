#pragma once

#include <TFT_eSPI.h>
#include <bb_captouch.h>
#include <Preferences.h>

#include "Types.h"

// Initialize display
extern TFT_eSPI tft;

// Initialize touch
extern BBCapTouch capTouch;
extern TOUCHINFO touchInfo;

// Initialize preferences object for non-volatile memory
extern Preferences prefs;

// Define arrays
extern Button buttons[4];
extern ActionBtn ActionBtns[2];
extern const char* const DOG_NAMES[2];
extern const char* const MEAL_NAMES[2];
extern const char* const BTN_KEYS[8];

// Define colors
extern const uint16_t COL_BG;
extern const uint16_t COL_TEXT;
extern const uint16_t COL_LINE;
extern const uint16_t COL_MORNING_BG;
extern const uint16_t COL_MORNING_TX;
extern const uint16_t COL_EVENING_BG;
extern const uint16_t COL_EVENING_TX;
extern const uint16_t POPUP_BG;

// Define timezone and NTP server setup for timestamp calculation
extern const char* DEFAULT_TIMESTAMP;
extern const char* NTP_SERVER;
extern const long GMT_OFFSET_SEC; // PST
extern const int DAYLIGHT_OFFSET_SEC; // +1 hour for PDT
extern bool resetDoneToday;

// Define loop function variables
extern bool wasTouched;
extern bool confirmationShown;
extern int buttonToReset;