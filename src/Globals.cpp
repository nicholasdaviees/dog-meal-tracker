#include <TFT_eSPI.h>
#include <SPI.h>
#include <Wire.h>
#include <time.h>
#include <bb_captouch.h>
#include <WiFi.h>
#include <Preferences.h>

#include "include/Globals.h"
#include "include/Config.h"

// Initialize display
TFT_eSPI tft = TFT_eSPI();

// Initialize touch
BBCapTouch capTouch;
TOUCHINFO touchInfo;

// Initialize preferences object for non-volatile memory
Preferences prefs;

// Define arrays
Button buttons[4];
ActionBtn ActionBtns[3];
const char* const DOG_NAMES[2] = {"Molly", "Toby"};
const char* const MEAL_NAMES[2] = {"Fed Morning", "Fed Evening"};
const char* const BTN_KEYS[8] = {"btn1_time", "btn2_time", "btn3_time", "btn4_time", "btn1_pressed", "btn2_pressed", "btn3_pressed", "btn4_pressed"};
MealSwitch *homeKitSwitches[4] = {nullptr, nullptr, nullptr, nullptr};

// Define colors
const uint16_t COL_BG         = tft.color565(255, 248, 231);  // Cream background
const uint16_t COL_TEXT       = TFT_BROWN;                    // Title Texts
const uint16_t COL_LINE       = TFT_BROWN;                    // Title Texts
const uint16_t COL_MORNING_BG = tft.color565(191, 211, 217);  // Button background
const uint16_t COL_MORNING_TX = TFT_NAVY;                     // Text background
const uint16_t COL_EVENING_BG = tft.color565(245, 195, 143);  // Button background
const uint16_t COL_EVENING_TX = TFT_MAROON;                   // Text background
const uint16_t POPUP_BG       = tft.color565(255, 255, 255);  // Popup background

// Define timezone and NTP server setup for timestamp calculation
const char* DEFAULT_TIMESTAMP = "--:--";
const char* NTP_SERVER = "pool.ntp.org";
const long GMT_OFFSET_SEC = -8 * 3600; // PST
const int DAYLIGHT_OFFSET_SEC = 3600; // +1 hour for PDT
bool resetDoneToday = false;

// Define loop function variables
bool wasTouched = false;
bool confirmationShown = false;
int buttonToReset = -1;

// Stack
UndoStack stack;

// Define stack functions
void push(const Button &btn){
	if (stack.size < STACK_SIZE) {
	int index = (stack.head + stack.size) % STACK_SIZE;
    stack.buf[index] = btn;
    stack.size++;
  } 

  // Stack full. Overwrites the oldest and wraps-around by advancing head
  else {
    stack.buf[stack.head] = btn;
    stack.head = (stack.head + 1) % STACK_SIZE;
  }
}

bool pop(Button &btn){
	if (stack.size == 0) return false; // Stack empty

  	int topIndex = (stack.head + stack.size - 1) % STACK_SIZE;
  	btn = stack.buf[topIndex];
  	stack.size--;
  	return true;
}

// Helper function for Homekit integration
int getButtonIndex(const Button &btn) {
  for (int index = 0; index < 4; index++) {
    if (&buttons[index] == &btn) return index; // Same object in memory
  }
  return -1;
}