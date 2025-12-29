#include <TFT_eSPI.h>
#include <SPI.h>
#include <Wire.h>
#include <time.h>
#include <bb_captouch.h>
#include <WiFi.h>
#include <Preferences.h>

// Include program header files
#include "include/Config.h"
#include "include/Types.h"
#include "include/Globals.h"
#include "include/UI.h"
#include "include/Storage.h"
#include "include/TimeUtils.h"
#include "include/Touch.h"

// Include WiFi credentials header file
#include "secrets.h"

// Populate button objects with data
void setupButtons() {
  int colWidth  = SCREEN_W / 2;
  int btnHeight = 52;
  int spacing   = 12;
  int yStart    = 102;

  // Dividing line between buttons
  tft.drawLine(colWidth, 62, colWidth, yStart + (btnHeight*2) + spacing, COL_TEXT); // (x0, y0, x1, y1, color)
  
  // Molly buttons: {x, y, w, h, label, background color, text color, time, pressed, keyTime, keyPressed}
  buttons[0] = {20, yStart, colWidth - 40, btnHeight, MEAL_NAMES[0], COL_MORNING_BG, COL_MORNING_TX, DEFAULT_TIMESTAMP, false, BTN_KEYS[0], BTN_KEYS[4]};                      
  buttons[1] = {20, yStart + btnHeight + spacing, colWidth - 40, btnHeight, MEAL_NAMES[1], COL_EVENING_BG, COL_EVENING_TX, DEFAULT_TIMESTAMP, false, BTN_KEYS[1], BTN_KEYS[5]};                       

  // Toby buttons: {x, y, w, h, label, background color, text color, time, pressed, keyTime, keyPressed}
  buttons[2] = {colWidth + 20, yStart, colWidth - 40, btnHeight, MEAL_NAMES[0], COL_MORNING_BG, COL_MORNING_TX, DEFAULT_TIMESTAMP, false, BTN_KEYS[2], BTN_KEYS[6]};                       
  buttons[3] = {colWidth + 20, yStart + btnHeight + spacing, colWidth - 40, btnHeight, MEAL_NAMES[1], COL_EVENING_BG, COL_EVENING_TX, DEFAULT_TIMESTAMP, false, BTN_KEYS[3], BTN_KEYS[7]};                      

  // Populates buttons with timestamps stored in flash. Otherwise leaves values as default.
  for (auto &b : buttons) loadTimeFromFlash(b);
}

void setup() {
  Serial.begin(115200);

  // Initialize display
  tft.init();
  tft.setRotation(1); // Set to landscape (320x240)

  // Call helper functions
  drawHeader();
  drawDogLabels();
  setupButtons();
  for (auto &b : buttons) drawButton(b);

  // Starts I^2C bus for communication with capacitive touch controller
  // Set slower clock to avoid timeout
  Wire.begin(TOUCH_SDA_PIN, TOUCH_SCL_PIN, 100000); // (serial data, serial clock, bus speed)

  // Initialize capacitive touch
  capTouch.init(TOUCH_SDA_PIN, TOUCH_SCL_PIN, TOUCH_RST_PIN, TOUCH_INT_PIN); // (serial data, serial clock, reset pin, interrupt pin)
  capTouch.setOrientation(0, SCREEN_W, SCREEN_H);

  // Connect to WiFi
  WiFi.begin(SSID, PASSWORD);
  Serial.println("\nConnecting");

  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }

  Serial.print("\nConnected: ");
  Serial.println(WiFi.localIP());

  // Config system time with NTP
  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);

  // Draws icons after successful WiFi and time configuration
  drawIcons();
}

void loop() {
  if (capTouch.getSamples(&touchInfo)) { // Detects touch input

    // True if at least one finger touching screen
    bool isTouched = (touchInfo.count > 0);

    // Prevents multiple triggers if holding or rubbing screen
    if (isTouched && !wasTouched) {

      int x = touchInfo.x[0];
      int y = touchInfo.y[0];

      // Line up touch controller system with display rotation
      mapCoordinates(x, y);
      x = constrain(x, 0, SCREEN_W - 1);
      y = constrain(y, 0, SCREEN_H - 1);

      if(confirmationShown) {
        handlePopupTouch(x, y);
      }
      
      else {
        handleButtonTouch(x, y);
      }  

      // Finger currently down
      wasTouched = true;
    }

    // Finger lifted
    if (!isTouched) {
      wasTouched = false;
    }

  } else { // No touch sample, so resets
    wasTouched = false;
  }

  // Resets buttons at midnight
  if (isMidnight() && !resetDoneToday) {
    for (auto &b : buttons) resetTime(b);
    resetDoneToday = true;
  }

  // Set boolean to default when no longer midnight to prevent reset occurring too many times
  if (!isMidnight()) {
    resetDoneToday = false;
  }
}