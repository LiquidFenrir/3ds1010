#pragma once

#include "common.h"

//0 is top screen, 1 is bottom screen
extern C3D_Mtx projections[2];
extern C3D_RenderTarget* targets[2];
extern int uLoc_projection;

void textSceneInit(void);
void setTextColor(u32 color);
void drawText(u16 x, u16 y, const char * str, u32 color);
void textSceneExit(void);
void textStartDraw(void);