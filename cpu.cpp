#pragma once
#include "cpu.h"
#include <intrin.h>

namespace NoxGB {

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
	regPairWordSave(REG_HL, regSP + imm);
}

void CPU::LDdirSP(uint16_t imm) {
	memory->writeWord(imm, regSP);
}

void CPU::PUSHpair(RegisterPair rp) {
	regSP--;
	memory->writeWord(regSP, regPairWord(rp));
	regSP--;
}

void CPU::POPpair(RegisterPair rp) {
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
	uint8_t addCarry = carry & ((reg[REG_F] & FLAG_CARRY) != 0) ? 1 : 0;
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

void CPU::addAr(RegisterID r, bool carry = false) {
	addA(reg[r], carry);
}

void CPU::addAn(uint8_t imm, bool carry = false) {
	addA(imm, carry);
}

void CPU::addAindir(bool carry = false) {
	addA(memory->readByte(regPairWord(REG_HL)), carry);
}

void CPU::subAr(RegisterID r, bool carry = false) {
	addA(byteNeg(reg[r]), carry, WITH_SUB);
}

void CPU::subAn(uint8_t imm, bool carry = false) {
	addA(byteNeg(imm), carry, WITH_SUB);
}

void CPU::subAindir(bool carry = false) {
	addA(byteNeg(memory->readByte(regPairWord(REG_HL))), carry, WITH_SUB);
}

void CPU::cmpAr(RegisterID r, bool carry = false) {
	addA(byteNeg(reg[r]), carry, WITH_SUB, DONT_SAVE);
}

void CPU::cmpAn(uint8_t imm, bool carry = false) {
	addA(byteNeg(imm), carry, WITH_SUB, DONT_SAVE);
}

void CPU::cmpAindir(bool carry = false) {
	addA(byteNeg(memory->readByte(regPairWord(REG_HL))), carry, WITH_SUB, DONT_SAVE);
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

void CPU::addHLPair(RegisterPair pair) {
	regPairWordSave(REG_HL, regPairWord(REG_HL) + regPairWord(pair));
}

void CPU::addSPn(uint8_t imm) {
	regSP += imm;
}

void CPU::incPair(RegisterPair rp) {
	regPairWordSave(rp, regPairWord(rp) + 1);
}

void CPU::decPair(RegisterPair rp) {
	regPairWordSave(rp, regPairWord(rp) - 1);
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

}