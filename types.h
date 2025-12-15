/* { types.h } : stdint etc
 */

#ifndef ALRTTS_TYPES
#define ALRTTS_TYPES

#if (_MSC_VER > 1000)
#   pragma once
#
#endif /* (_MSC_VER > 1000) */

#include <stdint.h>
#include <stddef.h>

typedef int8_t   i8_t;
typedef int16_t  i16_t;
typedef int32_t  i32_t;
typedef int64_t  i64_t;

typedef uint8_t  u8_t;
typedef uint16_t u16_t;
typedef uint32_t u32_t;
typedef uint64_t u64_t;

typedef float      f32_t;
typedef long float f64_t;

typedef u8_t  byte_t;
typedef u16_t word_t;
typedef u32_t dword_t;
typedef u64_t qword_t;

/*
 * feels better in asm to do a dword touch rather than a byte
 */
typedef dword_t bool_t;

#endif /* !ALRTTS_TYPES */
