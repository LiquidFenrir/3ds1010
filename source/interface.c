#include "interface.h"

#define MAX_ENTRIES_PER_SCREEN 9
u8 scroll = 0;

void drawPiece(piece currentPiece, u16 xPos, u16 yPos)
{
	u8 curbit = 0;
	for (int i = 0; i < currentPiece.width; i++) {
		for (int j = 0; j < currentPiece.height; j++) {
			u8 curval = GETBIT(currentPiece.blocks, curbit);
			if (curval == 1) {
				u16 x = xPos + j*10;
				u16 y = yPos + i*10;
				drawSprite( x, y, 16, 16, currentPiece.type + SPRITE_SMALL - 1);
			}
			curbit++;
		}
	}
}

void drawInventory(u8 inventory[3])
{
	for (int i = 0; i < 3; i++) {
		drawPiece(piecesType[inventory[i]], 240, 40+i*60);
	}
}

void drawGrid()
{
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			drawSprite(20 + j*20, 20 + i*20, 32, 32, grid[i][j]);
		}
	}
}

void drawHover(piece currentPiece, u16 xPos, u16 yPos)
{
	u8 curbit = 0;
	for (int i = 0; i < currentPiece.width; i++) {
		for (int j = 0; j < currentPiece.height; j++) {
			u8 curval = GETBIT(currentPiece.blocks, curbit);
			if (curval == 1) {
				u16 x = xPos + j*20;
				u16 y = yPos + i*20;
				drawSpriteWithZ( x, y, 32, 32, currentPiece.type, 0.0f);
			}
			curbit++;
		}
	}
}

void drawInterface(u8 inventory[3], u8 lost, u32 score, u32 highscore)
{
	
	textStartDraw();
	char buf[0x20] = {0};
	sprintf(buf, "highscore: %lu", highscore);
	drawText(14, 12, buf, currentTheme.txtColor);
	sprintf(buf, "score: %lu", score);
	drawText(19, 14, buf, currentTheme.txtColor);
	if (lost != 0)
		drawText(9, 17, "You lost. Press Y to restart.", currentTheme.selTxtColor);
	
	startDraw();
	drawSprite(64, 8, 256, 128, SPRITE_BANNER);
	
	C3D_FrameDrawOn(targets[GFX_BOTTOM]);
	C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection, &projections[GFX_BOTTOM]);
	drawSprite(320-28*3, 4, 32, 32, SPRITE_THEMES);
	drawSprite(320-28*2, 4, 32, 32, SPRITE_RESET);
	drawSprite(320-28, 4, 32, 32, SPRITE_EXIT);
	
	drawGrid();
	drawInventory(inventory);
}

void drawThemesMenu(Theme * themes, u8 themesCount, u8 selected_theme)
{
	textStartDraw();
		
	if (selected_theme == 0) {
		scroll = 0;
	}
	else if (selected_theme == themesCount) {
		scroll = themesCount - MAX_ENTRIES_PER_SCREEN + 1;
		if (scroll >= themesCount)
			scroll = 0;
	}
	else if (selected_theme >= (MAX_ENTRIES_PER_SCREEN + scroll)) {
		scroll++;
	}
	else if ((selected_theme - scroll) < 0) {
		scroll--;
	}

	if (scroll != 0)
		drawText(2, 2, "", currentTheme.selTxtColor);
	
	if ((themesCount > MAX_ENTRIES_PER_SCREEN) && (scroll != themesCount - MAX_ENTRIES_PER_SCREEN + 1))
		drawText(2, 18, "", currentTheme.selTxtColor);
	
	for(int i = scroll; i < (MAX_ENTRIES_PER_SCREEN + scroll); i++) {
		
		char * current_name = (char *)themes[i].name;
		
		if (current_name != NULL) {
			drawText(5, 2*(i-scroll)+2, current_name, (i == selected_theme) ? currentTheme.selTxtColor : currentTheme.txtColor);
		}
	}
	
	endDraw();
}