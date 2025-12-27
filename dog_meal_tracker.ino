#include <TFT_eSPI.h>
#include <SPI.h>
#include <Wire.h>
#include <time.h>
#include <bb_captouch.h>
#include <WiFi.h>
#include <Preferences.h>

// Include dog icon header files
#include "images/pawPadIcon.h"
#include "images/mollyIcon.h"
#include "images/tobyIcon.h"
#include "images/checkmarkMorningIcon.h"
#include "images/checkmarkEveningIcon.h"
#include "images/xMorningIcon.h"
#include "images/xEveningIcon.h"

// Include WiFi credentials header file
#include "secrets.h"

// Capacitive touch pins
#define TOUCH_SDA_PIN 33
#define TOUCH_SCL_PIN 32
#define TOUCH_RST_PIN 25
#define TOUCH_INT_PIN 21

// Initialize display
TFT_eSPI tft = TFT_eSPI();

// Initialize touch
BBCapTouch capTouch;
TOUCHINFO touchInfo;

// Initialize preferences object for non-volatile memory
Preferences prefs;

// Define screen layout constants
#define SCREEN_W 320
#define SCREEN_H 240

// Buttons
struct Button {
  int x, y, w, h;
  const char* label;
  uint16_t color;
  uint16_t textColor;
  String time;
  bool pressed;
  const char* keyTime;
  const char* keyPressed;
};

// Define arrays
Button buttons[4];
const char* const DOG_NAMES[2]  = {"Molly", "Toby"};
const char* const MEAL_NAMES[2] = {"Fed Morning", "Fed Evening"};
const char* const BTN_KEYS[8]   = {"btn1_time", "btn2_time", "btn3_time", "btn4_time", "btn1_pressed", "btn2_pressed", "btn3_pressed", "btn4_pressed"};

// Define colors
const uint16_t COL_BG         = tft.color565(255, 248, 231);  // Cream background
const uint16_t COL_TEXT       = TFT_BROWN;                    // Title Texts
const uint16_t COL_LINE       = TFT_BROWN;                    // Title Texts
const uint16_t COL_MORNING_BG = tft.color565(191, 211, 217);  // Button background
const uint16_t COL_MORNING_TX = TFT_NAVY;                     // Text background
const uint16_t COL_EVENING_BG = tft.color565(245, 195, 143);  // Button background
const uint16_t COL_EVENING_TX = TFT_MAROON;                   // Text background

// Define timezone and NTP server setup for timestamp calculation
const char* DEFAULT_TIMESTAMP = "--:--";
const char* NTP_SERVER = "pool.ntp.org";
const long GMT_OFFSET_SEC = -8 * 3600; // PST
const int DAYLIGHT_OFFSET_SEC = 3600; // +1 hour for PDT
bool resetDoneToday = false;

// ******************** Begin Helper Functions ********************
// -------------------- Begin Drawing Functions --------------------
// Sets background color and draws title text
void drawHeader() {
  tft.fillScreen(COL_BG);
  tft.setTextColor(COL_TEXT, COL_BG);                     
  tft.drawString("Dog Meal Tracker", 35, 8, 4); // (text, x, y, fontSize)
  tft.drawLine(20, 42, SCREEN_W - 20, 42, COL_LINE); // (x0, y0, x1, y1, color)
}

// Draws dog labels
void drawDogLabels() {
  tft.setTextDatum(TC_DATUM);
  tft.setTextColor(COL_TEXT, COL_BG);
  tft.drawString(DOG_NAMES[0], SCREEN_W / 3, 62, 4);        // (text, x, y, fontSize)
  tft.drawString(DOG_NAMES[1], SCREEN_W * 3.35 / 4, 62, 4); // (text, x, y, fontSize)
}

// Draws meal buttons
void drawButton(Button &btn) {
  const int iconW = 20;
  const int iconH = 20;
  const int spacing = 2;

  int textW = tft.textWidth(btn.time, 2);
  int groupW = iconW + spacing + textW;

  int centerX = btn.x + btn.w / 2;
  int centerY = btn.y + btn.h / 2 + 10;

  int iconX = centerX - groupW / 2;
  int textX = iconX + iconW + spacing;

  tft.fillRoundRect(btn.x, btn.y, btn.w, btn.h, 8, btn.color);
  tft.setTextColor(btn.textColor, btn.color);                              
  tft.setTextDatum(MC_DATUM);                                               
  tft.drawString(btn.label, btn.x + btn.w / 2, btn.y + btn.h / 2 - 10, 2); // Morning or evening label
  tft.drawString(btn.time, textX + textW / 2, centerY, 2); // Time fed
  tft.setSwapBytes(true);

  // Place icons with morning button background color
  if(btn.label == MEAL_NAMES[0]) {
    if(btn.pressed){
      tft.pushImage(iconX, centerY - iconH / 2, iconW, iconH, checkmarkMorningIcon);
    }
    else{
      tft.pushImage(iconX, centerY - iconH / 2, iconW, iconH, xMorningIcon);
    }
  }

  // Place icons with evening button background color
  else{
    if(btn.pressed){
      tft.pushImage(iconX, centerY - iconH / 2, iconW, iconH, checkmarkEveningIcon);
    }
    else{
      tft.pushImage(iconX, centerY - iconH / 2, iconW, iconH, xEveningIcon);
    }
  }
}

// Draws meal tracker icons
void drawIcons() {
  tft.setSwapBytes(true);
  tft.pushImage(245, 1, 40, 40, pawPadIcon); // (x, y, w, h, iconName)
  tft.pushImage(20, 48, 48, 48, mollyIcon);  // (x, y, w, h, iconName)
  tft.pushImage((SCREEN_W / 2) + 20, 50, 48, 48, tobyIcon); // (x, y, w, h, iconName)
}
// -------------------- End Drawing Functions --------------------

// -------------------- Begin Time Functions --------------------
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

// Loads timestamp stored in flash into buttonTimes array. Default value is "--:--" if timestamp not set
void loadTimeFromFlash(Button &btn) {
  prefs.begin("btnData", false);
  btn.time = prefs.getString(btn.keyTime, DEFAULT_TIMESTAMP); // getString(key, default value if not set)
  btn.pressed = prefs.getBool(btn.keyPressed, false);
  prefs.end();
}

// Saves button timestamp to flash
void saveTimeToFlash(Button &btn) {
  prefs.begin("btnData", false);
  prefs.putString(btn.keyTime, btn.time);
  prefs.putBool(btn.keyPressed, btn.pressed);
  prefs.end();
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
// -------------------- End Time Functions --------------------

// -------------------- Begin Button Setup Function --------------------
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
// -------------------- End Button Setup Function --------------------

// -------------------- Begin Button Calibration Functions --------------------
// Function to ensure touch press was inside button
bool inButton(int x, int y, const Button &btn) {
  // Left = btn.x
  // Right = btn.x + btn.w
  // Top = btn.y
  // Bottom = btn.y + btn.h
  return (x >= btn.x && x <= btn.x + btn.w && y >= btn.y && y <= btn.y + btn.h);
}

// Function to calibrate touch input
static inline void mapCoordinates(int &x, int &y) {
  int new_x = y; // Swap x and y because touch axes are rotated
  int new_y = SCREEN_H - 1 - x; // Invert y axis

  // Update x and y with new values
  x = new_x; 
  y = new_y;
}
// -------------------- End Button Calibration Functions --------------------
// ******************** End Helper Functions ********************

// ******************** Setup ********************
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
  capTouch.setOrientation(0, SCREEN_W, SCREEN_H); // Set to no rotation. This will be changed later

  // Connect to WiFi
  WiFi.begin(SSID, PASSWORD);
  Serial.println("\nConnecting");

  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }

  // Prints out IP address in serial monitor if connection successful
  Serial.print("\nConnected: ");
  Serial.println(WiFi.localIP());

  // Config system time with NTP
  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);

  // Draws icons after board is connected to WiFi and time is configured
  drawIcons();
}

// ******************** Loop ********************
void loop() {
  if (capTouch.getSamples(&touchInfo)) { // Detects touch input
    static unsigned long lastTap = 0;
    if (millis() - lastTap > 120) { // Comparison to prevent multiple presses
      for (int numTouch = 0; numTouch < touchInfo.count; numTouch++) { // Get x,y coordinates from each touch
        int x = touchInfo.x[numTouch];
        int y = touchInfo.y[numTouch];

        // Line up touch controller system with display rotation
        mapCoordinates(x, y);
        x = constrain(x, 0, SCREEN_W - 1);
        y = constrain(y, 0, SCREEN_H - 1);

        for (int numBtn = 0; numBtn < 4; numBtn++) {
          if (inButton(x, y, buttons[numBtn])) {
            // Check to prevent time from being updated again unless button is reset
            if(!buttons[numBtn].pressed) {
              updateTime(buttons[numBtn]); // Set time when pressed
            }
            lastTap = millis();
            break; // Exits loop to prevent from checking other buttons unnecessarily  
          }
        }
      }
    }
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