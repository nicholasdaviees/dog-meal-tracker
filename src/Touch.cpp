#include "include/Touch.h"
#include "include/Globals.h"
#include "include/Config.h"
#include "include/UI.h"
#include "include/TimeUtils.h"

// Ensures touch press was inside button
bool inButton(int x, int y, const Button &btn) {
  // Left = btn.x
  // Right = btn.x + btn.w
  // Top = btn.y
  // Bottom = btn.y + btn.h
  return (x >= btn.x && x <= btn.x + btn.w && y >= btn.y && y <= btn.y + btn.h);
}

// Overloaded for ActionBtn
bool inButton(int x, int y, const ActionBtn &btn) {
  return (x >= btn.x && x <= btn.x + btn.w && y >= btn.y && y <= btn.y + btn.h);
}

// Calibrates touch input
void mapCoordinates(int &x, int &y) {
  int new_x = y; // Swap x and y because touch axes are rotated
  int new_y = SCREEN_H - 1 - x; // Invert y axis

  // Update x and y with new values
  x = new_x; 
  y = new_y;
}

void handlePopupTouch(int x, int y) {
  for (int numBtn = 0; numBtn < 2; numBtn++) {
    if (inButton(x, y, ActionBtns[numBtn])) {
      if (numBtn == 1) { // Touch is on checkmark
        resetTime(buttons[buttonToReset]);
      }

      // Redraw main UI
      drawHeader();
      drawDogLabels();
      for (auto &b : buttons) drawButton(b);
      drawIcons();

      confirmationShown = false;
      buttonToReset = -1;
      break;
    }
  }
}

void handleButtonTouch(int x, int y) {
  for (int numBtn = 0; numBtn < 4; numBtn++) {
    if (inButton(x, y, buttons[numBtn])) {
      // Check to prevent time from being updated again unless button is reset
      if (!buttons[numBtn].pressed) {
        updateTime(buttons[numBtn]); // Set time when pressed
      } else {
        drawPopup();
        confirmationShown = true;
        buttonToReset = numBtn;
      }
      break; // Exits loop to prevent from checking other buttons unnecessarily
    }
  }
}