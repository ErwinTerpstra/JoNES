#include "cpu.h"

using namespace libnes;

const uint8_t CPU::ADDRESSING_MODE_LENGTHS[] =
{
	1,		// Implied
	2,		// Immediate
	2,		// Relative
	2,		// Zero page
	1,		// Zero page, X indexed
	1,		// Zero page, Y indexed
	3,		// Indirect
	2,		// Indirect from zero page, X pre-indexed
	2,		// Indirect from zero page, Y post-indexed
	3,		// Absolute
	3,		// Absolute, X indexed
	3,		// Absolute, Y indexed
	0,		// Invalid
};

const CPU::Instruction CPU::INSTRUCTION_MAP[] =
{
	{ 0x00, 7, ADDR_IMPL,			"BRK", &CPU::brk },
	{ 0x01, 1, ADDR_IZPX,			"ORA", &CPU::ora },
	{ 0x02, 1, ADDR_INVALID, "", NULL },
	{ 0x03, 1, ADDR_INVALID, "", NULL },
	{ 0x04, 1, ADDR_INVALID, "", NULL },
	{ 0x05, 1, ADDR_ZP,				"ORA", &CPU::ora },
	{ 0x06, 1, ADDR_ZP,				"ASL", &CPU::asl },
	{ 0x07, 1, ADDR_INVALID, "", NULL },
	{ 0x08, 1, ADDR_INVALID, "", NULL },
	{ 0x09, 1, ADDR_IMM,			"ORA", &CPU::ora },
	{ 0x0A, 1, ADDR_IMPL,			"ASL A", &CPU::asl_a },
	{ 0x0B, 1, ADDR_INVALID, "", NULL },
	{ 0x0C, 1, ADDR_INVALID, "", NULL },
	{ 0x0D, 1, ADDR_ABS,			"ORA", &CPU::ora },
	{ 0x0E, 1, ADDR_ABS,			"ASL", &CPU::asl },
	{ 0x0F, 1, ADDR_INVALID, "", NULL },

	{ 0x10, 2, ADDR_REL,			"BPL", &CPU::branch },
	{ 0x11, 1, ADDR_IZPY,			"ORA", &CPU::ora },
	{ 0x12, 1, ADDR_INVALID, "", NULL },
	{ 0x13, 1, ADDR_INVALID, "", NULL },
	{ 0x14, 1, ADDR_INVALID, "", NULL },
	{ 0x15, 1, ADDR_ZPX,			"ORA", &CPU::ora },
	{ 0x16, 1, ADDR_ZPX,			"ASL", &CPU::asl },
	{ 0x17, 1, ADDR_INVALID, "", NULL },
	{ 0x18, 1, ADDR_INVALID, "", NULL },
	{ 0x19, 1, ADDR_ABSY,			"ORA", &CPU::ora },
	{ 0x1A, 1, ADDR_INVALID, "", NULL },
	{ 0x1B, 1, ADDR_INVALID, "", NULL },
	{ 0x1C, 1, ADDR_INVALID, "", NULL },
	{ 0x1D, 1, ADDR_ABSX,			"ORA", &CPU::ora },
	{ 0x1E, 1, ADDR_ABSX,			"ASL", &CPU::asl },
	{ 0x1F, 1, ADDR_INVALID, "", NULL },

	{ 0x20, 6, ADDR_ABS,			"JSR", &CPU::jsr },
	{ 0x21, 1, ADDR_IZPX,			"AND", &CPU::and$ },
	{ 0x22, 1, ADDR_INVALID, "", NULL },
	{ 0x23, 1, ADDR_INVALID, "", NULL },
	{ 0x24, 1, ADDR_INVALID, "", NULL },
	{ 0x25, 1, ADDR_ZP,				"AND", &CPU::and$ },
	{ 0x26, 1, ADDR_ZP,				"ROL", &CPU::rol },
	{ 0x27, 1, ADDR_INVALID, "", NULL },
	{ 0x28, 1, ADDR_INVALID, "", NULL },
	{ 0x29, 1, ADDR_IMM,			"AND", &CPU::and$ },
	{ 0x2A, 1, ADDR_IMPL,			"ROL A", &CPU::rol_a },
	{ 0x2B, 1, ADDR_INVALID, "", NULL },
	{ 0x2C, 1, ADDR_INVALID, "", NULL },
	{ 0x2D, 1, ADDR_ABS,			"AND", &CPU::and$ },
	{ 0x2E, 1, ADDR_ABS,			"ROL", &CPU::rol },
	{ 0x2F, 1, ADDR_INVALID, "", NULL },

	{ 0x30, 2, ADDR_REL,			"BMI", &CPU::branch },
	{ 0x31, 1, ADDR_IZPY,			"AND", &CPU::and$ },
	{ 0x32, 1, ADDR_INVALID, "", NULL },
	{ 0x33, 1, ADDR_INVALID, "", NULL },
	{ 0x34, 1, ADDR_INVALID, "", NULL },
	{ 0x35, 1, ADDR_ZPX,			"AND", &CPU::and$ },
	{ 0x36, 1, ADDR_ZPX,			"ROL", &CPU::rol },
	{ 0x37, 1, ADDR_INVALID, "", NULL },
	{ 0x38, 1, ADDR_INVALID, "", NULL },
	{ 0x39, 1, ADDR_ABSY,			"AND", &CPU::and$ },
	{ 0x3A, 1, ADDR_INVALID, "", NULL },
	{ 0x3B, 1, ADDR_INVALID, "", NULL },
	{ 0x3C, 1, ADDR_INVALID, "", NULL },
	{ 0x3D, 1, ADDR_ABSX,			"AND", &CPU::and$ },
	{ 0x3E, 1, ADDR_ABSX,			"ROL", &CPU::rol },
	{ 0x3F, 1, ADDR_INVALID, "", NULL },

	{ 0x40, 6, ADDR_IMPL,			"RTI", &CPU::rti },
	{ 0x41, 1, ADDR_IZPX,			"EOR", &CPU::eor },
	{ 0x42, 1, ADDR_INVALID, "", NULL },
	{ 0x43, 1, ADDR_INVALID, "", NULL },
	{ 0x44, 1, ADDR_INVALID, "", NULL },
	{ 0x45, 1, ADDR_ZP,				"EOR", &CPU::eor },
	{ 0x46, 1, ADDR_ZP,				"LSR", &CPU::lsr },
	{ 0x47, 1, ADDR_INVALID, "", NULL },
	{ 0x48, 1, ADDR_INVALID, "", NULL },
	{ 0x49, 1, ADDR_IMM,			"EOR", &CPU::eor },
	{ 0x4A, 1, ADDR_IMPL,			"LSR A", &CPU::lsr_a },
	{ 0x4B, 1, ADDR_INVALID, "", NULL },
	{ 0x4C, 3, ADDR_ABS,			"JMP", &CPU::jmp_abs },
	{ 0x4D, 1, ADDR_ABS,			"EOR", &CPU::eor },
	{ 0x4E, 1, ADDR_ABS,			"LSR", &CPU::lsr },
	{ 0x4F, 1, ADDR_INVALID, "", NULL },

	{ 0x50, 2, ADDR_REL,			"BVC", &CPU::branch },
	{ 0x51, 1, ADDR_IZPY,			"EOR", &CPU::eor },
	{ 0x52, 1, ADDR_INVALID, "", NULL },
	{ 0x53, 1, ADDR_INVALID, "", NULL },
	{ 0x54, 1, ADDR_INVALID, "", NULL },
	{ 0x55, 1, ADDR_ZPX,			"EOR", &CPU::eor },
	{ 0x56, 1, ADDR_ZPX,			"LSR", &CPU::lsr },
	{ 0x57, 1, ADDR_INVALID, "", NULL },
	{ 0x58, 1, ADDR_INVALID, "", NULL },
	{ 0x59, 1, ADDR_ABSY,			"EOR", &CPU::eor },
	{ 0x5A, 1, ADDR_INVALID, "", NULL },
	{ 0x5B, 1, ADDR_INVALID, "", NULL },
	{ 0x5C, 1, ADDR_INVALID, "", NULL },
	{ 0x5D, 1, ADDR_ABSX,			"EOR", &CPU::eor },
	{ 0x5E, 1, ADDR_ABSX,			"LSR", &CPU::lsr },
	{ 0x5F, 1, ADDR_INVALID, "", NULL },

	{ 0x60, 6, ADDR_IMPL,			"RTS", &CPU::rts },
	{ 0x61, 1, ADDR_IZPX,			"ADC", &CPU::adc },
	{ 0x62, 1, ADDR_INVALID, "", NULL },
	{ 0x63, 1, ADDR_INVALID, "", NULL },
	{ 0x64, 1, ADDR_INVALID, "", NULL },
	{ 0x65, 1, ADDR_ZP,				"ADC", &CPU::adc },
	{ 0x66, 1, ADDR_ZP,				"ROR", &CPU::ror },
	{ 0x67, 1, ADDR_INVALID, "", NULL },
	{ 0x68, 1, ADDR_INVALID, "", NULL },
	{ 0x69, 1, ADDR_IMM,			"ADC", &CPU::adc },
	{ 0x6A, 1, ADDR_IMPL,			"ROR A", &CPU::ror_a },
	{ 0x6B, 1, ADDR_INVALID, "", NULL },
	{ 0x6C, 5, ADDR_IND,			"JMP", &CPU::jmp_ind },
	{ 0x6D, 1, ADDR_ABS,			"ADC", &CPU::adc },
	{ 0x6E, 1, ADDR_ABSX,			"ROR", &CPU::ror },
	{ 0x6F, 1, ADDR_INVALID, "", NULL },

	{ 0x70, 2, ADDR_REL,			"BVS", &CPU::branch },
	{ 0x71, 1, ADDR_IZPY,			"ADC", &CPU::adc },
	{ 0x72, 1, ADDR_INVALID, "", NULL },
	{ 0x73, 1, ADDR_INVALID, "", NULL },
	{ 0x74, 1, ADDR_INVALID, "", NULL },
	{ 0x75, 1, ADDR_ZPX,			"ADC", &CPU::adc },
	{ 0x76, 1, ADDR_ZPX,			"ROR", &CPU::ror },
	{ 0x77, 1, ADDR_INVALID, "", NULL },
	{ 0x78, 1, ADDR_INVALID, "", NULL },
	{ 0x79, 1, ADDR_ABSY,			"ADC", &CPU::adc },
	{ 0x7A, 1, ADDR_INVALID, "", NULL },
	{ 0x7B, 1, ADDR_INVALID, "", NULL },
	{ 0x7C, 1, ADDR_INVALID, "", NULL },
	{ 0x7D, 1, ADDR_ABSX,			"ADC", &CPU::adc },
	{ 0x7E, 1, ADDR_ABSX,			"ROR", &CPU::ror },
	{ 0x7F, 1, ADDR_INVALID, "", NULL },

	{ 0x80, 1, ADDR_INVALID, "", NULL },
	{ 0x81, 1, ADDR_INVALID, "", NULL },
	{ 0x82, 1, ADDR_INVALID, "", NULL },
	{ 0x83, 1, ADDR_INVALID, "", NULL },
	{ 0x84, 1, ADDR_INVALID, "", NULL },
	{ 0x85, 1, ADDR_INVALID, "", NULL },
	{ 0x86, 1, ADDR_INVALID, "", NULL },
	{ 0x87, 1, ADDR_INVALID, "", NULL },
	{ 0x88, 1, ADDR_IMPL,			"DEX", &CPU::dex },
	{ 0x89, 1, ADDR_INVALID, "", NULL },
	{ 0x8A, 1, ADDR_INVALID, "", NULL },
	{ 0x8B, 1, ADDR_INVALID, "", NULL },
	{ 0x8C, 1, ADDR_INVALID, "", NULL },
	{ 0x8D, 1, ADDR_INVALID, "", NULL },
	{ 0x8E, 1, ADDR_INVALID, "", NULL },
	{ 0x8F, 1, ADDR_INVALID, "", NULL },

	{ 0x90, 2, ADDR_REL,			"BCC", &CPU::branch },
	{ 0x91, 1, ADDR_INVALID, "", NULL },
	{ 0x92, 1, ADDR_INVALID, "", NULL },
	{ 0x93, 1, ADDR_INVALID, "", NULL },
	{ 0x94, 1, ADDR_INVALID, "", NULL },
	{ 0x95, 1, ADDR_INVALID, "", NULL },
	{ 0x96, 1, ADDR_INVALID, "", NULL },
	{ 0x97, 1, ADDR_INVALID, "", NULL },
	{ 0x98, 1, ADDR_INVALID, "", NULL },
	{ 0x99, 1, ADDR_INVALID, "", NULL },
	{ 0x9A, 1, ADDR_INVALID, "", NULL },
	{ 0x9B, 1, ADDR_INVALID, "", NULL },
	{ 0x9C, 1, ADDR_INVALID, "", NULL },
	{ 0x9D, 1, ADDR_INVALID, "", NULL },
	{ 0x9E, 1, ADDR_INVALID, "", NULL },
	{ 0x9F, 1, ADDR_INVALID, "", NULL },

	{ 0xA0, 1, ADDR_INVALID, "", NULL },
	{ 0xA1, 1, ADDR_INVALID, "", NULL },
	{ 0xA2, 1, ADDR_INVALID, "", NULL },
	{ 0xA3, 1, ADDR_INVALID, "", NULL },
	{ 0xA4, 1, ADDR_INVALID, "", NULL },
	{ 0xA5, 1, ADDR_INVALID, "", NULL },
	{ 0xA6, 1, ADDR_INVALID, "", NULL },
	{ 0xA7, 1, ADDR_INVALID, "", NULL },
	{ 0xA8, 1, ADDR_INVALID, "", NULL },
	{ 0xA9, 1, ADDR_INVALID, "", NULL },
	{ 0xAA, 1, ADDR_INVALID, "", NULL },
	{ 0xAB, 1, ADDR_INVALID, "", NULL },
	{ 0xAC, 1, ADDR_INVALID, "", NULL },
	{ 0xAD, 1, ADDR_INVALID, "", NULL },
	{ 0xAE, 1, ADDR_INVALID, "", NULL },
	{ 0xAF, 1, ADDR_INVALID, "", NULL },

	{ 0xB0, 2, ADDR_REL,			"BCS", &CPU::branch },
	{ 0xB1, 1, ADDR_INVALID, "", NULL },
	{ 0xB2, 1, ADDR_INVALID, "", NULL },
	{ 0xB3, 1, ADDR_INVALID, "", NULL },
	{ 0xB4, 1, ADDR_INVALID, "", NULL },
	{ 0xB5, 1, ADDR_INVALID, "", NULL },
	{ 0xB6, 1, ADDR_INVALID, "", NULL },
	{ 0xB7, 1, ADDR_INVALID, "", NULL },
	{ 0xA8, 1, ADDR_INVALID, "", NULL },
	{ 0xA9, 1, ADDR_INVALID, "", NULL },
	{ 0xAA, 1, ADDR_INVALID, "", NULL },
	{ 0xAB, 1, ADDR_INVALID, "", NULL },
	{ 0xAC, 1, ADDR_INVALID, "", NULL },
	{ 0xAD, 1, ADDR_INVALID, "", NULL },
	{ 0xAE, 1, ADDR_INVALID, "", NULL },
	{ 0xAF, 1, ADDR_INVALID, "", NULL },

	{ 0xC0, 1, ADDR_IMM,			"CPY", &CPU::cpy },
	{ 0xC1, 1, ADDR_IZPX,			"CMP", &CPU::cmp },
	{ 0xC2, 1, ADDR_INVALID, "", NULL },
	{ 0xC3, 1, ADDR_INVALID, "", NULL },
	{ 0xC4, 1, ADDR_ZP,				"CPY", &CPU::cpy },
	{ 0xC5, 1, ADDR_ZP,				"CMP", &CPU::cmp },
	{ 0xC6, 1, ADDR_ZP,				"DEC", &CPU::dec },
	{ 0xC7, 1, ADDR_INVALID, "", NULL },
	{ 0xC8, 1, ADDR_IMPL,			"INY", &CPU::iny },
	{ 0xC9, 1, ADDR_IMM,			"CMP", &CPU::cmp },
	{ 0xCA, 1, ADDR_IMPL,			"DEX", &CPU::dex },
	{ 0xCB, 1, ADDR_INVALID, "", NULL },
	{ 0xCC, 1, ADDR_ABS,			"CPY", &CPU::cpy },
	{ 0xCD, 1, ADDR_ABS,			"CMP", &CPU::cmp },
	{ 0xCE, 1, ADDR_ABS,			"DEC", &CPU::dec },
	{ 0xCF, 1, ADDR_INVALID, "", NULL },

	{ 0xD0, 2, ADDR_REL,			"BNE", &CPU::branch },
	{ 0xD1, 1, ADDR_IZPY,			"CMP", &CPU::cmp },
	{ 0xD2, 1, ADDR_INVALID, "", NULL },
	{ 0xD3, 1, ADDR_INVALID, "", NULL },
	{ 0xD4, 1, ADDR_INVALID, "", NULL },
	{ 0xD5, 1, ADDR_ZPX,			"CMP", &CPU::cmp },
	{ 0xD6, 1, ADDR_ZPX,			"DEC", &CPU::dec },
	{ 0xD7, 1, ADDR_INVALID, "", NULL },
	{ 0xD8, 1, ADDR_INVALID, "", NULL },
	{ 0xD9, 1, ADDR_ABSY,			"CMP", &CPU::cmp },
	{ 0xDA, 1, ADDR_INVALID, "", NULL },
	{ 0xDB, 1, ADDR_INVALID, "", NULL },
	{ 0xDC, 1, ADDR_INVALID, "", NULL },
	{ 0xDD, 1, ADDR_ABSX,			"CMP", &CPU::cmp },
	{ 0xDE, 1, ADDR_ABSX,			"DEC", &CPU::dec },
	{ 0xDF, 1, ADDR_INVALID, "", NULL },

	{ 0xE0, 1, ADDR_IMM,			"CPX", &CPU::cpx },
	{ 0xE1, 1, ADDR_IZPX,			"SBC", &CPU::sbc },
	{ 0xE2, 1, ADDR_INVALID, "", NULL },
	{ 0xE3, 1, ADDR_INVALID, "", NULL },
	{ 0xE4, 1, ADDR_ZP,				"CPX", &CPU::cpx },
	{ 0xE5, 1, ADDR_ZP,				"SBC", &CPU::sbc },
	{ 0xE6, 1, ADDR_ZP,				"INC", &CPU::inc },
	{ 0xE7, 1, ADDR_INVALID, "", NULL },
	{ 0xE8, 1, ADDR_IMPL,			"INX", &CPU::inx },
	{ 0xE9, 1, ADDR_IMM,			"SBC", &CPU::sbc },
	{ 0xEA, 1, ADDR_INVALID, "", NULL },
	{ 0xEB, 1, ADDR_INVALID, "", NULL },
	{ 0xEC, 1, ADDR_ABS,			"CPX", &CPU::cpx },
	{ 0xED, 1, ADDR_ABS,			"SBC", &CPU::sbc },
	{ 0xEE, 1, ADDR_ABS,			"INC", &CPU::inc },
	{ 0xEF, 1, ADDR_INVALID, "", NULL },

	{ 0xF0, 2, ADDR_REL,			"BEQ", &CPU::branch },
	{ 0xF1, 1, ADDR_IZPY,			"SBC", &CPU::sbc},
	{ 0xF2, 1, ADDR_INVALID, "", NULL },
	{ 0xF3, 1, ADDR_INVALID, "", NULL },
	{ 0xF4, 1, ADDR_INVALID, "", NULL },
	{ 0xF5, 1, ADDR_ZPX,			"SBC", &CPU::sbc },
	{ 0xF6, 1, ADDR_ZPX,			"INC", &CPU::inc },
	{ 0xF7, 1, ADDR_INVALID, "", NULL },
	{ 0xF8, 1, ADDR_INVALID, "", NULL },
	{ 0xF9, 1, ADDR_ABSY,			"SBC", &CPU::sbc },
	{ 0xFA, 1, ADDR_INVALID, "", NULL },
	{ 0xFB, 1, ADDR_INVALID, "", NULL },
	{ 0xFC, 1, ADDR_INVALID, "", NULL },
	{ 0xFD, 1, ADDR_ABSX,			"SBC", &CPU::sbc },
	{ 0xFE, 1, ADDR_ABSX,			"INC", &CPU::inc },
	{ 0xFF, 1, ADDR_INVALID, "", NULL },
};