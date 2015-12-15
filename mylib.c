#include "mylib.h"
#include "text.h"

unsigned short *videoBuffer = (unsigned short *)0x6000000;

void setPixel(int row, int col, u16 color) {
    videoBuffer[row * 240 + col] = color;
}

void drawRect(int row, int col, int height, int width, u16 color) {
    int r;
    for (r = 0; r < height; r++) {
        DMA[3].src = &color;
        DMA[3].dst = videoBuffer + ((row + r) * 240 + col);
        DMA[3].cnt = width | DMA_ON | DMA_SOURCE_FIXED;
    }
}

void drawImage3(int row, int col, int width, int height, const u16* image) {
    int size = 0;
    for (int r = 0; r < height; r++)
        for (int c = 0; c < width; c++)
            setPixel(row + r, col + c, image[size++]);
}

void drawImage4(int row, int col, int width, int height, const u16* image) {
    for (int r = 0; r < height; r++) {
        DMA[3].src = &image[r * width / 2];
        DMA[3].dst = videoBuffer + ((row + r) * 240 + col) / 2;
        DMA[3].cnt = (width / 2) | DMA_ON;
    }
}

void drawRect4(int row, int col, int height, int width, u8 index) {
    volatile int color = index << 8 | index;
    for (int r = 0; r < height; r++) {
        DMA[3].src = &color;
        DMA[3].dst = videoBuffer + ((row + r) * 240 + col) / 2;
        DMA[3].cnt = (width / 2) | DMA_ON | DMA_SOURCE_FIXED;
    }
}

void drawBall4(int row, int col, u8 index, u8 bgIndex) {
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if ((c == 0 || c == 3) && (r == 0 || r == 3))
                setPixel4(row + r, col + c, bgIndex);
            else
                setPixel4(row + r, col + c, index);
        }
    }
}

void waitForVblank() {
    while(SCANLINECOUNTER > 160);
    while(SCANLINECOUNTER < 160);
}

void setPixel4(int row, int col, u8 index) {
    int pixel = row * 240 + col;
    int whichShort = pixel / 2;
    if (col & 1)
        videoBuffer[whichShort] = (videoBuffer[whichShort] & 0x00FF) | (index << 8);
    else
        videoBuffer[whichShort] = (videoBuffer[whichShort] & 0xFF00) | index;
}

void FlipPage() {
    if (REG_DISPCTL & BUFFER1FLAG) {
        REG_DISPCTL = REG_DISPCTL & (~BUFFER1FLAG);
        videoBuffer = BUFFER1;
    }
    else {
        REG_DISPCTL = REG_DISPCTL | BUFFER1FLAG;
        videoBuffer = BUFFER0;
    }
}

void drawChar(int x, int y, char ch, u16 color) {
    for (int h = 0; h < 8; h++)
        for (int w = 0; w < 6; w++)
            if (fontdata_6x8[h * 6 + w + 48 * ch])
                setPixel(x + h, y + w, color);
}


void drawString(int x, int y, char *str, u16 color) {
    while (*str) {
        drawChar(x, y, *str++, color);
        y += 6;
    }
}

void drawChar4(int x, int y, char ch, u8 index) {
    for (int h = 0; h < 8; h++)
        for (int w = 0; w < 6; w++)
            if (fontdata_6x8[h * 6 + w + 48 * ch])
                setPixel4(x + h, y + w, index);
}


void drawString4(int x, int y, char *str, u8 index) {
    while (*str) {
        drawChar4(x, y, *str++, index);
        y += 6;
    }
}

void fillScreen3(volatile u16 color) {
    DMA[3].src = &color;
    DMA[3].dst = videoBuffer;
    DMA[3].cnt = 38400 | DMA_SOURCE_FIXED | DMA_ON;
}

void fillScreen4(u8 index) {
    volatile u16 color = index | (index << 8);
    DMA[3].src = &color;
    DMA[3].dst = videoBuffer;
    DMA[3].cnt = 19200 | DMA_SOURCE_FIXED | DMA_ON;
}

void loadPalette(const u16* colors, u8 numColors) {
    for(int i = 0; i < numColors; i++) {
        PALETTE[i] = colors[i];
    }
}

void clearScreen() {
    drawRect(0, 0, 240, 160, BLACK);
}