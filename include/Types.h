#pragma once
#include <Arduino.h>
#include <stdint.h>

constexpr int STACK_SIZE = 10;

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
struct ActionBtn {
  int x, y, w, h;
};

// Stack for undo button
struct UndoStack {
  Button buf[STACK_SIZE];
  int head = 0; // Index of oldest element
  int size = 0;
};