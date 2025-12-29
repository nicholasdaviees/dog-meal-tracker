#include "TimeUtils.h"

#include <time.h>

#include "Globals.h"
#include "UI.h"
#include "Storage.h"

// Calculates current time
String getCurrentTime() {
  struct tm timeinfo;

  if (getLocalTime(&timeinfo)) {
    char buffer[10];
    strftime(buffer, sizeof(buffer), "%l:%M %p", &timeinfo);
    return String(buffer);
  }

  return DEFAULT_TIMESTAMP;
}

// Updates button time to current time
void updateTime(Button &btn) {
  btn.time = getCurrentTime();
  btn.pressed = true;
  saveTimeToFlash(btn);
  drawButton(btn);
}

// Resets button time to default value
void resetTime(Button &btn) {
  btn.time = DEFAULT_TIMESTAMP;
  btn.pressed = false;
  saveTimeToFlash(btn);
  drawButton(btn);
}

// Helper function for resetting time at midnight
bool isMidnight() {
  struct tm timeinfo;

  if (getLocalTime(&timeinfo)) {
    return (timeinfo.tm_hour == 0 && timeinfo.tm_min == 0);
  }

  return false;
}