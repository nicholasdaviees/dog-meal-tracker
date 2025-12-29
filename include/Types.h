#pragma once
#include <Arduino.h>
#include <stdint.h>

// Normal buttons
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

// Action buttons
// ******* LIKELY TO CHANGE *******
struct ActionBtn {
  int x, y, w, h;
};
// ******* LIKELY TO CHANGE *******