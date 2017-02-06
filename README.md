# 3ds1010
Port of [1010!](http://1010ga.me/) to 3ds homebrew, made in C.  
It's like Tetris but with infinite time, you lose when you can no longer place any of the pieces you have available.  
  
Displays a 10 by 10 (hence the name) grid of blocks with 3 pieces you can place. When you complete a line or a column, it disappears, leaving space for new pieces and giving you a lot of points. You can't really win this game, the goal is just to get more points than the next guy

# Instructions
Use the touchscreen to select a piece, then drag it to the place you want on the grid. If you release it outside of the grid, or in a place where it doesn't fit, it will go back to your inventory

OR

Use the D-PAD to move your cursor and select where to place the selected piece.  
Use the L/R shoulder buttons to select which piece you want to place.  
Press A to place the selected piece.

Press Y to reset (if you lost, or want to start over), or START to quit.
It doesnt tell you where your cursors are, so I recommend using the touchscreen. If in doubt, press LEFT, UP, and L a couple times to go back to the top left corner and first piece

# Save format
There have been 2 save file format for now.
The first one is composed, in order, of: 
- 20 bytes for the grid
- 3 bytes for the inventory (each one containing 1 piece)
- 4 bytes for the current score
- 4 bytes for the highscore

The second, newer, is pretty much the same except for:
- 1 byte for the length of the theme name
- X bytes for the theme name (in ASCII, without a NULL terminator)

So, it's not hard to convert a type 1 save to a type 2, just append 0x0744656661756c74 to it - this shows a length of 7 and the name "Default", for the default theme included in the romfs

#Credits
Thanks to the folks at #3dsdev and #Cakey for the help
And xerpi and fincs for helping fix the drawing