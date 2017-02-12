# 3ds1010
Port of [1010!](http://1010ga.me/) to 3ds homebrew, made in C.  
It's like Tetris but with infinite time, you lose when you can no longer place any of the pieces you have available.  
  
Displays a 10 by 10 (hence the name) grid of blocks with 3 pieces you can place. When you complete a line or a column, it disappears, leaving space for new pieces and giving you a lot of points. You can't really win this game, the goal is just to get more points than the next guy

![Screenshot](http://i.imgur.com/NKWK5IV.png)

# Instructions
Use the touchscreen or press and hold L/R/Nothing + A to select a piece, then drag it to the place you want on the grid using the stylus or the D-PAD/Analog stick. If you release it outside of the grid, or in a place where it doesn't fit, it will go back to your inventory

Press X to open the Themes menu, Y to reset (if you lost, or want to start over), or START to quit, or touch the corresponding icons (they're in the same order on the screen).

# Save format
There have been 3 save file format for now.
The first one is composed, in order, of: 
- 20 bytes for the grid
- 3 bytes for the inventory (each one containing 1 piece)
- 4 bytes for the current score
- 4 bytes for the highscore

The second, newer, is pretty much the same except for:
- 1 byte for the length of the theme name
- X bytes for the theme name (in ASCII, without a NULL terminator)

The third and latest, use 40 bytes for the grid.

I included a converter for the save (accurately named "saveconverter"). A binary should be present in the release, and the source in the saveconverter.c file.

# Theme format
There have been 2 save formats for now:
The first one is composed of:
- a 3 bytes file named bgColor.bin containing the color of the background in RGB8
- a 64x64 pixels 32-bit color png file containing 4 32x32 sprites:
  - 1 for the "blank" blocks of the grid
  - 1 for the "filled" blocks of the grid
  - 1 unused for an options menu
  - 1 for the small blocks of the pieces in the inventory

The second and latest contains:
- a text file named colors.txt containing on 3 lines: the color of the background, the color of the text, the color of selected text (for the Themes menu), all in RGB8
- a 128x128 pixels 32-bit color png file containing 10 sprites:
  - 1 128x64 one for the banner displayed on the top screen
  - 4 32x32 sprites, for the "blank", and different colors of "filled" depending on the piece type
  - 3 32x32 sprites for the Themes, Reset, and Exit options
  - 3 16x16 for the different colors of pieces in the inventory

#Credits
Thanks to the folks at #3dsdev and #Cakey for the help, especially Desynched (DDews), xerpi and fincs for helping fix the drawing
