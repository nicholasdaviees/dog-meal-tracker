#pragma once
#include <HomeSpan.h>
#include "Types.h"

// Forward declarations
String getCurrentTime();
void saveTimeToFlash(const Button &btn);
void drawButton(const Button &btn);
void push(const Button &btn);

struct MealSwitch : Service::Switch {
  SpanCharacteristic *switchState;
  Button *btn;

  MealSwitch(Button *btnPtr, const char *switchName); // Constructor

  boolean update() override; // Override for custom update() definition

  void syncHomekitFromButton();
};