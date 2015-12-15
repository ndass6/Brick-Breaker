/*
 * Exported with nin10kit v1.1
 * Invocation command was nin10kit -resize=240x160 -mode=4 wonGame wonGame.gif 
 * Time-stamp: Monday 11/09/2015, 21:46:59
 * 
 * Image Information
 * -----------------
 * wonGame.gif (frame 0) 240@160
 * wonGame.gif (frame 1) 240@160
 * wonGame.gif (frame 2) 240@160
 * wonGame.gif (frame 3) 240@160
 * wonGame.gif (frame 4) 240@160
 * wonGame.gif (frame 5) 240@160
 * wonGame.gif (frame 6) 240@160
 * 
 * Quote/Fortune of the Day!
 * -------------------------
 * 
 * All bug reports / feature requests are to be sent to Brandon (bwhitehead0308@gmail.com)
 */

#ifndef WONGAME_H
#define WONGAME_H

extern const unsigned short wonGame_palette[255];
#define WONGAME_PALETTE_SIZE 255

extern const unsigned short wonGame0[19200];

extern const unsigned short wonGame1[19200];

extern const unsigned short wonGame2[19200];

extern const unsigned short wonGame3[19200];

extern const unsigned short wonGame4[19200];

extern const unsigned short wonGame5[19200];

extern const unsigned short wonGame6[19200];

extern const unsigned short* wonGame_frames[7];
#define WONGAME_FRAMES 7
#define WONGAME_SIZE 19200
#define WONGAME_WIDTH 240
#define WONGAME_HEIGHT 160

#endif

