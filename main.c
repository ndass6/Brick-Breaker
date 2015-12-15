// Name: Nathan Dass

#include "mylib.h"
#include "start.h"
#include "levels.h"
#include "nextLevel.h"
#include "loseLife.h"
#include "gameOver.h"
#include "ballPic.h"
#include "wonGame.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


enum {
	START_GAME, PLAY_GAME, LOSE_LIFE, GAME_OVER, NEXT_LEVEL, WON_GAME
};

enum {
	REDIDX, GREENIDX, BLUEIDX, MAGENTAIDX, CYANIDX, YELLOWIDX, WHITEIDX, ORANGEIDX
};

typedef struct Player {
	int x;
	int y;
	int oldX;
	int oldY;
	int width;
	int height;
	int color;
} Player;

typedef struct Brick {
	int x;
	int y;
	int type;
	int width;
	int height;
	int color;
} Brick;

typedef struct Ball {
	int x;
	int y;
	int oldX;
	int oldY;
	int xDir;
	int yDir;
	int width;
	int height;
	int color;
} Ball;

Player player;

Brick bricks[20];
int brickPos;

Ball ball;

int buttonA;
int lives;
int state;
int level;
int frame;

volatile u16 bgcolor4dma;
u8 bgColorIdx;
u8 numColors;

void startScreen();
void playGame();
void checkKeys();
void updatePlayer();
void updateBricks();
void updateBall();
void loseLife();
void lostGame();
void nextLevel();
void wonGame();
void getBricks();
void newLevel();
void resetLevel();
void resetEnemies();
void loadGamePalette();

int main(void) {
	brickPos = 0;

	buttonA = 0;
	lives = INITIAL_LIVES;
	level = 1;
	frame = 0;

	state = START_GAME;

	while(1) {
		REG_DISPCTL = MODE4 | BG2_ENABLE;
		while (KEY_PRESSED(BUTTON_SELECT)) {
			state = START_GAME;
		}
		switch (state) {
			case START_GAME:
				startScreen();
				break;
			case PLAY_GAME:
				playGame();
				break;
			case LOSE_LIFE:
				loseLife();
				break;
			case GAME_OVER:
				lostGame();
				break;
			case NEXT_LEVEL:
				nextLevel();
				break;
			case WON_GAME:
				wonGame();
				break;
		}
	}
	return 0;
}

void startScreen() {
	REG_DISPCTL = MODE4 | BG2_ENABLE;
	loadPalette(start_palette, START_PALETTE_SIZE);
	numColors = START_PALETTE_SIZE;
	int count = 0;
	while (state == START_GAME) {
		fillScreen4(0);
		drawImage4(0, 0, 240, 160, start);
		waitForVblank();
		FlipPage();
		if (!KEY_PRESSED(BUTTON_START))
			count++;
		if (KEY_PRESSED(BUTTON_START)) {
			while (KEY_PRESSED(BUTTON_START));
			srand(count);
			state = PLAY_GAME;

			lives = INITIAL_LIVES;
			level = 1;

			newLevel();
			getBricks();
		}
	}
}

void playGame() {
	REG_DISPCTL = MODE4 | BG2_ENABLE;

	loadGamePalette();

	FlipPage();
        
    // Clear Screen
    bgcolor4dma = bgColorIdx | (bgColorIdx << 8);
    DMA[3].src = &bgcolor4dma;
    DMA[3].dst = videoBuffer;
    DMA[3].cnt = (240*160/2) | DMA_SOURCE_FIXED | DMA_ON;
    FlipPage();
    DMA[3].src = &bgcolor4dma;
    DMA[3].dst = videoBuffer;
    DMA[3].cnt = (240*160/2) | DMA_SOURCE_FIXED | DMA_ON;

	while (state == PLAY_GAME) {
		fillScreen4(bgColorIdx);
		checkKeys();
		updatePlayer();
		updateBricks();
		updateBall();
		
		char buffer[50];
		drawRect4(150, 0, 10, 240, bgColorIdx);
		if (brickPos == 0) {
			if (level + 1 <= HIGHEST_LEVEL)
				state = NEXT_LEVEL;
			else
				state = WON_GAME;
		}
		else {
			if (brickPos == 1)
				sprintf(buffer, "%d brick left.", brickPos);
			else
				sprintf(buffer, "%d bricks left.", brickPos);
			drawString4(150, 35, buffer, BLUEIDX);
			if (lives == 1)
				sprintf(buffer, "%d life left.", lives);
			else
				sprintf(buffer, "%d lives left.", lives);
			drawString4(150, 135, buffer, BLUEIDX);
			waitForVblank();
			FlipPage();		
			while (KEY_PRESSED(BUTTON_SELECT))
				state = START_GAME;
		}
	}
}

void checkKeys() {
	drawRect4(player.oldY, player.oldX, player.width, player.height, bgColorIdx);
	player.oldX = player.x;
	player.oldY = player.y;
	if (KEY_PRESSED(BUTTON_LEFT))
		player.x = player.x - 2;
	if (KEY_PRESSED(BUTTON_RIGHT))
		player.x = player.x + 2;
	if (buttonA == 0 && KEY_PRESSED(BUTTON_A)) {
		int newColor = (rand() % numColors) + 1;
		while (player.color == newColor || newColor == bgColorIdx)
			newColor = rand() % numColors;
		player.color = newColor;
		buttonA = 1;
	} else if (buttonA == 1 && !KEY_PRESSED(BUTTON_A))
		buttonA = 0;
}

void updatePlayer() {
	if (player.x < 0)
		player.x = 0;
	if (player.x + player.height > 240)
		player.x = 240 - player.height;
	drawRect4(player.y, player.x, player.width, player.height, player.color);
}

void updateBricks() {
	for (int i = 0; i < brickPos; i++) {
		drawRect4(bricks[i].y, bricks[i].x, bricks[i].width, bricks[i].height, bgColorIdx);
		if ((ball.x <= bricks[i].x + bricks[i].height &&
				ball.x >= bricks[i].x &&
				ball.y <= bricks[i].y + bricks[i].width &&
				ball.y >= bricks[i].y) ||
				(bricks[i].x <= ball.x + ball.height &&
				bricks[i].x >= ball.x &&
				bricks[i].y <= ball.y + ball.width &&
				bricks[i].y >= ball.y)) {
			ball.yDir *= -1;
			for (int k = i; k < brickPos - 1; k++)
				bricks[k] = bricks[k + 1];
			brickPos--;
			i--;
		} else
			drawRect4(bricks[i].y, bricks[i].x, bricks[i].width, bricks[i].height, bricks[i].color);
	}
}

void updateBall() {
	drawRect4(ball.oldY, ball.oldX, ball.height, ball.width, bgColorIdx);
	ball.y += ball.yDir;
	ball.x += ball.xDir;
	if (ball.x < 0) {
		ball.x = 0;
		ball.xDir *= -1;
	}
	if (ball.x > 240 - ball.height) {
		ball.x = 240 - ball.height;
		ball.xDir *= -1;
	}
	if (ball.y < 0) {
		ball.y = 0;
		ball.yDir *= -1;
	}
	if (ball.y + ball.width > player.y && ball.yDir > 0) {
		if (player.x < ball.x && ball.x < player.x + player.height) {
			ball.y = player.y - player.width - ball.width;
			ball.yDir *= -1;
			ball.yDir = (rand() % 2 + 1) * ball.yDir / abs(ball.yDir);
			ball.xDir = (rand() % 2 + 1) * ball.xDir / abs(ball.xDir);
		} else {
			ball.y = player.y - ball.width;
			ball.yDir *= -1;
			lives--;
			if (lives > 0)
				state = LOSE_LIFE;
			else
				state = GAME_OVER;
		}
	}
	ball.oldY = ball.y;
	ball.oldX = ball.x;
	loadPalette(ballPic_palette, (u8) BALLPIC_PALETTE_SIZE);
	numColors = (u8) BALLPIC_PALETTE_SIZE;
	drawImage4(ball.y, ball.x, ball.height, ball.width, ballPic);
	loadGamePalette();
}

void loseLife() {
	REG_DISPCTL = MODE4 | BG2_ENABLE;
	frame = 0;
	loadPalette(loseLife_palette, LOSELIFE_PALETTE_SIZE);
	numColors = LOSELIFE_PALETTE_SIZE;
	while (state == LOSE_LIFE) {
		drawImage4(0, 0, 240, 160, loseLife_frames[(frame >> 1) % LOSELIFE_FRAMES]);
		waitForVblank();
		FlipPage();
		frame++;
		if (KEY_PRESSED(BUTTON_START)) {
			while (KEY_PRESSED(BUTTON_START));
			state = PLAY_GAME;
			resetLevel();
		}
		while (KEY_PRESSED(BUTTON_SELECT))
			state = START_GAME;
	}
}

void lostGame() {
	REG_DISPCTL = MODE4 | BG2_ENABLE;
	frame = 0;
	loadPalette(gameOver_palette, GAMEOVER_PALETTE_SIZE);
	numColors = GAMEOVER_PALETTE_SIZE;
	while (state == GAME_OVER) {
		fillScreen4(0);
		drawImage4(0, 0, 240, 160, gameOver);
		waitForVblank();
		FlipPage();
		if (KEY_PRESSED(BUTTON_START)) {
			while (KEY_PRESSED(BUTTON_START));
			state = START_GAME;
			level = 1;
			brickPos = 0;
			lives = INITIAL_LIVES;
			newLevel();
			getBricks();
		}
		while (KEY_PRESSED(BUTTON_SELECT))
				state = START_GAME;
	}
}

void nextLevel() {
	REG_DISPCTL = MODE4 | BG2_ENABLE;
	frame = 0;
	loadPalette(nextLevel_palette, NEXTLEVEL_PALETTE_SIZE);
	numColors = NEXTLEVEL_PALETTE_SIZE;
	fillScreen4(0);
	while (state == NEXT_LEVEL) {
		drawImage4(0, 0, 240, 160, nextLevel_frames[(frame >> 1) % NEXTLEVEL_FRAMES]);
		waitForVblank();
		FlipPage();
		frame++;
		if (KEY_PRESSED(BUTTON_START)) {
			while (KEY_PRESSED(BUTTON_START));
			level++;
			state = PLAY_GAME;
			newLevel();
			getBricks();
		}
		while (KEY_PRESSED(BUTTON_SELECT))
			state = START_GAME;
	}
}

void wonGame() {
	REG_DISPCTL = MODE4 | BG2_ENABLE;
	frame = 0;
	loadPalette(wonGame_palette, WONGAME_PALETTE_SIZE);
	numColors = WONGAME_PALETTE_SIZE;
	while (state == WON_GAME) {
		drawImage4(0, 0, 240, 160, wonGame_frames[(frame >> 2) % WONGAME_FRAMES]);
		waitForVblank();
		FlipPage();
		frame++;
		if (KEY_PRESSED(BUTTON_START)) {
			while (KEY_PRESSED(BUTTON_START));
			state = START_GAME;
			level = 1;
			newLevel();
			getBricks();
		}
		while (KEY_PRESSED(BUTTON_SELECT))
			state = START_GAME;
	}
}


void getBricks() {
	if (level == 1) {
		for (int i = 0; i < ((int) (sizeof(level1) / sizeof(int))); i += 5) {
			Brick newBrick;
			newBrick.x = level1[i + 0];
			newBrick.y = level1[i + 1];
			newBrick.type = level1[i + 2];
			newBrick.width = level1[i + 3];
			newBrick.height = level1[i + 4];
			newBrick.color = rand() % (int) (numColors - 1);
			bricks[brickPos++] = newBrick;
		}
	} else if (level == 2) {
		for (int i = 0; i < ((int) (sizeof(level2) / sizeof(int))); i += 5) {
			Brick newBrick;
			newBrick.x = level2[i + 0];
			newBrick.y = level2[i + 1];
			newBrick.type = level2[i + 2];
			newBrick.width = level2[i + 3];
			newBrick.height = level2[i + 4];
			newBrick.color = rand() % (int) (numColors - 1);
			bricks[brickPos++] = newBrick;
		}
	} else if (level == 3) {
		for (int i = 0; i < ((int) (sizeof(level3) / sizeof(int))); i += 5) {
			Brick newBrick;
			newBrick.x = level3[i + 0];
			newBrick.y = level3[i + 1];
			newBrick.type = level3[i + 2];
			newBrick.width = level3[i + 3];
			newBrick.height = level3[i + 4];
			newBrick.color = rand() % (int) (numColors - 1);
			bricks[brickPos++] = newBrick;
		}
	} else if (level == 4) {
		for (int i = 0; i < ((int) (sizeof(level3) / sizeof(int))); i += 5) {
			Brick newBrick;
			newBrick.x = level4[i + 0];
			newBrick.y = level4[i + 1];
			newBrick.type = level4[i + 2];
			newBrick.width = level4[i + 3];
			newBrick.height = level4[i + 4];
			newBrick.color = rand() % (int) (numColors - 1);
			bricks[brickPos++] = newBrick;
		}
	}
}

void newLevel() {
	resetLevel();
	loadGamePalette();
	brickPos = 0;
}

void resetLevel() {
	player.x = INITIAL_PLAYER_X;
	player.y = INITIAL_PLAYER_Y;
	player.oldX = player.x;
	player.oldY = player.y;
	player.width = INITIAL_PLAYER_WIDTH;
	player.height = INITIAL_PLAYER_HEIGHT;
	player.color = ORANGEIDX;

	ball.width = 4;
	ball.height = 4;
	ball.color = BLUEIDX;
	ball.x = INITIAL_PLAYER_X + (INITIAL_PLAYER_HEIGHT - ball.height) / 2;
	ball.y = INITIAL_PLAYER_Y - ball.width - 5;
	ball.oldX = ball.x;
	ball.oldY = ball.y;
	ball.xDir = -1;
	ball.yDir = -1;
}

void loadGamePalette() {
	u16 colors[] = {RED, GREEN, BLUE, MAGENTA, CYAN, YELLOW, WHITE, ORANGE};
    numColors = sizeof(colors) / sizeof(colors[0]);
    loadPalette(colors, numColors);
    PALETTE[numColors] = BLACK;
    bgColorIdx = numColors++;
}