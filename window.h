#ifndef WINDOW_H
#define WINDOW_H

#include "general.h"

void initWindow();
void initShaders();
void initVBO();
void doFrame();
void updateMatrices(mat4& view, mat4& proj);

#endif

