#include "cpu.h"

using namespace libnes;

const CPU::Instruction CPU::INSTRUCTION_MAP[] =
{
	{ 0x00, "", 1, 1, NULL },
	{ 0x01, "", 1, 1, NULL },
	{ 0x02, "", 1, 1, NULL },
	{ 0x03, "", 1, 1, NULL },
	{ 0x04, "", 1, 1, NULL },
	{ 0x05, "", 1, 1, NULL },
	{ 0x06, "", 1, 1, NULL },
	{ 0x07, "", 1, 1, NULL },
	{ 0x08, "", 1, 1, NULL },
	{ 0x09, "", 1, 1, NULL },
	{ 0x0A, "", 1, 1, NULL },
	{ 0x0B, "", 1, 1, NULL },
	{ 0x0C, "", 1, 1, NULL },
	{ 0x0D, "", 1, 1, NULL },
	{ 0x0E, "", 1, 1, NULL },
	{ 0x0F, "", 1, 1, NULL },

	{ 0x10, "", 1, 1, NULL },
	{ 0x11, "", 1, 1, NULL },
	{ 0x12, "", 1, 1, NULL },
	{ 0x13, "", 1, 1, NULL },
	{ 0x14, "", 1, 1, NULL },
	{ 0x15, "", 1, 1, NULL },
	{ 0x16, "", 1, 1, NULL },
	{ 0x17, "", 1, 1, NULL },
	{ 0x18, "", 1, 1, NULL },
	{ 0x19, "", 1, 1, NULL },
	{ 0x1A, "", 1, 1, NULL },
	{ 0x1B, "", 1, 1, NULL },
	{ 0x1C, "", 1, 1, NULL },
	{ 0x1D, "", 1, 1, NULL },
	{ 0x1E, "", 1, 1, NULL },
	{ 0x1F, "", 1, 1, NULL },

	{ 0x20, "", 1, 1, NULL },
	{ 0x21, "", 1, 1, NULL },
	{ 0x22, "", 1, 1, NULL },
	{ 0x23, "", 1, 1, NULL },
	{ 0x24, "", 1, 1, NULL },
	{ 0x25, "", 1, 1, NULL },
	{ 0x26, "", 1, 1, NULL },
	{ 0x27, "", 1, 1, NULL },
	{ 0x28, "", 1, 1, NULL },
	{ 0x29, "", 1, 1, NULL },
	{ 0x2A, "", 1, 1, NULL },
	{ 0x2B, "", 1, 1, NULL },
	{ 0x2C, "", 1, 1, NULL },
	{ 0x2D, "", 1, 1, NULL },
	{ 0x2E, "", 1, 1, NULL },
	{ 0x2F, "", 1, 1, NULL },

	{ 0x30, "", 1, 1, NULL },
	{ 0x31, "", 1, 1, NULL },
	{ 0x32, "", 1, 1, NULL },
	{ 0x33, "", 1, 1, NULL },
	{ 0x34, "", 1, 1, NULL },
	{ 0x35, "", 1, 1, NULL },
	{ 0x36, "", 1, 1, NULL },
	{ 0x37, "", 1, 1, NULL },
	{ 0x38, "", 1, 1, NULL },
	{ 0x39, "", 1, 1, NULL },
	{ 0x3A, "", 1, 1, NULL },
	{ 0x3B, "", 1, 1, NULL },
	{ 0x3C, "", 1, 1, NULL },
	{ 0x3D, "", 1, 1, NULL },
	{ 0x3E, "", 1, 1, NULL },
	{ 0x3F, "", 1, 1, NULL },

	{ 0x40, "", 1, 1, NULL },
	{ 0x41, "", 1, 1, NULL },
	{ 0x42, "", 1, 1, NULL },
	{ 0x43, "", 1, 1, NULL },
	{ 0x44, "", 1, 1, NULL },
	{ 0x45, "", 1, 1, NULL },
	{ 0x46, "", 1, 1, NULL },
	{ 0x47, "", 1, 1, NULL },
	{ 0x48, "", 1, 1, NULL },
	{ 0x49, "", 1, 1, NULL },
	{ 0x4A, "", 1, 1, NULL },
	{ 0x4B, "", 1, 1, NULL },
	{ 0x4C, "", 1, 1, NULL },
	{ 0x4D, "", 1, 1, NULL },
	{ 0x4E, "", 1, 1, NULL },
	{ 0x4F, "", 1, 1, NULL },

	{ 0x50, "", 1, 1, NULL },
	{ 0x51, "", 1, 1, NULL },
	{ 0x52, "", 1, 1, NULL },
	{ 0x53, "", 1, 1, NULL },
	{ 0x54, "", 1, 1, NULL },
	{ 0x55, "", 1, 1, NULL },
	{ 0x56, "", 1, 1, NULL },
	{ 0x57, "", 1, 1, NULL },
	{ 0x58, "", 1, 1, NULL },
	{ 0x59, "", 1, 1, NULL },
	{ 0x5A, "", 1, 1, NULL },
	{ 0x5B, "", 1, 1, NULL },
	{ 0x5C, "", 1, 1, NULL },
	{ 0x5D, "", 1, 1, NULL },
	{ 0x5E, "", 1, 1, NULL },
	{ 0x5F, "", 1, 1, NULL },

	{ 0x60, "", 1, 1, NULL },
	{ 0x61, "", 1, 1, NULL },
	{ 0x62, "", 1, 1, NULL },
	{ 0x63, "", 1, 1, NULL },
	{ 0x64, "", 1, 1, NULL },
	{ 0x65, "", 1, 1, NULL },
	{ 0x66, "", 1, 1, NULL },
	{ 0x67, "", 1, 1, NULL },
	{ 0x68, "", 1, 1, NULL },
	{ 0x69, "", 1, 1, NULL },
	{ 0x6A, "", 1, 1, NULL },
	{ 0x6B, "", 1, 1, NULL },
	{ 0x6C, "", 1, 1, NULL },
	{ 0x6D, "", 1, 1, NULL },
	{ 0x6E, "", 1, 1, NULL },
	{ 0x6F, "", 1, 1, NULL },

	{ 0x70, "", 1, 1, NULL },
	{ 0x71, "", 1, 1, NULL },
	{ 0x72, "", 1, 1, NULL },
	{ 0x73, "", 1, 1, NULL },
	{ 0x74, "", 1, 1, NULL },
	{ 0x75, "", 1, 1, NULL },
	{ 0x76, "", 1, 1, NULL },
	{ 0x77, "", 1, 1, NULL },
	{ 0x78, "", 1, 1, NULL },
	{ 0x79, "", 1, 1, NULL },
	{ 0x7A, "", 1, 1, NULL },
	{ 0x7B, "", 1, 1, NULL },
	{ 0x7C, "", 1, 1, NULL },
	{ 0x7D, "", 1, 1, NULL },
	{ 0x7E, "", 1, 1, NULL },
	{ 0x7F, "", 1, 1, NULL },

	{ 0x80, "", 1, 1, NULL },
	{ 0x81, "", 1, 1, NULL },
	{ 0x82, "", 1, 1, NULL },
	{ 0x83, "", 1, 1, NULL },
	{ 0x84, "", 1, 1, NULL },
	{ 0x85, "", 1, 1, NULL },
	{ 0x86, "", 1, 1, NULL },
	{ 0x87, "", 1, 1, NULL },
	{ 0x88, "", 1, 1, NULL },
	{ 0x89, "", 1, 1, NULL },
	{ 0x8A, "", 1, 1, NULL },
	{ 0x8B, "", 1, 1, NULL },
	{ 0x8C, "", 1, 1, NULL },
	{ 0x8D, "", 1, 1, NULL },
	{ 0x8E, "", 1, 1, NULL },
	{ 0x8F, "", 1, 1, NULL },

	{ 0x90, "", 1, 1, NULL },
	{ 0x91, "", 1, 1, NULL },
	{ 0x92, "", 1, 1, NULL },
	{ 0x93, "", 1, 1, NULL },
	{ 0x94, "", 1, 1, NULL },
	{ 0x95, "", 1, 1, NULL },
	{ 0x96, "", 1, 1, NULL },
	{ 0x97, "", 1, 1, NULL },
	{ 0x98, "", 1, 1, NULL },
	{ 0x99, "", 1, 1, NULL },
	{ 0x9A, "", 1, 1, NULL },
	{ 0x9B, "", 1, 1, NULL },
	{ 0x9C, "", 1, 1, NULL },
	{ 0x9D, "", 1, 1, NULL },
	{ 0x9E, "", 1, 1, NULL },
	{ 0x9F, "", 1, 1, NULL },

	{ 0xA0, "", 1, 1, NULL },
	{ 0xA1, "", 1, 1, NULL },
	{ 0xA2, "", 1, 1, NULL },
	{ 0xA3, "", 1, 1, NULL },
	{ 0xA4, "", 1, 1, NULL },
	{ 0xA5, "", 1, 1, NULL },
	{ 0xA6, "", 1, 1, NULL },
	{ 0xA7, "", 1, 1, NULL },
	{ 0xA8, "", 1, 1, NULL },
	{ 0xA9, "", 1, 1, NULL },
	{ 0xAA, "", 1, 1, NULL },
	{ 0xAB, "", 1, 1, NULL },
	{ 0xAC, "", 1, 1, NULL },
	{ 0xAD, "", 1, 1, NULL },
	{ 0xAE, "", 1, 1, NULL },
	{ 0xAF, "", 1, 1, NULL },

	{ 0xB0, "", 1, 1, NULL },
	{ 0xB1, "", 1, 1, NULL },
	{ 0xB2, "", 1, 1, NULL },
	{ 0xB3, "", 1, 1, NULL },
	{ 0xB4, "", 1, 1, NULL },
	{ 0xB5, "", 1, 1, NULL },
	{ 0xB6, "", 1, 1, NULL },
	{ 0xB7, "", 1, 1, NULL },
	{ 0xA8, "", 1, 1, NULL },
	{ 0xA9, "", 1, 1, NULL },
	{ 0xAA, "", 1, 1, NULL },
	{ 0xAB, "", 1, 1, NULL },
	{ 0xAC, "", 1, 1, NULL },
	{ 0xAD, "", 1, 1, NULL },
	{ 0xAE, "", 1, 1, NULL },
	{ 0xAF, "", 1, 1, NULL },

	{ 0xC0, "", 1, 1, NULL },
	{ 0xC1, "", 1, 1, NULL },
	{ 0xC2, "", 1, 1, NULL },
	{ 0xC3, "", 1, 1, NULL },
	{ 0xC4, "", 1, 1, NULL },
	{ 0xC5, "", 1, 1, NULL },
	{ 0xC6, "", 1, 1, NULL },
	{ 0xC7, "", 1, 1, NULL },
	{ 0xC8, "", 1, 1, NULL },
	{ 0xC9, "", 1, 1, NULL },
	{ 0xCA, "", 1, 1, NULL },
	{ 0xCB, "", 1, 1, NULL },
	{ 0xCC, "", 1, 1, NULL },
	{ 0xCD, "", 1, 1, NULL },
	{ 0xCE, "", 1, 1, NULL },
	{ 0xCF, "", 1, 1, NULL },

	{ 0xD0, "", 1, 1, NULL },
	{ 0xD1, "", 1, 1, NULL },
	{ 0xD2, "", 1, 1, NULL },
	{ 0xD3, "", 1, 1, NULL },
	{ 0xD4, "", 1, 1, NULL },
	{ 0xD5, "", 1, 1, NULL },
	{ 0xD6, "", 1, 1, NULL },
	{ 0xD7, "", 1, 1, NULL },
	{ 0xD8, "", 1, 1, NULL },
	{ 0xD9, "", 1, 1, NULL },
	{ 0xDA, "", 1, 1, NULL },
	{ 0xDB, "", 1, 1, NULL },
	{ 0xDC, "", 1, 1, NULL },
	{ 0xDD, "", 1, 1, NULL },
	{ 0xDE, "", 1, 1, NULL },
	{ 0xDF, "", 1, 1, NULL },

	{ 0xE0, "", 1, 1, NULL },
	{ 0xE1, "", 1, 1, NULL },
	{ 0xE2, "", 1, 1, NULL },
	{ 0xE3, "", 1, 1, NULL },
	{ 0xE4, "", 1, 1, NULL },
	{ 0xE5, "", 1, 1, NULL },
	{ 0xE6, "", 1, 1, NULL },
	{ 0xE7, "", 1, 1, NULL },
	{ 0xE8, "", 1, 1, NULL },
	{ 0xE9, "", 1, 1, NULL },
	{ 0xEA, "", 1, 1, NULL },
	{ 0xEB, "", 1, 1, NULL },
	{ 0xEC, "", 1, 1, NULL },
	{ 0xED, "", 1, 1, NULL },
	{ 0xEE, "", 1, 1, NULL },
	{ 0xEF, "", 1, 1, NULL },

	{ 0xF0, "", 1, 1, NULL },
	{ 0xF1, "", 1, 1, NULL },
	{ 0xF2, "", 1, 1, NULL },
	{ 0xF3, "", 1, 1, NULL },
	{ 0xF4, "", 1, 1, NULL },
	{ 0xF5, "", 1, 1, NULL },
	{ 0xF6, "", 1, 1, NULL },
	{ 0xF7, "", 1, 1, NULL },
	{ 0xF8, "", 1, 1, NULL },
	{ 0xF9, "", 1, 1, NULL },
	{ 0xFA, "", 1, 1, NULL },
	{ 0xFB, "", 1, 1, NULL },
	{ 0xFC, "", 1, 1, NULL },
	{ 0xFD, "", 1, 1, NULL },
	{ 0xFE, "", 1, 1, NULL },
	{ 0xFF, "", 1, 1, NULL },
};