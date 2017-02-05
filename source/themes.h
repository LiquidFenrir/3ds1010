#pragma once

#include "common.h"

typedef struct {
	u8 * sprite;
	u32 spritesize;
	u32 bgColor;
	char * name;
} Theme;

extern Theme currentTheme;

void loadTheme();
void freeTheme();