#ifndef _UTIL_H_
#define _UTIL_H_

#define READ_MASK(value, mask) (((value) & mask) == mask)
#define READ_BIT(value, bit) (((value) & (1 << bit)) != 0)

#define SET_MASK(value, mask) ((value) | mask)
#define SET_BIT(value, bit) ((value) | (1 << bit))

#define UNSET_MASK(value, mask) ((value) & ~mask)
#define UNSET_BIT(value, bit) ((value) & ~(1 << bit))

#define SET_MASK_IF(value, mask, condition) (condition ? SET_MASK(value, mask) : UNSET_MASK(value, mask))
#define SET_BIT_IF(value, bit, condition) (condition ? SET_BIT(value, bit) : UNSET_BIT(value, bit))

#define REINTERPRET(value, type) (*reinterpret_cast<const type*>(&value))

#endif