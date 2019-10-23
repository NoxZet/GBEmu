#pragma once

#include "memory.h"

namespace NoxGB {

void Memory::initiate() {
	cartSize = 0x8000;
	cartridge = new uint8_t[cartSize]{};
	cartRAMSize = 0x2000;
	cartRAM = new uint8_t[cartRAMSize]{};

	intRAM = new uint8_t[0x2000]{};
	tileData = new uint8_t[0xC00]{};
	for (int i = 0; i < 2; i++) {
		tileMap[i] = new uint8_t[0x400]{};
	}
	oamRAM = new uint8_t[0xA0]{ 0 };
}

Memory::~Memory() {
	if (cartridge) delete[] cartridge;
	if (cartRAM) delete[] cartRAM;
	if (intRAM) delete[] intRAM;
	if (tileData) delete[] tileData;
	for (int i = 0; i < 2; i++) {
		if (tileMap[i]) delete[] tileMap[i];
	}
	if (oamRAM) delete[] oamRAM;
}

uint8_t Memory::readByte(uint16_t address) {
	if (address < 0x4000)
		return cartridge[address];
	else if (address < 0x8000) {
		// lowest 5 bits from mbcReg1, if equal to 0, use 1
		// if mbcMode is false, lowest 2 bits from mbcReg2 left shift 5 and add
		uint8_t mbcTotal =
			(((mbcReg1 & 0x03) != 0 ? mbcReg1 : 0x01) & 0x1F)
			| (mbcMode ? 0 : ((mbcReg2 & 0x03) << 5));
		long dest = (address - 0x4000) + mbcTotal * 0x4000;
		return cartridge[dest % cartSize];
	}
	else if (address < 0x9800)
		return tileData[address - 0x8000];
	else if (address < 0x9C00)
		return tileMap[0][address - 0x9800];
	else if (address < 0xA000)
		return tileMap[1][address - 0x9C00];
	else if (address < 0xC000) {
		// if mbcMode is true, lowest 2 bits from mbcReg2
		uint8_t mbcTotal = mbcMode ? (mbcReg2 & 0x03) : 0;
		long dest = (address - 0xA000) + mbcTotal * 0x2000;
		return cartRAM[dest % cartRAMSize];
	}
	else if (address < 0xFE00)
		return intRAM[(address - 0xC000) % 0x2000];
	else if (address < 0xFEA0)
		return oamRAM[address - 0xFE00];
	else if (address < 0xFF00)
		return 0;
	// IO, mostly todo
	else if (address == 0xFF0F)
		return interruptFlag;
	else if (address == 0xFF40)
		return LCDC;
	else if (address == 0xFF41)
		return LCDCstatus;
	else if (address == 0xFF42)
		return scroll[0];
	else if (address == 0xFF43)
		return scroll[1];
	else if (address == 0xFF44)
		return LCDClineY;
	else if (address == 0xFF45)
		return LCDClineCMP;
	else if (address == 0xFF47)
		return bcgPalette;
	else if (address == 0xFF48)
		return objPalette[0];
	else if (address == 0xFF49)
		return objPalette[1];
	else if (address == 0xFF4A)
		return window[0];
	else if (address == 0xFF4B)
		return window[1];
	else if (address == 0xFFFF)
		return interruptEnable;
	else if (address < 0xFF80)
		return 0;
	else
		return intRAM[address - 0xE000];
}

uint16_t Memory::readWord(uint16_t address) {
	// This function fails on big-endian machine
	uint16_t output;
	uint8_t* pointer = (uint8_t*)&output;
	pointer[0] = readByte(address);
	pointer[1] = readByte(address + 1);
	return output;
}

void Memory::writeByte(uint16_t address, uint8_t data) {
	if (address < 0x2000)
		ramEnable = (data == 0) ? false : true;
	else if (address < 0x4000)
		mbcReg1 = data & 0x1F;
	else if (address < 0x6000)
		mbcReg2 = data & 0x03;
	else if (address < 0x8000)
		ramEnable = (data == 0) ? false : true;
	else if (address < 0x9800)
		tileData[address - 0x8000] = data;
	else if (address < 0x9C00)
		tileMap[0][address - 0x9800] = data;
	else if (address < 0xA000)
		tileMap[1][address - 0x9C00] = data;
	else if (address < 0xC000) {
		uint8_t mbcTotal = mbcMode ? (mbcReg2 & 0x03) : 0;
		long dest = (address - 0xA000) + mbcTotal * 0x2000;
		cartRAM[dest % cartRAMSize] = data;
	}
	else if (address < 0xFE00)
		intRAM[(address - 0xC000) % 0x2000] = data;
	else if (address < 0xFEA0)
		oamRAM[address - 0xFE00] = data;
	else if (address < 0xFF00)
		;
	// IO, mostly todo
	else if (address == 0xFF0F)
		interruptFlag = data;
	else if (address == 0xFF40)
		LCDC = data;
	else if (address == 0xFF41)
		LCDCstatus = data;
	else if (address == 0xFF42)
		scroll[0] = data;
	else if (address == 0xFF43)
		scroll[1] = data;
	else if (address == 0xFF44)
		LCDClineY = 0;
	else if (address == 0xFF45)
		LCDClineCMP = data;
	else if (address == 0xFF47)
		bcgPalette = data;
	else if (address == 0xFF48)
		objPalette[0] = data;
	else if (address == 0xFF49)
		objPalette[1] = data;
	else if (address == 0xFF4A)
		window[0] = data;
	else if (address == 0xFF4B)
		window[1] = data;
	else if (address == 0xFFFF)
		interruptEnable = data;
	else if (address < 0xFF80)
		;
	else
		intRAM[address - 0xE000] = data;
}

void Memory::writeWord(uint16_t address, uint16_t data) {
	// This function fails on big-endian machine
	uint8_t* pointer = (uint8_t*)&data;
	writeByte(address, pointer[0]);
	writeByte(address + 1, pointer[1]);
}

}