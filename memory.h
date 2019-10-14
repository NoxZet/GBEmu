#pragma once

#include <stdint.h>

namespace NoxGB {

class Memory {
public:
	uint8_t interruptEnable = 0;
	uint8_t interruptFlag = 0;
	uint8_t mbcReg1 = 1;
	uint8_t mbcReg2 = 0;
	bool ramEnable = false;
	bool mbcMode = false; // false ROM banking, true RAM banking
	uint8_t* intRAM;
	uint8_t* tileData;
	uint8_t* tileMap[2];
	uint8_t* oamRAM;
	uint16_t cartSize;
	uint8_t* cartridge;
	uint16_t cartRAMSize;
	uint8_t* cartRAM;
	uint8_t readByte(uint16_t address);
	uint16_t readWord(uint16_t address);
	void writeByte(uint16_t address, uint8_t data);
	void writeWord(uint16_t address, uint16_t data);

	uint8_t LCDC = 0x91;
	uint8_t LCDCstatus = 0;
	uint8_t scroll[2] = { 0 };
	uint8_t LCDClineY = 0;
	uint8_t LCDClineCMP = 0;
	uint8_t bcgPalette = 0b00011011;
	uint8_t objPalette[2] = { 0b00011011, 0b00011011 };
	uint8_t window[2] = { 0 };
};

}