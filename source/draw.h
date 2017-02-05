#include <citro3d.h>
#include "common.h"
#include "themes.h"

enum sprites {
	SPRITE_BLANK = 0,
	SPRITE_FULL = 1,
	SPRITE_OPTIONS = 2,
	SPRITE_SMALL = 3
};

void drawSpriteWithZ(int x, int y, int width, int height, int image, float z);
void drawSprite( int x, int y, int width, int height, int image);
void setupTextures(u8 * imgData, u32 imgSize, u32 clear_color);
void sceneExit(void);
void startDraw(void);
void endDraw(void);