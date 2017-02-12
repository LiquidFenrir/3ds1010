#include <citro3d.h>
#include "common.h"

enum sprites {
	SPRITE_BLANK,
	SPRITE_FULL,
	SPRITE_FULL_BAR,
	SPRITE_FULL_CORNER,
	SPRITE_THEMES,
	SPRITE_RESET,
	SPRITE_EXIT,
	SPRITE_SMALL,
	SPRITE_SMALL_BAR,
	SPRITE_SMALL_CORNER,
	SPRITE_BANNER
};

void drawSpriteWithZ(int x, int y, int width, int height, int image, float z);
void drawSprite( int x, int y, int width, int height, int image);
void setupTextures(u8 * imgData, u32 imgSize, u32 clear_color);
void sceneExit(void);
void startDraw(void);
void endDraw(void);