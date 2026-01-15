#include "core.h"

// Divisione intera con segno tramite shift
static inline i8 sdiv(i8 n, u8 k) {
	i8 adj = (n >> k) & ((1 << k) - 1);
	return (n + adj) >> k;
}
