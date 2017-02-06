#include "interface.h"

static PrintConsole scoreWindow;
static u32 oldScore = -1;
static u8 oldSelectedTheme = -1;

#define MENU_WIDTH 40
#define MAX_ENTRIES_PER_SCREEN 18
u8 scroll = 0;

void setupScreens()
{
	consoleInit(GFX_TOP, &scoreWindow);
	consoleSelect(&scoreWindow);
}

void drawPiece(piece currentPiece, u16 xPos, u16 yPos)
{
	u8 curbit = 0;
	for (int i = 0; i < currentPiece.width; i++) {
		for (int j = 0; j < currentPiece.height; j++) {
			u8 curval = GETBIT(currentPiece.blocks, curbit);
			if (curval == 1) {
				u16 x = xPos + j*10;
				u16 y = yPos + i*10;
				drawSprite( x, y, 16, 16, SPRITE_SMALL);
			}
			curbit++;
		}
	}
}

void drawInventory(u8 inventory[3], u8 selected_piece)
{
	for (int i = 0; i < 3; i++) {
		drawPiece(piecesType[inventory[i]], 240, 20+i*60);
	}
}

void drawGrid(u8 selected_tile)
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
				drawSpriteWithZ( x, y, 32, 32, SPRITE_FULL, 0.0f);
			}
			curbit++;
		}
	}
}

void drawInterface(u8 selected_tile, u8 inventory[3], u8 selected_piece, u32 score, u32 highscore)
{
	if (oldScore != score) {
		consoleClear();
		printf("\x1b[0;0Hhighscore: %lu", highscore);
		printf("\x1b[1;4Hscore: %lu", score);
		oldScore = score;
	}
	
	drawGrid(selected_tile);
	drawInventory(inventory, selected_piece);
}

void drawThemesMenu(Theme * themes, u8 themesCount, u8 selected_theme)
{
	if (oldSelectedTheme != selected_theme) {
		consoleClear();		
		oldSelectedTheme = selected_theme;
		
		if (selected_theme == 0) {
			scroll = 0;
		}
		else if(selected_theme == themesCount) {
			scroll = themesCount - MAX_ENTRIES_PER_SCREEN;
			if (scroll >= themesCount)
				scroll = 0;
		}
		else if (selected_theme >= (MAX_ENTRIES_PER_SCREEN + scroll)) {
			scroll++;
		}
		else if ((selected_theme - scroll) < 0) {
			scroll--;
		}
		
		for (int i = 0; i <= MENU_WIDTH; i++) {
			printf("\x1b[0;40;37m\x1b[5;%uH=", (4+i));
			printf("\x1b[0;40;37m\x1b[24;%uH=", (4+i));
		}
		
		for(int i = scroll; i < (MAX_ENTRIES_PER_SCREEN + scroll); i++) {
			
			char * current_name = (char *)themes[i].name;
			
			if (current_name != NULL) {
				char format[64];
				sprintf(format, "\x1b[%u;4H", (i+6-scroll));
				
				if (i == selected_theme ) {
					strcat(format, "\x1b[47;30m"); //selected entry has gray background
				}
				
				strcat(format, "%s");
				for (u8 i = 0; i < (MENU_WIDTH - strlen(current_name) + 1); i++) {
					strcat(format, " ");
				}
				strcat(format, "\x1b[0m"); //remove all color
				
				printf(format, current_name);
			}
		}
	}
}