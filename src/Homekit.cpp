#include "include/Homekit.h"
#include "include/Globals.h"
#include "include/TimeUtils.h"
#include "include/Storage.h"
#include "include/UI.h"

// Builds the Homekit switch and assigns it to a button
MealSwitch::MealSwitch(Button *btnPtr, const char *switchName) : Service::Switch() {
  btn = btnPtr;
  switchState = new Characteristic::On(btn->pressed);
  new Characteristic::Name(switchName);
}

// Updates button on board display when switch is updated in Homekit
boolean MealSwitch::update() {
  bool newSwitchState = switchState->getNewVal();
  push(*btn);

  // Apply new switch state to update button
  btn->pressed = newSwitchState;
  btn->time = newSwitchState ? getCurrentTime() : DEFAULT_TIMESTAMP;
  saveTimeToFlash(*btn);
  drawButton(*btn);

  return true;
}

// Sends update to Homekit when button status is changed
void MealSwitch::syncHomekitFromButton() {
  switchState->setVal(btn->pressed);
}