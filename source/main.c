#include "interface.h"
#include "pieces.h"
#include "grid.h"
#include "save.h"

piece piecesType[PIECES_AMOUNT] = {0}; //piece 0 is reserved for blank
u8 grid[10][10] = {0};
static u8 quit = 0;

void themesMenu()
{
	Theme themes[256] = {0};
	Theme defaultTheme;
	defaultTheme.name = DEFAULT_THEME;
	themes[0] = defaultTheme;
	u8 themesCount = 0;
	u8 selected_theme = 0;
	if (strcmp(currentTheme.name, themes[selected_theme].name) == 0)
		selected_theme = 1;
	
	listThemes(themes, &themesCount);
	drawThemesMenu(themes, themesCount, selected_theme);
	selected_theme = 0;
	
	while(aptMainLoop()) {
		drawThemesMenu(themes, themesCount, selected_theme);
		hidScanInput();
		
		if (hidKeysDown() & KEY_START) {
			quit = 1;
			break;
		}
		else if(hidKeysDown() & KEY_A) {
			if (strcmp(currentTheme.name, themes[selected_theme].name) != 0) {
				sceneExit();
				currentTheme = themes[selected_theme];
				loadTheme();
				setupTextures(currentTheme.sprite, currentTheme.spritesize, currentTheme.bgColor);
			}
			break;
		}
		else if(hidKeysDown() & KEY_B) {
			break;
		}
		else if(hidKeysDown() & KEY_UP) {
			if (selected_theme != 0)
				selected_theme--;
		}
		else if(hidKeysDown() & KEY_DOWN) {
			if (selected_theme != themesCount)
				selected_theme++;
		}
		else if(hidKeysDown() & KEY_LEFT) {
			selected_theme = 0;
		}
		else if(hidKeysDown() & KEY_RIGHT) {
			selected_theme = themesCount;
		}
		
		gfxFlushBuffers();
		gfxSwapBuffers();
		gspWaitForVBlank();
	}
}

int main()
{
	gfxInitDefault();
	romfsInit();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	
	u8 inventory[3] = {0};
	u8 selected_tile = 0;
	u8 selected_piece = 0;
	u8 hoveringPiece = 0;
	u32 score = 0;
	u32 highscore = 0;
	u8 place = 0;
	u8 hover = 0;
	currentTheme.name = DEFAULT_THEME;
	
	setupScreens();
	generatePiecesTypes();
	
	readSave(inventory, &score, &highscore);
	loadTheme();
	setupTextures(currentTheme.sprite, currentTheme.spritesize, currentTheme.bgColor);
	
	while (aptMainLoop())
	{
		if (inventory[0] == 0 && inventory[1] == 0 && inventory[2] == 0 && hoveringPiece == 0) //refill inventory when all pieces are placed
			getPieces(inventory);
		
		touchPosition touch;
		hidTouchRead(&touch);
		hidScanInput();
		
		startDraw();
		drawInterface(selected_tile, inventory, selected_piece, score, highscore);
		
		if (hover == 1) {
			drawHover(piecesType[hoveringPiece], touch.px-8, touch.py-8);
			if (hidKeysUp() & KEY_TOUCH) {
				inventory[selected_piece] = hoveringPiece;
				hoveringPiece = 0;
				place = 1;
				hover = 0;
			}
		}
		else {
			for (int i = 0; i < 3; i++) {
				if (touch.px >= 240 && touch.px <= 290) {
					if (touch.py >= (20 + i*50) && touch.py <= (70 + i*50)) {
						selected_piece = i;
						hoveringPiece = inventory[i];
						inventory[i] = 0;
						hover = 1;
					}
				}
			}
		}
		
		endDraw();
		
		if (touch.px >= 20 && touch.px <= 220 && touch.py >= 20 && touch.py <= 220) {
			u8 x = (touch.px - 20)/20;
			u8 y = (touch.py - 20)/20;
			selected_tile = y*10 +x;
		}
		
		/* save and quit */
		if (hidKeysDown() & KEY_START || quit != 0) {
			saveToFile(inventory, score, highscore);
			break;
		}
		/* place the selected piece on the selected tile (starting from top left block) */
		else if (hidKeysDown() & KEY_A || place != 0) {
			u8 tempscore = placePiece(selected_tile, piecesType[inventory[selected_piece]]);
			place = 0;
			if (tempscore != 0) {
				score += tempscore;
				score += checkGrid();
				inventory[selected_piece] = 0;
				if (score > highscore)
					highscore = score;
			}
		}
		/*
		navigate through the grid
		checks prevent the cursor from going out of the grid
		*/
		else if (hidKeysDown() & KEY_UP) {
			if (selected_tile/10 != 0)
				selected_tile -= 10;
		}
		else if (hidKeysDown() & KEY_DOWN) {
			if (selected_tile/10 != 9)
				selected_tile += 10;
		}
		else if (hidKeysDown() & KEY_LEFT) {
			if (selected_tile%10 != 0)
				selected_tile -= 1;
		}
		else if (hidKeysDown() & KEY_RIGHT) {
			if (selected_tile%10 != 9)
				selected_tile += 1;
		}
		/* select the piece to place */
		else if (hidKeysDown() & KEY_L) {
			if (selected_piece != 0)
				selected_piece--;
		}
		else if (hidKeysDown() & KEY_R) {
			if (selected_piece != 2)
				selected_piece++;
		}
		/* reset */
		else if (hidKeysDown() & KEY_Y) {
			memset(&grid, 0, 100);
			score = 0;
			selected_piece = 0;
			selected_tile = 0;
			getPieces(inventory);
		}
		/* open the themes menu */
		else if (hidKeysDown() & KEY_X) {
			themesMenu();
			consoleClear();
			printf("\x1b[0;0Hhighscore: %lu", highscore);
			printf("\x1b[1;4Hscore: %lu", score);
		}
		
	}
	
	sceneExit();
	
	C3D_Fini();
	romfsExit();
	gfxExit();
	return 0;
}
