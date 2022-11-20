#pragma once

#ifndef ADDON_BUTTONDEBOUNCE_H
#define ADDON_BUTTONDEBOUNCE_H

#include <Arduino.h>

int buttonRefresh();
int buttonStatus(int buttonid);
void buttonsInit();
int checkButton(struct debounceButton *butt);
void buttonDecoder();
#endif