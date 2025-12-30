#include "include/Storage.h"

#include <Preferences.h>

#include "include/Globals.h"

// Loads timestamp stored in flash into buttonTimes array
// Default value is "--:--" if timestamp not set
void loadTimeFromFlash(Button &btn) {
  btn.time = prefs.getString(btn.keyTime, DEFAULT_TIMESTAMP); // getString(key, default value if not set)
  btn.pressed = prefs.getBool(btn.keyPressed, false);
}

void saveTimeToFlash(Button &btn) {
  prefs.putString(btn.keyTime, btn.time);
  prefs.putBool(btn.keyPressed, btn.pressed);
}