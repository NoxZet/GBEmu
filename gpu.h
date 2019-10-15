#pragma once

#include <stdint.h>
#include "memory.h"

namespace NoxGB {

struct Sprite {
	uint8_t i;
	uint8_t y;
	uint8_t x;
	uint8_t tile;
	uint8_t flags;
};

class GPU {
public:
	enum SpriteBit {
		FLAG_PRIORITY = 0x80,
		FLAG_YFLIP = 0x40,
		FLAG_XFLIP = 0x20,
		FLAG_PALETTE = 0x10
	};
	unsigned long cycle = 0;
	void fetchRenderControls();
	void renderLine(uint8_t lineY);
	void renderBg(uint8_t lineY);
	void renderSprites(uint8_t lineY);
	void pushPixel(uint8_t palette, size_t y, size_t x, uint8_t paletteColor, bool forceAlpha = true, bool hidden = false);
	// returns true if a change has been made to the screen
	bool passCycles(int cycles);
	unsigned char* buffer;
	Memory* memory;
	uint8_t scrollY = 0;
	uint8_t scrollX = 0;
	// true: 0x8000-0x8FFF; false: 0x8800-0x97FF
	bool enable = true;
	bool tileWindowSource = false;
	bool enableWindow = false;
	bool tileDataSource = true;
	bool tileMapBgSource = false;
	bool spriteSize = false;
	bool enableSprite = false;
	bool enableBgWindow = true;
	uint8_t bcgPalette = 0b00011011;
};

}