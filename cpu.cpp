#pragma once
#include "cpu.h"
#include <intrin.h>

namespace NoxGB {

void CPU::runInstruction() {
	regPC = regPCnext;
	regPCnext = regPC;
	uint8_t inst = memory->readByte(regPC);
	unsigned int cycles = 0;
	// CB instructions
	if (inst == 0xCB) {
	}
	// Top quarter
	else if (inst < 0x40) {
		switch (inst) {
			case 0x00: regPCnext += 1; cycles = 1; break;
			// TODO: STOP
			case 0x10: regPCnext += 2; cycles = 1; break;
			// LD RP,imm16
			case 0x01: regPCnext += 3; cycles = 3; LDpairn(REG_BC, memory->readWord(regPC + 1)); break;
			case 0x11: regPCnext += 3; cycles = 3; LDpairn(REG_DE, memory->readWord(regPC + 1)); break;
			case 0x21: regPCnext += 3; cycles = 3; LDpairn(REG_HL, memory->readWord(regPC + 1)); break;
			case 0x31: regPCnext += 3; cycles = 3; LDSPdir(memory->readWord(regPC + 1)); break;
			// LD (RP),A
			case 0x02: regPCnext += 1; cycles = 2; LDindirPairA(REG_BC); break;
			case 0x12: regPCnext += 1; cycles = 2; LDindirPairA(REG_DE); break;
			case 0x22: regPCnext += 1; cycles = 2; LDIindirA(); break;
			case 0x32: regPCnext += 1; cycles = 2; LDDindirA(); break;
			// LD A,(RP)
			case 0x0A: regPCnext += 1; cycles = 2; LDaIndirPair(REG_BC); break;
			case 0x1A: regPCnext += 1; cycles = 2; LDaIndirPair(REG_DE); break;
			case 0x2A: regPCnext += 1; cycles = 2; LDIaIndir(); break;
			case 0x3A: regPCnext += 1; cycles = 2; LDDaIndir(); break;
			// ADD HL,RP
			case 0x09: regPCnext += 1; cycles = 2; addHLpair(REG_BC); break;
			case 0x19: regPCnext += 1; cycles = 2; addHLpair(REG_DE); break;
			case 0x29: regPCnext += 1; cycles = 2; addHLpair(REG_HL); break;
			case 0x39: regPCnext += 1; cycles = 2; addHLSP(); break;
			// INC RP
			case 0x03: regPCnext += 1; cycles = 2; incPair(REG_BC); break;
			case 0x13: regPCnext += 1; cycles = 2; incPair(REG_DE); break;
			case 0x23: regPCnext += 1; cycles = 2; incPair(REG_HL); break;
			case 0x33: regPCnext += 1; cycles = 2; incSP(); break;
			// DEC RP
			case 0x0B: regPCnext += 1; cycles = 2; decPair(REG_BC); break;
			case 0x1B: regPCnext += 1; cycles = 2; decPair(REG_DE); break;
			case 0x2B: regPCnext += 1; cycles = 2; decPair(REG_HL); break;
			case 0x3B: regPCnext += 1; cycles = 2; decSP(); break;
			// INC R
			case 0x04: regPCnext += 1; cycles = 1; incR(REG_B); break;
			case 0x0C: regPCnext += 1; cycles = 1; incR(REG_C); break;
			case 0x14: regPCnext += 1; cycles = 1; incR(REG_D); break;
			case 0x1C: regPCnext += 1; cycles = 1; incR(REG_E); break;
			case 0x24: regPCnext += 1; cycles = 1; incR(REG_H); break;
			case 0x2C: regPCnext += 1; cycles = 1; incR(REG_L); break;
			case 0x34: regPCnext += 1; cycles = 3; incIndir(); break;
			case 0x3C: regPCnext += 1; cycles = 1; incR(REG_A); break;
			// DEC R
			case 0x05: regPCnext += 1; cycles = 1; decR(REG_B); break;
			case 0x0D: regPCnext += 1; cycles = 1; decR(REG_C); break;
			case 0x15: regPCnext += 1; cycles = 1; decR(REG_D); break;
			case 0x1D: regPCnext += 1; cycles = 1; decR(REG_E); break;
			case 0x25: regPCnext += 1; cycles = 1; decR(REG_H); break;
			case 0x2D: regPCnext += 1; cycles = 1; decR(REG_L); break;
			case 0x35: regPCnext += 1; cycles = 3; decIndir(); break;
			case 0x3D: regPCnext += 1; cycles = 1; decR(REG_A); break;
			// LD R,imm8
			case 0x06: regPCnext += 2; cycles = 2; LDrn(REG_B, memory->readByte(regPC + 1)); break;
			case 0x0E: regPCnext += 2; cycles = 2; LDrn(REG_C, memory->readByte(regPC + 1)); break;
			case 0x16: regPCnext += 2; cycles = 2; LDrn(REG_D, memory->readByte(regPC + 1)); break;
			case 0x1E: regPCnext += 2; cycles = 2; LDrn(REG_E, memory->readByte(regPC + 1)); break;
			case 0x26: regPCnext += 2; cycles = 2; LDrn(REG_H, memory->readByte(regPC + 1)); break;
			case 0x2E: regPCnext += 2; cycles = 2; LDrn(REG_L, memory->readByte(regPC + 1)); break;
			case 0x36: regPCnext += 2; cycles = 2; LDindirN(memory->readByte(regPC + 1)); break;
			case 0x3E: regPCnext += 2; cycles = 2; LDrn(REG_A, memory->readByte(regPC + 1)); break;
			// A rotates
			case 0x07: regPCnext += 1; cycles = 1; rotLcarry(REG_A); break;
			case 0x17: regPCnext += 1; cycles = 1; rotL(REG_A); break;
			case 0x0F: regPCnext += 1; cycles = 1; rotRcarry(REG_A); break;
			case 0x1F: regPCnext += 1; cycles = 1; rotR(REG_A); break;
			// JR XX
			case 0x18: regPCnext += 2; cycles = 2; jumpRel(memory->readByte(regPC + 1)); break;
			case 0x20: regPCnext += 2; cycles = 2; jumpRelReset(memory->readByte(regPC + 1), FLAG_ZERO); break;
			case 0x30: regPCnext += 2; cycles = 2; jumpRelReset(memory->readByte(regPC + 1), FLAG_CARRY); break;
			case 0x28: regPCnext += 2; cycles = 2; jumpRelSet(memory->readByte(regPC + 1), FLAG_ZERO); break;
			case 0x38: regPCnext += 2; cycles = 2; jumpRelSet(memory->readByte(regPC + 1), FLAG_CARRY); break;
			// Misc first quater
			case 0x08: regPCnext += 3; cycles = 5; LDSPdir(memory->readWord(regPC + 1)); break;
			case 0x27: regPCnext += 1; cycles = 1; daa(); break;
			case 0x2F: regPCnext += 1; cycles = 1; complementA(); break;
			case 0x37: regPCnext += 1; cycles = 1; setCarry(); break;
			case 0x3F: regPCnext += 1; cycles = 1; complementCarry(); break;
		}
	}
	// Second quarter - 8-bit LD
	else if (inst < 0x80) {
		regPCnext += 1;
		// LD R,X
		if ((inst & 0x38) != 0x30) {
			// LD R,(HL)
			if ((inst & 0x07) == 0x06) {
				cycles = 2; LDrIndir((RegisterID)((inst & 0x38) >> 3));
			}
			// LD R,R
			else {
				cycles = 1; LDrr((RegisterID)((inst & 0x38) >> 3), (RegisterID)(inst & 0x07));
			}
		}
		// TODO: HALT
		else if (inst == 0x76) {
			cycles = 1;
		}
		// LD (HL),R
		else {
			cycles = 2; LDindirR((RegisterID)(inst & 0x07));
		}
	}
	// Fifth eighth - ADD, ADC, SUB, SBC
	else if (inst < 0xA0) {
		regPCnext += 1;
		// (HL)
		if ((inst & 0x07) == 0x06) {
			cycles = 2; addAr((RegisterID)(inst & 0x07), inst & 0x08, inst & 0x10);
		}
		// R
		else {
			cycles = 1; addA((RegisterID)(inst & 0x07), inst & 0x08, inst & 0x10);
		}
	}
	// Sixth eighth - AND, XOR, OR, CP
	else if (inst < 0xC0) {
		regPCnext += 1;
		uint8_t op = inst & 0x18;
		// (HL)
		if ((inst & 0x07) == 0x06) {
			cycles = 2;
			switch (op) {
				case 0x00: andAindir(); break;
				case 0x08: xorAindir(); break;
				case 0x10: orAindir(); break;
				case 0x18: cmpAindir(); break;
			}
		}
		else {
			cycles = 1;
			switch (op) {
				case 0x00: andAr((RegisterID)(inst & 0x07)); break;
				case 0x08: xorAr((RegisterID)(inst & 0x07)); break;
				case 0x10: orAr((RegisterID)(inst & 0x07)); break;
				case 0x18: cmpAr((RegisterID)(inst & 0x07)); break;
			}
		}
	}
	// Fourth fourth
	else {
		switch (inst) {
			// RET X
			case 0xC0: regPCnext += 1; cycles = 2; retReset(FLAG_ZERO); break;
			case 0xD0: regPCnext += 1; cycles = 2; retReset(FLAG_CARRY); break;
			case 0xC8: regPCnext += 1; cycles = 2; retSet(FLAG_ZERO); break;
			case 0xD8: regPCnext += 1; cycles = 2; retSet(FLAG_CARRY); break;
			case 0xC9: regPCnext += 1; cycles = 2; ret(); break;
			case 0xD9: regPCnext += 1; cycles = 2; retInterrupt(); break;
			// High RAM
			case 0xE0: regPCnext += 2; cycles = 3; LDhighDirA(memory->readByte(regPC + 1)); break;
			case 0xF0: regPCnext += 2; cycles = 3; LDaHighDir(memory->readByte(regPC + 1)); break;
			case 0xE2: regPCnext += 2; cycles = 2; LDhighCA(); break;
			case 0xF2: regPCnext += 2; cycles = 2; LDaHighC(); break;
			// POP
			case 0xC1: regPCnext += 1; cycles = 3; popPair(REG_BC); break;
			case 0xD1: regPCnext += 1; cycles = 3; popPair(REG_DE); break;
			case 0xE1: regPCnext += 1; cycles = 3; popPair(REG_HL); break;
			case 0xF1: regPCnext += 1; cycles = 3; popPair(REG_FA); break;
			// PUSH
			case 0xC5: regPCnext += 1; cycles = 3; pushPair(REG_BC); break;
			case 0xD5: regPCnext += 1; cycles = 3; pushPair(REG_DE); break;
			case 0xE5: regPCnext += 1; cycles = 3; pushPair(REG_HL); break;
			case 0xF5: regPCnext += 1; cycles = 3; pushPair(REG_FA); break;
			// JP X,a16
			case 0xC2: regPCnext += 3; cycles = 3; jumpReset(memory->readWord(regPC + 1), FLAG_ZERO); break;
			case 0xD2: regPCnext += 3; cycles = 3; jumpReset(memory->readWord(regPC + 1), FLAG_CARRY); break;
			case 0xCA: regPCnext += 3; cycles = 3; jumpSet(memory->readWord(regPC + 1), FLAG_ZERO); break;
			case 0xDA: regPCnext += 3; cycles = 3; jumpSet(memory->readWord(regPC + 1), FLAG_CARRY); break;
			case 0xC3: regPCnext += 3; cycles = 3; jump(memory->readWord(regPC + 1)); break;
			// CALL X,a16
			case 0xC4: regPCnext += 2; cycles = 3; callReset(memory->readWord(regPC + 1), FLAG_ZERO); break;
			case 0xD4: regPCnext += 2; cycles = 3; callReset(memory->readWord(regPC + 1), FLAG_CARRY); break;
			case 0xCC: regPCnext += 2; cycles = 3; callSet(memory->readWord(regPC + 1), FLAG_ZERO); break;
			case 0xDC: regPCnext += 2; cycles = 3; callSet(memory->readWord(regPC + 1), FLAG_CARRY); break;
			case 0xCD: regPCnext += 2; cycles = 3; call(memory->readWord(regPC + 1)); break;
			// 8-bit immediate arithmetic
			case 0xC6: regPCnext += 2; cycles = 2; addA(memory->readByte(regPC + 1)); break;
			case 0xCE: regPCnext += 2; cycles = 2; addA(memory->readByte(regPC + 1), true); break;
			case 0xD6: regPCnext += 2; cycles = 2; subA(memory->readByte(regPC + 1)); break;
			case 0xDE: regPCnext += 2; cycles = 2; subA(memory->readByte(regPC + 1), true); break;
			case 0xE6: regPCnext += 2; cycles = 2; andA(memory->readByte(regPC + 1)); break;
			case 0xEE: regPCnext += 2; cycles = 2; xorA(memory->readByte(regPC + 1)); break;
			case 0xF6: regPCnext += 2; cycles = 2; orA(memory->readByte(regPC + 1)); break;
			case 0xFE: regPCnext += 2; cycles = 2; cmpA(memory->readByte(regPC + 1)); break;
			// SP arithmetics and loads
			case 0xE8: regPCnext += 2; cycles = 4; addSPn(memory->readByte(regPC + 1)); break;
			case 0xF8: regPCnext += 2; cycles = 3; LDHLSPn(memory->readByte(regPC + 1)); break;
			case 0xF9: regPCnext += 1; cycles = 2; LDSPHL(); break;
			// LD A;(a16)
			case 0xEA: regPCnext += 3; cycles = 4; LDdirA(memory->readWord(regPC + 1)); break;
			case 0xFA: regPCnext += 3; cycles = 4; LDaDir(memory->readWord(regPC + 1)); break;
			// Interrupt set
			case 0xF3: regPCnext += 1; cycles = 1; interruptSet(false); break;
			case 0xFB: regPCnext += 1; cycles = 1; interruptSet(true); break;
			// Restart
			case 0xC7: regPCnext += 1; cycles = 4; restart(0x00); break;
			case 0xCF: regPCnext += 1; cycles = 4; restart(0x08); break;
			case 0xD7: regPCnext += 1; cycles = 4; restart(0x10); break;
			case 0xDF: regPCnext += 1; cycles = 4; restart(0x18); break;
			case 0xE7: regPCnext += 1; cycles = 4; restart(0x20); break;
			case 0xEF: regPCnext += 1; cycles = 4; restart(0x28); break;
			case 0xF7: regPCnext += 1; cycles = 4; restart(0x30); break;
			case 0xFF: regPCnext += 1; cycles = 4; restart(0x38); break;
			// JP (HL)
			case 0xE9: regPCnext += 1; cycles = 4; jumpHL(); break;
		}
	}
}

uint16_t CPU::regPairWord(RegisterPair rp) {
	// This function fails on big-endian machine
	uint16_t output;
	uint8_t* pointer = (uint8_t*)&output;
	pointer[0] = reg[((RegisterID)rp) + 1];
	pointer[1] = reg[(RegisterID)rp];
	return output;
}

void CPU::regPairWordSave(RegisterPair rp, uint16_t n) {
	// This function fails on big-endian machine
	uint8_t* pointer = (uint8_t*)&n;
	reg[((RegisterID)rp) + 1] = pointer[0];
	reg[(RegisterID)rp] = pointer[1];
}

uint8_t CPU::swap(uint8_t value) {
	return ((value & 0x0F) << 4) | ((value & 0xF0) >> 4);
}

void CPU::LDrr(RegisterID r1, RegisterID r2) {
	reg[r1] = reg[r2];
}

void CPU::LDrn(RegisterID r, uint8_t imm) {
	reg[r] = imm;
}

void CPU::LDrIndir(RegisterID r) {
	reg[r] = memory->readByte(regPairWord(REG_HL));
}

void CPU::LDindirR(RegisterID r) {
	memory->writeByte(regPairWord(REG_HL), reg[r]);
}

void CPU::LDindirN(uint8_t imm) {
	memory->writeByte(regPairWord(REG_HL), imm);
}

void CPU::LDaIndirPair(RegisterPair rp) {
	reg[REG_A] = memory->readByte(regPairWord(rp));
}

void CPU::LDaDir(uint16_t imm) {
	reg[REG_A] = memory->readByte(imm);
}

void CPU::LDindirPairA(RegisterPair rp) {
	memory->writeByte(regPairWord(rp), reg[REG_A]);
}

void CPU::LDdirA(uint16_t imm) {
	memory->writeByte(imm, reg[REG_A]);
}

void CPU::LDaHighC() {
	reg[REG_A] = memory->readByte(0xFF00 + reg[REG_C]);
}

void CPU::LDhighCA() {
	memory->writeByte(0xFF00 + reg[REG_C], reg[REG_A]);
}

void CPU::LDaHighDir(uint8_t imm) {
	reg[REG_A] = memory->readByte(0xFF00 + imm);
}

void CPU::LDhighDirA(uint8_t imm) {
	memory->writeByte(0xFF00 + imm, reg[REG_A]);
}

void CPU::LDpairn(RegisterPair rp, uint16_t imm) {
	regPairWordSave(rp, imm);
}

void CPU::LDSPHL() {
	regSP = regPairWord(REG_HL);
}

void CPU::LDHLSPn(uint8_t imm) {
	uint16_t value;
	if (imm > 127) {
		value = regSP - (~imm + 1);
	}
	else {
		value = regSP + imm;
	}
	regPairWordSave(REG_HL, value);
}

void CPU::LDSPdir(uint16_t imm) {
	memory->writeWord(imm, regSP);
}

void CPU::pushPair(RegisterPair rp) {
	regSP--;
	memory->writeWord(regSP, regPairWord(rp));
	regSP--;
}

void CPU::popPair(RegisterPair rp) {
	regSP++;
	regPairWordSave(rp, memory->readWord(regSP));
	regSP++;
}

void CPU::LDDaIndir() {
	reg[REG_A] = memory->readByte(regPairWord(REG_HL));
	decPair(REG_HL);
}

void CPU::LDDindirA() {
	memory->writeByte(regPairWord(REG_HL), reg[REG_A]);
	decPair(REG_HL);
}

void CPU::LDIaIndir() {
	reg[REG_A] = memory->readByte(regPairWord(REG_HL));
	incPair(REG_HL);
}

void CPU::LDIindirA() {
	memory->writeByte(regPairWord(REG_HL), reg[REG_A]);
	incPair(REG_HL);
}

void CPU::addA(uint8_t value, bool carry, bool sub, bool save) {
	uint8_t addCarry = carry & (reg[REG_F] & FLAG_CARRY) ? 1 : 0;
	bool carry = ((uint16_t)reg[REG_A] + (uint16_t)value > 0xFF);
	bool hcarry = (((reg[REG_A] & 0x0F) + (value & 0x0F)) > 0x0F);
	bool zero;
	if (save) {
		reg[REG_A] = reg[REG_A] + value;
		zero = reg[REG_A] == 0;
	}
	else {
		zero = (reg[REG_A] + value) == 0;
	}
	reg[REG_F] = (
		(carry ? FLAG_CARRY : 0) |
		(hcarry ? FLAG_HALFCARRY : 0) |
		(sub ? FLAG_OPERATION : 0) |
		(zero ? FLAG_ZERO : 0)
	);
}

void CPU::addAr(RegisterID r, bool carry, bool sub) {
	addA(reg[r], carry, sub);
}

void CPU::addAindir(bool carry) {
	addA(memory->readByte(regPairWord(REG_HL)), carry);
}

void CPU::subA(uint8_t imm, bool carry) {
	addA(byteNeg(imm), carry, WITH_SUB);
}

void CPU::subAr(RegisterID r, bool carry) {
	addA(byteNeg(reg[r]), carry, WITH_SUB);
}

void CPU::subAindir(bool carry) {
	addA(byteNeg(memory->readByte(regPairWord(REG_HL))), carry, WITH_SUB);
}

void CPU::cmpA(uint8_t imm) {
	addA(byteNeg(imm), false, WITH_SUB, DONT_SAVE);
}

void CPU::cmpAr(RegisterID r) {
	addA(byteNeg(reg[r]), false, WITH_SUB, DONT_SAVE);
}

void CPU::cmpAindir() {
	addA(byteNeg(memory->readByte(regPairWord(REG_HL))), false, WITH_SUB, DONT_SAVE);
}

void CPU::andA(uint8_t value) {
	reg[REG_A] = reg[REG_A] & value;
	bool zero = reg[REG_A] == 0;
	reg[REG_F] = FLAG_HALFCARRY | zero;
}

void CPU::andAr(RegisterID r) {
	andA(reg[r]);
}

void CPU::andAn(uint8_t imm) {
	andA(imm);
}

void CPU::andAindir() {
	andA(memory->readByte(regPairWord(REG_HL)));
}

void CPU::orA(uint8_t value) {
	reg[REG_A] = reg[REG_A] | value;
	bool zero = reg[REG_A] == 0;
	reg[REG_F] = zero;
}

void CPU::orAr(RegisterID r) {
	orA(reg[r]);
}

void CPU::orAn(uint8_t imm) {
	orA(imm);
}

void CPU::orAindir() {
	orA(memory->readByte(regPairWord(REG_HL)));
}

void CPU::xorA(uint8_t value) {
	reg[REG_A] = reg[REG_A] ^ value;
	bool zero = reg[REG_A] == 0;
	reg[REG_F] = zero;
}

void CPU::xorAr(RegisterID r) {
	xorA(reg[r]);
}

void CPU::xorAn(uint8_t imm) {
	xorA(imm);
}

void CPU::xorAindir() {
	xorA(memory->readByte(regPairWord(REG_HL)));
}

void CPU::incR(RegisterID r) {
	reg[r]++;
}

void CPU::incIndir() {
	uint16_t target = regPairWord(REG_HL);
	memory->writeByte(target, memory->readByte(target) + 1);
}

void CPU::decR(RegisterID r) {
	reg[r]--;
}

void CPU::decIndir() {
	uint16_t target = regPairWord(REG_HL);
	memory->writeByte(target, memory->readByte(target) - 1);
}

void CPU::addHLpair(RegisterPair pair) {
	regPairWordSave(REG_HL, regPairWord(REG_HL) + regPairWord(pair));
}

void CPU::addHLSP() {
	regPairWordSave(REG_HL, regPairWord(REG_HL) + regSP);
}

void CPU::addSPn(uint8_t imm) {
	if (imm > 127) {
		regSP -= ~imm + 1;
	}
	else {
		regSP += imm;
	}
	// TODO: proper flags
	reg[REG_F] = 0;
}

void CPU::incPair(RegisterPair rp) {
	regPairWordSave(rp, regPairWord(rp) + 1);
}

void CPU::decPair(RegisterPair rp) {
	regPairWordSave(rp, regPairWord(rp) - 1);
}

void CPU::incSP() {
	regSP++;
}

void CPU::decSP() {
	regSP--;
}

void CPU::rotL(RegisterID r) {
	reg[r] = _rotl8(reg[r], 1);
	reg[REG_F] = ((reg[r] == 0) ? FLAG_ZERO : 0) | ((reg[r] & 0x01) ? FLAG_CARRY : 0);
}

void CPU::rotLindir() {
	uint16_t target = regPairWord(REG_HL);
	uint8_t newVal = _rotl8(memory->readByte(target), 1);
	reg[REG_F] = ((newVal == 0) ? FLAG_ZERO : 0) | ((newVal & 0x01) ? FLAG_CARRY : 0);
	memory->writeByte(target, newVal);
}

void CPU::rotLcarry(RegisterID r) {
	uint8_t newVal = (reg[r] << 1) | ((reg[REG_F] & FLAG_CARRY) ? 0x01 : 0);
	reg[REG_F] = ((newVal == 0) ? FLAG_ZERO : 0) | ((reg[r] & 0x80) ? FLAG_CARRY : 0);
	reg[r] = newVal;
}

void CPU::rotLcarryIndir() {
	uint16_t target = regPairWord(REG_HL);
	uint8_t oldVal = memory->readByte(target);
	uint8_t newVal = (oldVal << 1) | ((reg[REG_F] & FLAG_CARRY) ? 0x01 : 0);
	reg[REG_F] = ((newVal == 0) ? FLAG_ZERO : 0) | ((oldVal & 0x80) ? FLAG_CARRY : 0);
	memory->writeByte(target, newVal);
}

void CPU::rotR(RegisterID r) {
	reg[r] = _rotr8(reg[r], 1);
	reg[REG_F] = ((reg[r] == 0) ? FLAG_ZERO : 0) | ((reg[r] & 0x80) ? FLAG_CARRY : 0);
}

void CPU::rotRindir() {
	uint16_t target = regPairWord(REG_HL);
	uint8_t newVal = _rotr8(memory->readByte(target), 1);
	reg[REG_F] = ((newVal == 0) ? FLAG_ZERO : 0) | ((newVal & 0x80) ? FLAG_CARRY : 0);
	memory->writeByte(target, newVal);
}

void CPU::rotRcarry(RegisterID r) {
	uint8_t newVal = (reg[r] >> 1) | ((reg[REG_F] & FLAG_CARRY) ? 0x80 : 0);
	reg[REG_F] = ((newVal == 0) ? FLAG_ZERO : 0) | ((reg[r] & 0x01) ? FLAG_CARRY : 0);
	reg[r] = newVal;
}

void CPU::rotRcarryIndir() {
	uint16_t target = regPairWord(REG_HL);
	uint8_t oldVal = memory->readByte(target);
	uint8_t newVal = (oldVal >> 1) | ((reg[REG_F] & FLAG_CARRY) ? 0x80 : 0);
	reg[REG_F] = ((newVal == 0) ? FLAG_ZERO : 0) | ((oldVal & 0x01) ? FLAG_CARRY : 0);
	memory->writeByte(target, newVal);
}

void CPU::sftL(RegisterID r) {
	uint8_t newVal = reg[r] << 1;
	reg[REG_F] = ((newVal == 0) ? FLAG_ZERO : 0) | ((reg[r] & 0x80) ? FLAG_CARRY : 0);
	reg[r] = newVal;
}

void CPU::sftLindir() {
	uint16_t target = regPairWord(REG_HL);
	uint8_t oldVal = memory->readByte(target);
	uint8_t newVal = oldVal << 1;
	reg[REG_F] = ((newVal == 0) ? FLAG_ZERO : 0) | ((oldVal & 0x80) ? FLAG_CARRY : 0);
	memory->writeByte(target, newVal);
}

void CPU::sftR(RegisterID r) {
	uint8_t newVal = reg[r] >> 1;
	reg[REG_F] = ((newVal == 0) ? FLAG_ZERO : 0) | ((reg[r] & 0x01) ? FLAG_CARRY : 0);
	reg[r] = newVal;
}

void CPU::sftRindir() {
	uint16_t target = regPairWord(REG_HL);
	uint8_t oldVal = memory->readByte(target);
	uint8_t newVal = oldVal >> 1;
	reg[REG_F] = ((newVal == 0) ? FLAG_ZERO : 0) | ((oldVal & 0x01) ? FLAG_CARRY : 0);
	memory->writeByte(target, newVal);
}

void CPU::sftRA(RegisterID r) {
	uint8_t newVal = (reg[r] >> 1) | (reg[r] & 0x80);
	reg[REG_F] = ((newVal == 0) ? FLAG_ZERO : 0) | ((reg[r] & 0x01) ? FLAG_CARRY : 0);
	reg[r] = newVal;
}

void CPU::sftRAindir() {
	uint16_t target = regPairWord(REG_HL);
	uint8_t oldVal = memory->readByte(target);
	uint8_t newVal = (oldVal >> 1) | (oldVal & 0x80);
	reg[REG_F] = ((newVal == 0) ? FLAG_ZERO : 0) | ((oldVal & 0x01) ? FLAG_CARRY : 0);
	memory->writeByte(target, newVal);
}

void CPU::bitTest(RegisterID r, uint8_t bit) {
	reg[REG_F] = (
		((reg[r] & _rotl8(0x01, bit)) ? 0 : FLAG_ZERO) |
		(reg[REG_F] & FLAG_CARRY) |
		FLAG_HALFCARRY
		);
}

void CPU::bitTestIndir(uint8_t bit) {
	reg[REG_F] = (
		((memory->readByte(regPairWord(REG_HL)) & _rotl8(0x01, bit)) ? 0 : FLAG_ZERO) |
		(reg[REG_F] & FLAG_CARRY) |
		FLAG_HALFCARRY
		);
}

void CPU::bitReset(RegisterID r, uint8_t bit) {
	reg[r] = _rotl8(0xFE, bit) & reg[r];
}

void CPU::bitResetIndir(uint8_t bit) {
	uint16_t target = regPairWord(REG_HL);
	memory->writeByte(target, _rotl8(0xFE, bit) & memory->readByte(target));
}

void CPU::bitSet(RegisterID r, uint8_t bit) {
	reg[r] = (_rotl8(0xFE, bit) & reg[r]) | _rotl8(0x01, bit);
}

void CPU::bitSetIndir(uint8_t bit) {
	uint16_t target = regPairWord(REG_HL);
	memory->writeByte(
		target,
		(_rotl8(0xFE, bit) & memory->readByte(target)) | _rotl8(0x01, bit)
	);
}

void CPU::swapR(RegisterID r) {
	reg[r] = swap(reg[r]);
}

void CPU::swapIndir() {
	uint16_t target = regPairWord(REG_HL);
	memory->writeByte(target, swap(memory->readByte(target)));
}

void CPU::daa() {
	uint16_t low = reg[REG_A] & 0x0F;
	bool hcarry = false;
	if (low > 9 || reg[REG_F] & FLAG_HALFCARRY) {
		reg[REG_A] += 0x06;
		hcarry = true;
	}
	uint16_t high = reg[REG_A] & 0xF0;
	bool carry = false;
	if (high > 9 || reg[REG_F] & FLAG_CARRY) {
		reg[REG_A] += 0x60;
		carry = true;
	}
	bool op = reg[REG_F] & FLAG_OPERATION;
	bool zero = reg[REG_A] == 0;
	reg[REG_F] = (
		(carry ? FLAG_CARRY : 0) |
		(hcarry ? FLAG_HALFCARRY : 0) |
		(op ? FLAG_OPERATION : 0) |
		(zero ? FLAG_ZERO : 0)
	);
}

void CPU::complementA() {
	reg[REG_A] = ~reg[REG_A];
	reg[REG_F] = (reg[REG_F] & (FLAG_ZERO | FLAG_CARRY)) | FLAG_OPERATION | FLAG_HALFCARRY;
}

void CPU::complementCarry() {
	bool carry = reg[REG_F] & FLAG_CARRY;
	reg[REG_F] = (reg[REG_F] & FLAG_ZERO) | (carry ? 0 : FLAG_CARRY);
}

void CPU::setCarry() {
	reg[REG_F] = (reg[REG_F] & FLAG_ZERO) | FLAG_CARRY;
}

void CPU::interruptSet(bool enable) {
	interruptDoNext = true;
	interruptNext = enable;
}

void CPU::jump(uint16_t imm) {
	regPCnext = imm;
}

void CPU::jumpSet(uint16_t imm, CPU::FlagBit flag) {
	if (reg[REG_F] & flag)
		regPCnext = imm;
}

void CPU::jumpReset(uint16_t imm, CPU::FlagBit flag) {
	if ((reg[REG_F] & flag) == 0)
		regPCnext = imm;
}

void CPU::jumpHL() {
	regPCnext = memory->readByte(regPairWord(REG_HL));
}

void CPU::jumpRel(uint8_t imm) {
	regPCnext = regPC + (uint16_t)imm;
}

void CPU::jumpRelSet(uint8_t imm, CPU::FlagBit flag) {
	if (reg[REG_F] & flag)
		regPCnext = regPC + (uint16_t)imm;
}

void CPU::jumpRelReset(uint8_t imm, CPU::FlagBit flag) {
	if ((reg[REG_F] & flag) == 0)
		regPCnext = regPC + (uint16_t)imm;
}

void CPU::call(uint16_t imm) {
	regSP--;
	memory->writeWord(regSP, regPCnext);
	regSP--;
	regPCnext = imm;
}

void CPU::callSet(uint16_t imm, CPU::FlagBit flag) {
	if (reg[REG_F] & flag) {
		regSP--;
		memory->writeWord(regSP, regPCnext);
		regSP--;
		regPCnext = imm;
	}
}

void CPU::callReset(uint16_t imm, CPU::FlagBit flag) {
	if ((reg[REG_F] & flag) == 0) {
		regSP--;
		memory->writeWord(regSP, regPCnext);
		regSP--;
		regPCnext = imm;
	}
}

void CPU::ret() {
	regSP--;
	regPCnext = memory->readWord(regSP);
	regSP--;
}

void CPU::retSet(CPU::FlagBit flag) {
	if (reg[REG_F] & flag) {
		regSP--;
		regPCnext = memory->readWord(regSP);
		regSP--;
	}
}

void CPU::retReset(CPU::FlagBit flag) {
	if ((reg[REG_F] & flag) == 0) {
		regSP--;
		regPCnext = memory->readWord(regSP);
		regSP--;
	}
}

void CPU::retInterrupt() {
	regSP--;
	regPCnext = memory->readWord(regSP);
	regSP--;
	interruptDoNext = true;
	interruptNext = true;
}

void CPU::restart(uint8_t dest) {
	regSP--;
	memory->writeWord(regSP, regPCnext);
	regSP--;
	regPCnext = dest;
}

}