// Nathan Dass

typedef unsigned char 	u8;
typedef unsigned short 	u16;
typedef unsigned int 	u32;

#define INITIAL_LIVES			3
#define INITIAL_PLAYER_X		100
#define INITIAL_PLAYER_Y		143
#define INITIAL_PLAYER_HEIGHT	30
#define INITIAL_PLAYER_WIDTH	5

#define REG_DISPCTL *(unsigned short *) 0x4000000
#define MODE3 3
#define MODE4 4

#define BUFFER0 (unsigned short *) 0x6000000
#define BUFFER1 (unsigned short *) 0x600A000

#define BUFFER1FLAG (1<<4)

#define PALETTE ((unsigned short *) 0x5000000)

#define BG2_ENABLE (1 << 10)
#define COLOR(r, g, b) ((r) | (g) << 5 | (b) << 10)
#define RED COLOR(31, 0, 0)
#define GREEN COLOR(0, 31, 0)
#define BLUE COLOR(0, 0, 31)
#define MAGENTA COLOR(31, 0, 31)
#define CYAN COLOR(0, 31, 31)
#define WHITE COLOR(31, 31, 31)
#define YELLOW COLOR(31, 31, 0)
#define LTGRAY COLOR(20, 20, 20)
#define ORANGE COLOR(31, 16, 0)
#define BLACK 0

#define SCANLINECOUNTER *(volatile u16 *)0x4000006
extern unsigned short *videoBuffer;

// Buttons
#define BUTTONS 			*(volatile unsigned int*) 0x4000130
#define BUTTON_A			(1 << 0)	// Z
#define BUTTON_B			(1 << 1)	// X
#define BUTTON_SELECT		(1 << 2)	// Backspace
#define BUTTON_START		(1 << 3)	// Enter
#define BUTTON_RIGHT		(1 << 4)	// Right Arrow
#define BUTTON_LEFT			(1 << 5)	// Left Arrow
#define BUTTON_UP			(1 << 6)	// Up Arrow
#define BUTTON_DOWN			(1 << 7)	// Down Arrow
#define BUTTON_R			(1 << 8)	// S
#define BUTTON_L			(1 << 9)	// A
#define KEY_PRESSED(key)	(~(BUTTONS) & key)

// DMA
#define REG_DMA0SAD         *(const volatile u32*) 0x40000B0
#define REG_DMA0DAD         *(volatile u32*) 0x40000B4
#define REG_DMA0CNT         *(volatile u32*) 0x40000B8

// Channel 1
#define REG_DMA1SAD         *(const volatile u32*) 0x40000BC
#define REG_DMA1DAD         *(volatile u32*) 0x40000C0
#define REG_DMA1CNT         *(volatile u32*) 0x40000C4

// Channel 2
#define REG_DMA2SAD         *(const volatile u32*) 0x40000C8
#define REG_DMA2DAD         *(volatile u32*) 0x40000CC
#define REG_DMA2CNT         *(volatile u32*) 0x40000D0

// Channel 3
#define REG_DMA3SAD         *(const volatile u32*) 0x40000D4
#define REG_DMA3DAD         *(volatile u32*) 0x40000D8
#define REG_DMA3CNT         *(volatile u32*) 0x40000DC

typedef struct
{
	const volatile void *src;
	const volatile void *dst;
	u32                  cnt;
} DMA_CONTROLLER;

#define DMA ((volatile DMA_CONTROLLER *) 0x040000B0)

// Defines
#define DMA_CHANNEL_0 0
#define DMA_CHANNEL_1 1
#define DMA_CHANNEL_2 2
#define DMA_CHANNEL_3 3

#define DMA_DESTINATION_INCREMENT (0 << 21)
#define DMA_DESTINATION_DECREMENT (1 << 21)
#define DMA_DESTINATION_FIXED (2 << 21)
#define DMA_DESTINATION_RESET (3 << 21)

#define DMA_SOURCE_INCREMENT (0 << 23)
#define DMA_SOURCE_DECREMENT (1 << 23)
#define DMA_SOURCE_FIXED (2 << 23)

#define DMA_REPEAT (1 << 25)

#define DMA_16 (0 << 26)
#define DMA_32 (1 << 26)

#define DMA_NOW (0 << 28)
#define DMA_AT_VBLANK (1 << 28)
#define DMA_AT_HBLANK (2 << 28)
#define DMA_AT_REFRESH (3 << 28)

#define DMA_IRQ (1 << 30)
#define DMA_ON (1 << 31)

// Prototypes
void setPixel(int row, int col, unsigned short color);
void drawRect(int row, int col, int height, int width, u16 color);
void drawImage3(int x, int y, int width, int height, const u16* image);
void drawImage4(int x, int y, int width, int height, const u16* image);
void waitForVblank();
void drawRect4(int row, int col, int height, int width, u8 index);
void drawBall4(int row, int col, u8 index, u8 bgIndex);
void setPixel4(int row, int col, u8 index);
void FlipPage();
void drawChar(int row, int col, char ch, u16 color);
void drawString(int row, int col, char *str, u16 color);
void drawChar4(int row, int col, char ch, u8 index);
void drawString4(int row, int col, char *str, u8 index);
void fillScreen3(volatile u16 color);
void fillScreen4(u8 index);
void loadPalette(const u16 *color, u8 numColors);
void delay(int n);
void clearScreen();