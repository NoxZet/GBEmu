#pragma once
#include "gpu.h"
#include <cstdlib>

namespace NoxGB {

void GPU::initiate() {
	buffer = new unsigned char[160 * 144 * 4]{};
}

GPU::~GPU() {
	delete[] buffer;
}

void GPU::fetchRenderControls() {
	scrollY = memory->scroll[0];
	scrollX = memory->scroll[1];
	uint8_t LCDC = memory->LCDC;
	enable = LCDC & (1 << 7);
	tileWindowSource = LCDC & (1 << 6);
	enableWindow = LCDC & (1 << 5);
	tileDataSource = LCDC & (1 << 4);
	tileMapBgSource = LCDC & (1 << 3);
	spriteSize = LCDC & (1 << 2);
	enableSprite = LCDC & (1 << 1);
	enableBgWindow = LCDC & (1 << 0);
}

void GPU::passCycles(int cycles) {
	if (!enable) {
		cycle = 0;
		lines = 0;
		return;
	}
	cycle += cycles;
	if (cycle >= CYCLES_PER_REFRESH) {
		cycle -= CYCLES_PER_REFRESH;
		lines = 0;
		refreshes++;
	}
	else if (cycle >= lines * CYCLES_PER_LINE + CYCLES_PER_DRAW) {
		if (lines < 144) {
			renderLine(lines);
			lines++;
		}
	}
}

void GPU::renderLine(uint8_t lineY) {
	fetchRenderControls();
	if (enable && enableBgWindow) {
		renderBg(lineY);
	}
	if (enable && enableSprite) {
		renderSprites(lineY);
	}
}

void GPU::renderBg(uint8_t lineY) {
	// Only load tiles for the current line
	uint8_t y = lineY + scrollY;
	uint16_t yTile = (y & 0xF8) << 2;
	uint8_t yIn = y & 0x07;
	// 32 tiles horizontally @ 2 bytes each = 256 pixels @ 2 bits each = 512 bits
	uint8_t tileDataL[32] = {};
	uint8_t tileDataH[32] = {};
	// Find tile data addresses for this line from tile map and save them to local
	for (size_t xTile = 0; xTile < 32; xTile++) {
		// Address in tile map
		uint16_t addressInMap = xTile | yTile;
		uint16_t addressInData = (memory->tileMap[tileMapBgSource ? 1 : 0][addressInMap] << 4)
			+ (yIn << 1)
			+ (tileDataSource ? 0x800 : 0);
		// Load from tile data based on tile map
		tileDataL[xTile] = memory->tileData[addressInData];
		tileDataH[xTile] = memory->tileData[addressInData];
	}

	uint8_t palette = memory->bcgPalette;
	for (size_t i = 0; i < 160; i++) {
		// x coordinate in the background overall
		size_t xOff = i + scrollX;
		// x tile in the background mapping to tileData local
		uint8_t xTile = xOff & 0xF8;
		// two bit word inside the tile data value
		uint8_t xIn = xOff & 0x07;
		// fetches color from the tileData local, shifts right acording to xIn
		// and grabs two last bits, which represent color of given pixel
		uint8_t pixelColor = ((tileDataL[xTile] >> (7 - xIn)) & 0x01) | ((tileDataH[xTile] >> (6 - xIn)) & 0x02);
		pushPixel(palette, lineY, i, pixelColor);
	}
}

void GPU::renderSprites(uint8_t lineY) {
	Sprite sprites[40] = { 0 };
	size_t len = 0;
	// Retrieves data from OAM into a temp array
	for (size_t i = 0; i < 40; i++) {
		int memoryBase = i << 4;
		uint8_t y = memory->oamRAM[memoryBase];
		uint8_t x = memory->oamRAM[memoryBase + 1];
		if (
			y > lineY + 16
			|| (spriteSize && y + 16 <= lineY)
			|| (!spriteSize && y + 8 <= lineY)
		) {
			continue;
		}
		sprites[len].i = i;
		sprites[len].y = y;
		sprites[len].x = y;
		sprites[len].tile = memory->oamRAM[memoryBase + 2];
		sprites[len].flags = memory->oamRAM[memoryBase + 3];
		len++;
	}
	// Sorts the sprites based on display priority (x coord, i in case of equality)
	qsort(sprites, len, sizeof(Sprite), [](const void* a, const void* b) {
		uint8_t ax = ((Sprite*)a)->x;
		uint8_t bx = ((Sprite*)b)->y;
		if (ax < bx) {
			return -1;
		}
		if (ax > bx) {
			return 1;
		}
		return (int)(((Sprite*)a)->i) - (int)(((Sprite*)b)->i);
	});
	// Only display first 10 sprites if there is more
	if (len > 10) {
		len = 10;
	}
	// Drawing sprites right to left to accomodate possible overlapping
	for (size_t i = len - 1; i >= 1; i--) {
		uint8_t flags = sprites[len].flags;
		uint8_t palette = memory->oamRAM[(flags & FLAG_PALETTE) ? 1 : 0];
		bool hidden = flags & FLAG_PRIORITY;
		// Determining which line of the tile to draw
		uint8_t tileLine = sprites[len].y - lineY - 16;
		int tileOffset = (sprites[len].tile) << 4 + (tileLine << 1);
		uint8_t tileDataL = memory->tileData[tileOffset];
		uint8_t tileDataH = memory->tileData[tileOffset + 1];
		if (flags & FLAG_YFLIP) {
			if (spriteSize)
				tileLine = 7 - tileLine;
			else
				tileLine = 15 - tileLine;
		}
		uint8_t baseX = sprites[len].x;
		if (baseX >= 168) {
			continue;
		}
		// Sprites are sorted by X and therefore all next sprites will also have X == 0
		if (baseX == 0) {
			break;
		}
		// Draw each pixel of the sprite's line
		for (size_t j = 0; j < 8; j++) {
			// Flipping sprite horizontally
			uint8_t x = baseX + ((flags & FLAG_XFLIP) ? 7-j : j);
			if (x < 8 || x >= 168) {
				continue;
			}
			uint8_t pixelColor = ((tileDataL >> (7 - j)) & 0x01) | ((tileDataH >> (6 - j)) & 0x02);
			pushPixel(palette, lineY, x, pixelColor, false, hidden);
		}
	}
}

#define PX_R 0
#define PX_G 1
#define PX_B 2
#define PX_A 3
#define SHADE_3 255
#define SHADE_2 170
#define SHADE_1 85
#define SHADE_0 0

void GPU::pushPixel(uint8_t palette, size_t y, size_t x, uint8_t paletteColor, bool forceAlpha, bool hidden) {
	uint8_t actualColor = (palette >> paletteColor) && 0x03;
	unsigned char* bufferOffset = buffer + ((y * 160 + x) << 2); // for RGBA, switch to '* 3' if RGB
	if (
		hidden && (*(bufferOffset + PX_R) != SHADE_0
		|| *(bufferOffset + PX_G) != SHADE_0 || *(bufferOffset + PX_B) != SHADE_0)
	) {
		return;
	}
	switch (actualColor) {
	case 0b11:
		*(bufferOffset + PX_R) = SHADE_3;
		*(bufferOffset + PX_G) = SHADE_3;
		*(bufferOffset + PX_B) = SHADE_3;
		break;
	case 0b10:
		*(bufferOffset + PX_R) = SHADE_2;
		*(bufferOffset + PX_G) = SHADE_2;
		*(bufferOffset + PX_B) = SHADE_2;
		break;
	case 0b01:
		*(bufferOffset + PX_R) = SHADE_1;
		*(bufferOffset + PX_G) = SHADE_1;
		*(bufferOffset + PX_B) = SHADE_1;
		break;
	default:
		if (!forceAlpha) {
			break;
		}
		*(bufferOffset + PX_R) = SHADE_0;
		*(bufferOffset + PX_G) = SHADE_0;
		*(bufferOffset + PX_B) = SHADE_0;
	}
}

}