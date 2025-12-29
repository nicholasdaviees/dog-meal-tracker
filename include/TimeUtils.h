#pragma once
#include <Arduino.h>

#include "Types.h"

String getCurrentTime();
void updateTime(Button &btn);
void resetTime(Button &btn);
bool isMidnight();