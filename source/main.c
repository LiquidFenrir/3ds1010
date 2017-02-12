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
	
	listThemes(themes, &themesCount);
	
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
	s16 x = 42;
	s16 y = 42;
	currentTheme.name = DEFAULT_THEME;
	
	generatePiecesTypes();
	
	getPieces(inventory);
	readSave(inventory, &score, &highscore);
	u8 lost = checkInventory(inventory)^1;
	
	loadTheme();
	setupTextures(currentTheme.sprite, currentTheme.spritesize, currentTheme.bgColor);
	
	while (aptMainLoop())
	{
		
		touchPosition touch;
		hidTouchRead(&touch);
		hidScanInput();
		
		drawInterface(inventory, lost, score, highscore);
		
		if (hover == 1) {
			if (hidKeysHeld() & KEY_TOUCH) {
				x = touch.px;
				y = touch.py;
			}
			else if (hidKeysHeld() & KEY_UP) {
				y -= 4;
			}
			else if (hidKeysHeld() & KEY_DOWN) {
				y += 4;
			}
			else if (hidKeysHeld() & KEY_LEFT) {
				x -= 4;
			}
			else if (hidKeysHeld() & KEY_RIGHT) {
				x += 4;
			}
			
			if (x < 4)
				x = 4;
			if (x > 316)
				x = 316;
			
			if (y < 4)
				y = 4;
			if (y > 236)
				y = 236;
			
			if (BETWEEN(x, 20, 220) && BETWEEN(y, 20, 220)) {
				u8 column = (x - 20)/20;
				u8 row = (y - 20)/20;
				selected_tile = row*10 + column;
			}
			
			drawHover(piecesType[hoveringPiece], x-8, y-8);
			
			if (hidKeysUp() & KEY_TOUCH || hidKeysUp() & KEY_A) {
				inventory[selected_piece] = hoveringPiece;
				hoveringPiece = 0;
				hover = 0;
				if (BETWEEN(x, 20, 220) && BETWEEN(y, 20, 220))
					place = 1;
			}
		}
		else {
			for (int i = 0; i < 3; i++) {
				if (BETWEEN(touch.px, 240, 290)) {
					if (BETWEEN(touch.py, 40 + i*50, 90 + i*50)) {
						selected_piece = i;
						hoveringPiece = inventory[i];
						inventory[i] = 0;
						hover = 1;
					}
				}
			}
			if (hidKeysDown() & KEY_A) {
				selected_piece = 1;
				selected_piece -=  (hidKeysHeld() & KEY_L) ? 1 : 0;
				selected_piece +=  (hidKeysHeld() & KEY_R) ? 1 : 0;
				
				hoveringPiece = inventory[selected_piece];
				inventory[selected_piece] = 0;
				hover = 1;
			}
		}
		
		endDraw();
		
		if (BETWEEN(touch.py, 4, 28)) {
			if (BETWEEN(touch.px, 320-28, 320-4)) {
				quit = 1;
			}
			else if (BETWEEN(touch.px, 320-56, 320-32)) {
				goto reset;
			}
			else if (BETWEEN(touch.px, 320-84, 320-60)) {
				goto themes;
			}
		}
		
		/* save and quit */
		if (hidKeysDown() & KEY_START || quit != 0) {
			saveToFile(inventory, score, highscore);
			break;
		}
		/* place the selected piece on the selected tile (starting from top left block) */
		else if (place != 0) {
			u8 tempscore = placePiece(selected_tile, piecesType[inventory[selected_piece]]);
			place = 0;
			if (tempscore != 0) {
				score += tempscore;
				score += checkGrid();
				inventory[selected_piece] = 0;
				
				if (score > highscore)
					highscore = score;
				
				if (inventory[0] == 0 && inventory[1] == 0 && inventory[2] == 0 && hoveringPiece == 0) //refill inventory when all pieces are placed
					getPieces(inventory);
				
				lost = checkInventory(inventory)^1;
			}
		}
		/* reset */
		else if (hidKeysDown() & KEY_Y) {
			reset:
				memset(&grid, 0, 100);
				score = 0;
				lost = 0;
				selected_piece = 0;
				selected_tile = 0;
				getPieces(inventory);
		}
		/* open the themes menu */
		else if (hidKeysDown() & KEY_X) {
			themes:
				themesMenu();
		}
	}
	
	sceneExit();
	
	C3D_Fini();
	romfsExit();
	gfxExit();
	return 0;
}
