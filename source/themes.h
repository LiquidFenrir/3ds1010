#pragma once

#include "common.h"

typedef struct {
	u8 * sprite;
	u32 spritesize;
	u32 bgColor;
	u32 txtColor;
	u32 selTxtColor;
	char * name;
} Theme;

extern Theme currentTheme;

void listThemes(Theme themes[256], u8 * themesCount);
void loadTheme();
void freeTheme();