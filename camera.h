#ifndef CAMERA_H
#define CAMERA_H

#include "general.h"
#include "window.h"

void camInit();
//Pass x and y motion (in pixels) for current frame
void cameraUpdate(int xrel, int yrel);

#endif

