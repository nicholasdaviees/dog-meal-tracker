#include "include/UI.h"

#include <TFT_eSPI.h>
#include <SPI.h>
#include <Wire.h>
#include <time.h>
#include <bb_captouch.h>
#include <WiFi.h>
#include <Preferences.h>

#include "include/Globals.h"
#include "include/Config.h"

// Include icon header files
#include "../images/pawPadIcon.h"
#include "../images/mollyIcon.h"
#include "../images/tobyIcon.h"
#include "../images/checkmarkMorningIcon.h"
#include "../images/checkmarkEveningIcon.h"
#include "../images/xMorningIcon.h"
#include "../images/xEveningIcon.h"
#include "../images/checkmarkPopupIcon.h"
#include "../images/xPopupIcon.h"

// Sets background color and draws title text
void drawHeader() {
  tft.fillScreen(COL_BG);
  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(COL_TEXT, COL_BG);                     
  tft.drawString("Dog Meal Tracker", 35, 8, 4); // (text, x, y, fontSize)
  tft.drawLine(20, 42, SCREEN_W - 20, 42, COL_LINE); // (x0, y0, x1, y1, color)
  tft.drawLine(SCREEN_W / 2, 62, SCREEN_W / 2, 218, COL_TEXT); // Dividing center line between buttons
}

void drawDogLabels() {
  tft.setTextDatum(TC_DATUM);
  tft.setTextColor(COL_TEXT, COL_BG);
  tft.drawString(DOG_NAMES[0], SCREEN_W / 3, 62, 4); // (text, x, y, fontSize)
  tft.drawString(DOG_NAMES[1], SCREEN_W * 3.35 / 4, 62, 4);
}

// Draws meal buttons
void drawButton(const Button &btn) {
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
  tft.pushImage(20, 48, 48, 48, mollyIcon);
  tft.pushImage((SCREEN_W / 2) + 20, 50, 48, 48, tobyIcon);
}

// Draws confirmation message when action buttons are pressed
void drawPopup() {
  const int iconW = 40;
  const int iconH = 40;
  const int spacing = 15;

  tft.setTextFont(4);
  int textH = tft.fontHeight() - 8;
  
  // Popup box positions
  int w = 200;
  int h = spacing + textH + spacing + iconH + spacing;
  int x = (SCREEN_W - w) / 2;
  int y = (SCREEN_H - h) / 2;

  // Popup background
  tft.fillRoundRect(x, y, w, h, 10, POPUP_BG);
  tft.drawRoundRect(x, y, w, h, 10, COL_TEXT);

  // Popup text
  int textX = x + (w - tft.textWidth("Reset time?")) / 2;
  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(COL_TEXT, POPUP_BG);
  tft.drawString("Reset time?", textX, y + spacing);

  // Calculate icon positions
  int groupW = iconW * 2 + spacing;
  int groupX = x + (w - groupW) / 2;
  int groupY = y + spacing + textH + spacing;

  // Update icons with correct positions
  ActionBtns[0] = {groupX, groupY, iconW, iconH};
  ActionBtns[1] = {groupX + iconW + spacing, groupY, iconW, iconH};

  // Draw icons
  tft.setSwapBytes(true);
  tft.pushImage(ActionBtns[0].x, ActionBtns[0].y, ActionBtns[0].w, ActionBtns[0].h, xPopupIcon);
  tft.pushImage(ActionBtns[1].x, ActionBtns[1].y, ActionBtns[1].w, ActionBtns[1].h, checkmarkPopupIcon);
}