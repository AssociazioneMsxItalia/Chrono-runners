#include "core.h"

static inline i16 abs(i16 a)
{
	return (a > 0) ? a : -a;
}

// Divisione intera con segno tramite shift
static inline i8 sdiv(i8 n, u8 k) {
	i8 adj = (n >> k) & ((1 << k) - 1);
	return (n + adj) >> k;
}