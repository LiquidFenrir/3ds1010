#include <dirent.h>
#include "themes.h"

Theme currentTheme;

void listThemes(Theme themes[256], u8 * themesCount)
{
	DIR *dir;
	struct dirent *ent;
	
	u8 count = *themesCount;
	char themesdir[256];
	sprintf(themesdir, "%s%s/", WORKING_DIR, "Themes");
	
	if ((dir = opendir(themesdir)) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			count++;
			themes[count].name = strdup(ent->d_name);
		}
  closedir (dir);
	}
	
	*themesCount = count;
}

void loadTheme()
{
	char themedir[256];
	if (strcmp(currentTheme.name, DEFAULT_THEME) == 0) {
		sprintf(themedir, "romfs:/");
	}
	else {
		sprintf(themedir, "%s%s/%s/", WORKING_DIR, "Themes", currentTheme.name);
	}
	
	char spritespath[256];
	sprintf(spritespath, "%s%s", themedir, "sprites.png");
	FILE * fptr = fopen(spritespath, "rb");
	
	fseek(fptr, 0, SEEK_END);
	currentTheme.spritesize = ftell(fptr);
	fseek(fptr, 0, SEEK_SET);
	
	u8 * buf = malloc(currentTheme.spritesize);
	fread(buf, currentTheme.spritesize, 1, fptr);
	
	currentTheme.sprite = buf;
	
	fclose(fptr);
	
	char filepath[256];
	sprintf(filepath, "%s%s", themedir, "colors.txt");
	FILE * colorfptr = fopen(filepath, "rb");
	u32 * colors[3] = {&currentTheme.bgColor, &currentTheme.txtColor, &currentTheme.selTxtColor};
	u8 * tmpbuf = malloc(1);
	
	for (int i = 0; i < 3; i++) {
		char tempcolor[6] = {0};
		fread(tempcolor, 6, 1, colorfptr);
		
		if (i != 2) { //skip newline characters for the first 2 lines
			fread(tmpbuf, 1, 1, colorfptr);
			if (*tmpbuf == 0x0D) //if the file has windows file endings, have to remove another bytes
				fread(tmpbuf, 1, 1, colorfptr);
		}
		
		*colors[i] =  (strtoul(tempcolor, NULL, 16) << 8) | 0xFF; //0xFF alpha
	}
	fclose(colorfptr);
	free(tmpbuf);
}

void freeTheme()
{
	free(currentTheme.sprite);
}