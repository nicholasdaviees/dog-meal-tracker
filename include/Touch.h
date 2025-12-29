#pragma once
#include "Types.h"

bool inButton(int x, int y, const Button &btn);
bool inButton(int x, int y, const ActionBtn &btn);
void mapCoordinates(int &x, int &y);
void handlePopupTouch(int x, int y);
void handleButtonTouch(int x, int y);