#ifndef LGL_FIXED_POINT_H
#define LGL_FIXED_POINT_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef int32_t fixed_t;

#define FIXED_SHIFT 16
#define FIXED_ONE (1 << FIXED_SHIFT)
#define FIXED_INT_PART(x) ((x) >> FIXED_SHIFT)
#define TO_FIXED(x) ((x) << FIXED_SHIFT)
// TODO: on ESP32 64-bit values are slow, but without it multiplying doesn't work properly even on PC
#define FIXED_MUL(a, b) (fixed_t)(((int64_t)(a) * (int64_t)(b)) >> FIXED_SHIFT)
#define FIXED_DIV(a, b) (fixed_t)(((a) << FIXED_SHIFT) / (b))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif // LGL_FIXED_POINT_H
