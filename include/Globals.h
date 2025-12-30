#pragma once

#include <TFT_eSPI.h>
#include <bb_captouch.h>
#include <Preferences.h>

#include "Types.h"

// Board display
extern TFT_eSPI tft;

// Display touch
extern BBCapTouch capTouch;
extern TOUCHINFO touchInfo;

// Preferences object for non-volatile memory
extern Preferences prefs;

// Program arrays
extern Button buttons[4];
extern ActionBtn ActionBtns[3];
extern const char* const DOG_NAMES[2];
extern const char* const MEAL_NAMES[2];
extern const char* const BTN_KEYS[8];

// Program colors
extern const uint16_t COL_BG;
extern const uint16_t COL_TEXT;
extern const uint16_t COL_LINE;
extern const uint16_t COL_MORNING_BG;
extern const uint16_t COL_MORNING_TX;
extern const uint16_t COL_EVENING_BG;
extern const uint16_t COL_EVENING_TX;
extern const uint16_t POPUP_BG;

// Timezone and NTP server setup for timestamp calculation
extern const char* DEFAULT_TIMESTAMP;
extern const char* NTP_SERVER;
extern const long GMT_OFFSET_SEC; // PST
extern const int DAYLIGHT_OFFSET_SEC; // +1 hour for PDT
extern bool resetDoneToday;

// Program loop function variables
extern bool wasTouched;
extern bool confirmationShown;
extern int buttonToReset;

// Stack
extern UndoStack stack;

// Stack functions
void push(const Button &btn);
bool pop(Button &btn);