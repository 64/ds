#pragma once
#include <stdbool.h>
#include <stddef.h>

#define bitmap_test(map, which) (map[(which) >> 3] & (1 << ((which) & 7)))

#define bitmap_set(map, which) map[(which) >> 3] |= (1 << ((which) & 7))

#define bitmap_clear(map, which) map[(which) >> 3] &= ~(1 << ((which) & 7))
