#pragma once
#include "gpu.h"

namespace NoxGB {

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

void GPU::renderLine(uint8_t lineY) {
	fetchRenderControls();
	if (enable && enableBgWindow)
		renderBg(lineY);
}

void GPU::renderBg(uint8_t lineY) {
	// Only load tiles for the current line
	uint8_t y = lineY + scrollY;
	uint16_t yTile = (y & 0xF8) << 2;
	uint8_t yIn = y & 0x07;
	// 32 tiles horizontally @ 2 bytes each = 256 pixels @ 2 bits each = 512 bits
	uint8_t tileData[64] = { 0 };
	// Find tile data addresses for this line from tile map and save them to local
	for (size_t xTile = 0; xTile < 32; xTile++) {
		// Address in tile map
		uint16_t addressInMap = xTile | yTile;
		uint16_t addressInData = (memory->tileMap[tileMapBgSource ? 1 : 0][addressInMap] << 4)
			+ (yIn << 1)
			+ (tileDataSource ? 0x800 : 0);
		// Load from tile data based on tile map
		tileData[xTile << 2] = memory->tileData[addressInData];
		tileData[xTile << 2 + 1] = memory->tileData[addressInData];
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
		uint8_t pixelColor = ((tileData[xTile] >> (7 - xIn)) | (tileData[xTile + 1] >> (6 - xIn))) & 0x03;
		pushPixel(palette, lineY, i, pixelColor);
	}
}

#define PX_R 0
#define PX_G 1
#define PX_B 2
#define PX_A 3

void GPU::pushPixel(uint8_t palette, size_t y, size_t x, uint8_t paletteColor) {
	uint8_t actualColor = (palette >> paletteColor) && 0x03;
	unsigned char* bufferOffset = buffer + ((y * 160 + x) << 2); // for RGBA, switch to '* 3' if RGB
	switch (actualColor) {
	case 0b11:
		*(bufferOffset + PX_R) = 255;
		*(bufferOffset + PX_G) = 255;
		*(bufferOffset + PX_B) = 255;
		break;
	case 0b10:
		*(bufferOffset + PX_R) = 170;
		*(bufferOffset + PX_G) = 170;
		*(bufferOffset + PX_B) = 170;
		break;
	case 0b01:
		*(bufferOffset + PX_R) = 85;
		*(bufferOffset + PX_G) = 85;
		*(bufferOffset + PX_B) = 85;
		break;
	default:
		*(bufferOffset + PX_R) = 0;
		*(bufferOffset + PX_G) = 0;
		*(bufferOffset + PX_B) = 0;
	}
}

}