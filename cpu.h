#pragma once

#include <stdint.h>
#include "memory.h"

#define WITH_CARRY true
#define WITH_SUB true
#define DONT_SAVE false
#define byteNeg(val) (~(val)+1)

namespace NoxGB {

class CPU {
public:
	enum RegisterID {
		REG_B = 0,
		REG_C = 1,
		REG_D = 2,
		REG_E = 3,
		REG_H = 4,
		REG_L = 5,
		REG_F = 6,
		REG_A = 7
	};
	enum RegisterPair {
		REG_BC = 0,
		REG_DE = 2,
		REG_HL = 4,
		REG_PSW = 6
	};
	enum FlagBit {
		FLAG_CARRY = 0x10,
		FLAG_HALFCARRY = 0x20,
		FLAG_OPERATION = 0x40,
		FLAG_ZERO = 0x80
	};
	uint8_t reg[8] = { 0 };
	uint16_t regPC = 0;
	uint16_t regSP = 0;
	Memory* memory;
	bool interruptEnable = true;
	bool interruptDoNext = false;
	bool interruptNext = false;
	unsigned long cycle = 0;
	// Gets 2-byte word stored in register pair
	uint16_t regPairWord(RegisterPair rp);
	// Saves 2-byte word to register pair
	void regPairWordSave(RegisterPair rp, uint16_t n);
	// Swaps the lower and upper 4-bits
	uint8_t swap(uint8_t value);
	// 8-bit loads
	void LDrr(RegisterID r1, RegisterID r2);
	void LDrn(RegisterID r, uint8_t imm);
	void LDrIndir(RegisterID r);
	void LDindirR(RegisterID r);
	void LDindirN(uint8_t imm);
	void LDaIndirPair(RegisterPair rp);
	void LDaDir(uint16_t imm);
	void LDindirPairA(RegisterPair rp);
	void LDdirA(uint16_t imm);
	// high RAM
	void LDaHighC();
	void LDhighCA();
	void LDaHighDir(uint8_t imm);
	void LDhighDirA(uint8_t imm);
	// 16-bit loads
	void LDpairn(RegisterPair rp, uint16_t imm);
	void LDSPHL();
	void LDHLSPn(uint8_t imm);
	void LDdirSP(uint16_t imm);
	// stack
	void PUSHpair(RegisterPair rp);
	void POPpair(RegisterPair rp);
	// load decrement increment
	void LDDaIndir();
	void LDDindirA();
	void LDIaIndir();
	void LDIindirA();
	// 8-bit arithmetic
	// sub flag only affects flag register, the number input must be flipped
	void addA(uint8_t value, bool carry = false, bool sub = false, bool save = true);
	void addAr(RegisterID r, bool carry = false);
	void addAn(uint8_t imm, bool carry = false);
	void addAindir(bool carry = false);
	void subAr(RegisterID r, bool carry = false);
	void subAn(uint8_t imm, bool carry = false);
	void subAindir(bool carry = false);
	void cmpAr(RegisterID r, bool carry = false);
	void cmpAn(uint8_t imm, bool carry = false);
	void cmpAindir(bool carry = false);
	// logical operations
	void andA(uint8_t value);
	void andAr(RegisterID r);
	void andAn(uint8_t imm);
	void andAindir();
	void orA(uint8_t value);
	void orAr(RegisterID r);
	void orAn(uint8_t imm);
	void orAindir();
	void xorA(uint8_t value);
	void xorAr(RegisterID r);
	void xorAn(uint8_t imm);
	void xorAindir();
	// increments & decrements
	void incR(RegisterID r);
	void incIndir();
	void decR(RegisterID r);
	void decIndir();
	// 16-bit arithmetic
	void addHLPair(RegisterPair pair);
	void addSPn(uint8_t imm);
	void incPair(RegisterPair pair);
	void decPair(RegisterPair pair);
	// rotates
	void rotL(RegisterID r);
	void rotLindir();
	void rotLcarry(RegisterID r);
	void rotLcarryIndir();
	void rotR(RegisterID r);
	void rotRindir();
	void rotRcarry(RegisterID r);
	void rotRcarryIndir();
	// shifts
	void sftL(RegisterID r);
	void sftLindir();
	void sftR(RegisterID r);
	void sftRindir();
	void sftRA(RegisterID r);
	void sftRAindir();

	// misc
	void swapR(RegisterID r);
	void swapIndir();
	void daa();
	void complementA();
	void complementCarry();
	void setCarry();
	void interruptSet(bool enable);
};

}