#ifndef _UTIL_H_
#define _UTIL_H_

// Bit manipulation
#define READ_BIT(value, bit) (((value) >> (bit)) & 0x01)

#define TEST_MASK(value, mask) (((value) & mask) == mask)
#define TEST_BIT(value, bit) (((value) & (1 << bit)) != 0)

#define SET_MASK(value, mask) ((value) | mask)
#define SET_BIT(value, bit) ((value) | (1 << bit))

#define UNSET_MASK(value, mask) ((value) & ~mask)
#define UNSET_BIT(value, bit) ((value) & ~(1 << bit))

#define COPY_MASK(dst, src, mask) (((dst) & (~(mask))) | ((src) & (mask)))

#define SET_MASK_IF(value, mask, condition) (condition ? SET_MASK(value, mask) : UNSET_MASK(value, mask))
#define SET_BIT_IF(value, bit, condition) (condition ? SET_BIT(value, bit) : UNSET_BIT(value, bit))

#define REINTERPRET(value, type) (*reinterpret_cast<const type*>(&value))

#define DETECT_CARRY(a, b, r) ((a & b) | (a & ~r) | (b & ~r))

// Memory
#define SAFE_DELETE(ptr) \
if (ptr) \
{ \
	delete ptr; \
	ptr = NULL; \
} \

#define SAFE_DELETE_ARRAY(ptr) \
if (ptr) \
{ \
	delete[] ptr; \
	ptr = NULL; \
} \

#endif