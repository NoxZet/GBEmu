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
	uint8_t y = lineY + scrollY;
	uint8_t x = scrollX;
	// yTile is saved at bits 0000xxxxx00000 to allow easy adding to xTile
	uint16_t yTile = (y & 0xF8) << 2;
	uint8_t yIn = y & 0x07;
	// Find tile data addresses for this line from tile map and save them to local
	uint8_t tileData[64] = { 0 };
	for (size_t i = 0; i < 32; i++) {
		// Address in tile map
		uint16_t tile = i << 3;
		tile |= yTile;
		uint8_t tileAddress = memory->tileMap[tileMapBgSource ? 1 : 0][tile];
		uint8_t tileDataMemory = tileAddress + (tileDataSource ? 0 : 0x0400) + (yIn << 2);
		// Load from tile data based on tile map
		tileData[i << 2] = memory->tileData[tileDataMemory];
		tileData[i << 2 + 1] = memory->tileData[tileDataMemory + 1];
	}
	for (size_t i = 0; i < 160; i++) {
		// x coordinate in the background overall
		size_t xOff = i + x;
		// x tile in the background mapping to tileData local
		uint8_t xTile = xOff & 0xFC;
		// two bit word inside the tile data value
		uint8_t xIn = x & 0x03;
		// fetches color from the tileData local, shifts right acording to xIn
		// and grabs two last bits, which represent color of given pixel
		uint8_t pixelColor = (tileData[xTile] >> (xIn << 2)) & 0x03;
		pushPixel(memory->bcgPalette, lineY, i, pixelColor);
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