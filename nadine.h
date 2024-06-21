/*******************************************************************************
*                                                                              *
*   NADINE -- PORTABLE, SINGLE-HEADER, ENDIAN CONVERSION LIBRARY FOR C         *
*   DEVELOPED BY SAMPO HIPPELAINEN (HISAHI)                                    *
*   VERSION   2024-06-21.2                                                     *
*                                                                              *
*   THIS LIBRARY IS DUAL-LICENSED UNDER THE UNLICENSE PUBLIC-DOMAIN            *
*   EQUIVALENT LICENSE AND THE MIT LICENSE.                                    *
*                                                                              *
*******************************************************************************/

/* To use this library:
    1. define NADINE_STATIC before every #include "nadine.h" to define all 
       needed functions statically, or
    2. include this in any number of compilation units, but define NADINE_IMPL 
       before #include "nadine.h" in exactly one of them. */

/* To configure the behavior of this library: 
    NADINE_IMPL         0|1     whether to define functions 
    NADINE_STATIC       0|1     whether to define functions statically
    NADINE_FLOAT        0|1     enable or disable float support
                                enabled automatically for IEEE 754
                                behavior undefined if enabled and not IEEE 754
    NADINE_STDINT       0|1     whether stdint.h is available
                                default value is whether on C99 or above
    NADINE_MEMCPY       0|1     whether memcpy is available
                                default value is whether on hosted C env
    NADINE_NATIVE_ENDIAN_INT_AUTO
                        0|1     detect NADINE_NATIVE_ENDIAN_INT automatically
                                through the compiler, if possible. default = 1
    NADINE_NATIVE_ENDIAN_FLOAT_AUTO
                        0|1     detect NADINE_NATIVE_ENDIAN_FLOAT automatically
                                through the compiler, if possible. default = 1
    NADINE_NATIVE_ENDIAN_INT
        native endianness (see `endian' below) for integral types
    NADINE_NATIVE_ENDIAN_FLOAT
        native endianness (see `endian' below) for floating-point types */

/* Documentation:

  For all of the following functions, `endian' values are one of the following:
      NADINE_ENDIAN_LITTLE      for little-endian
      NADINE_ENDIAN_BIG         for big-endian

  combined with any number (through bitwise OR or addition) of the following:
      NADINE_ENDIAN_SWAPCHARS   swap char pairs

  examples:
      NADINE_ENDIAN_BIG | NADINE_ENDIAN_SWAPCHARS
            is the PDP-11 byte order
      NADINE_ENDIAN_LITTLE | NADINE_ENDIAN_SWAPCHARS
            is the Honeywell 316 byte order

  T nadine_convert_N(unsigned endian, T value)
      Converts the given value to or from the given endianness.
      endian must be a valid endianness value, or the behavior is undefined.
      Aliases of this function (as macros) are also provided with the same
      names, but convert replaced with convert_from and convert_to.
      Since endianness conversions are involutions, these functions have
      identical behavior, and their purpose is merely to allow for
      self-documenting code.
  T nadine_read_N(unsigned endian, const void *source)
      Reads an integer or floating-point value of type T at the given pointer
      with the specified endianness and returns it. This function results in
      undefined behavior if source does not contain at least `sizeof(T)'
      characters of information. `endian' must be a valid endianness value,
      or the behavior is undefined.
  void nadine_write_N(unsigned endian, void *destination, T value)
      Writes an integer or floating-point value of type T at the given pointer
      with the specified endianness. This function results in undefined behavior
      if destination does not have space for at least `sizeof(T)' characters
      of information. `endian' must be a valid endianness value,
      or the behavior is undefined.
  unsigned nadine_endian_native_N(void)
      Returns the native endianness of the system for the type
      corresponding to N. The return value is always either a valid `endian'
      value, or `NADINE_ENDIAN_UNKNOWN' (not a valid `endian')
      if the endianness is unsupported.
      If compiler support is available, endianness may be detected
      automatically, in which case this may instead be implemented
      as a preprocessor macro.

Internal functions are prefixed with `nadine_i_'.

Note that functions in the public API are not guaranteed to have
external linkage. They are not even guaranteed to be actual functions
rather than preprocessor macros.

These sets of functions are provided for the following types:

| T                    | N                    | Notes                          |
| -------------------- | -------------------- | ------------------------------ |
| `short'              | `short'              |                                |
| `unsigned short'     | `unsigned_short'     |                                |
| `int'                | `int'                |                                |
| `unsigned int'       | `unsigned_int'       |                                |
| `long'               | `long'               |                                |
| `unsigned long'      | `unsigned_long'      |                                |
| `long long'          | `long_long'          | C99 and above only             |
| `unsigned long long' | `unsigned_long_long' | C99 and above only             |
| `int8_t'             | `int8'               | `NADINE_STDINT'                |
| `uint8_t'            | `uint8'              | `NADINE_STDINT'                |
| `int16_t'            | `int16'              | `NADINE_STDINT'                |
| `uint16_t'           | `uint16'             | `NADINE_STDINT'                |
| `int32_t'            | `int32'              | `NADINE_STDINT'                |
| `uint32_t'           | `uint32'             | `NADINE_STDINT'                |
| `int64_t'            | `int64'              | `NADINE_STDINT'                |
| `uint64_t'           | `uint64'             | `NADINE_STDINT'                |
| `float'              | `float'              | `NADINE_FLOAT'                 |
| `double'             | `double'             | `NADINE_FLOAT'                 |

Example of a full name under this scheme: `nadine_convert_int'

*******************************************************************************/

/** MIT license terms:

MIT License

Copyright (c) 2024 Sampo Hippelainen (hisahi)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

For Unlicense terms, please refer to <https://unlicense.org>.

*******************************************************************************/

#ifndef NADINE_H
#define NADINE_H

/* we at least pretend to be C++ compatible */
#ifdef __cplusplus
extern "C" {
#endif

#include <limits.h>
#include <stddef.h>

/* endian values. all are valid except for NADINE_ENDIAN_UNKNOWN,
   which can be returned by nadine_endian_native_* */
#define NADINE_ENDIAN_LITTLE 0
#define NADINE_ENDIAN_BIG 1
#define NADINE_ENDIAN_SWAPCHARS 2
#define NADINE_ENDIAN_UNKNOWN UINT_MAX

/* check C99 */
#ifndef NADINE_I_C99
#if __STDC_VERSION__ >= 199901L
#define NADINE_I_C99 1
#else
#define NADINE_I_C99 0
#endif
#endif /* #ifndef NADINE_I_C99 */

/* check C11 */
#ifndef NADINE_I_C11
#if __STDC_VERSION__ >= 201112L
#define NADINE_I_C11 1
#else
#define NADINE_I_C11 0
#endif
#endif /* #ifndef NADINE_I_C11 */

/* check C23 */
#ifndef NADINE_I_C23
#if __STDC_VERSION__ >= 202311L
#define NADINE_I_C23 1
#else
#define NADINE_I_C23 0
#endif
#endif /* #ifndef NADINE_I_C23 */

/* check memcpy */
#ifndef NADINE_MEMCPY
#if __STDC_HOSTED__ || HAVE_STRING_H
#define NADINE_MEMCPY 1
#else
#define NADINE_MEMCPY 0
#endif
#endif /* #ifndef NADINE_MEMCPY */

/* check floats */
#if !defined(NADINE_FLOAT) || NADINE_FLOAT != 0
#include <float.h>
/* attempt an IEEE 754 check */
#if FLT_RADIX != 2 || DBL_MANT_DIG != 53 || DBL_DIG != 15 \
    || DBL_MIN_EXP != -1021 || DBL_MIN_10_EXP != -307 \
    || DBL_MAX_EXP != 1024 || DBL_MAX_10_EXP != 308
#define NADINE_I_FLOAT_IEEE754 0
#else
#define NADINE_I_FLOAT_IEEE754 1
#endif
/* enable NADINE_FLOAT by default if on IEEE 754 */
#ifndef NADINE_FLOAT
#define NADINE_FLOAT NADINE_I_FLOAT_IEEE754
#endif /* #ifndef NADINE_FLOAT */
#if NADINE_FLOAT && !NADINE_I_FLOAT_IEEE754
#error NADINE_FLOAT=1 requires IEEE 754 compatible floating-point types
#endif
#endif /* #if !defined(NADINE_FLOAT) || NADINE_FLOAT != 0 */

/* check stdint */
#ifndef NADINE_STDINT
#if NADINE_I_C99 || defined(UINT64_MAX) || HAS_STDINT_H
#define NADINE_STDINT 1
#else
#define NADINE_STDINT 0
#endif
#endif

#if NADINE_STDINT
#include <stdint.h>
#endif

/* unsigned long long check */
#ifndef NADINE_I_HAS_ULL
#if NADINE_I_C99 || defined(ULLONG_MAX)
#define NADINE_I_HAS_ULL 1
#else
#define NADINE_I_HAS_ULL 0
#endif
#endif /* NADINE_I_HAS_ULL */

/* auto for both by default unless explicitly disabled */
#ifndef NADINE_NATIVE_ENDIAN_INT_AUTO
#define NADINE_NATIVE_ENDIAN_INT_AUTO 1
#endif
#ifndef NADINE_NATIVE_ENDIAN_FLOAT_AUTO
#define NADINE_NATIVE_ENDIAN_FLOAT_AUTO 1
#endif

#if (!defined(NADINE_NATIVE_ENDIAN_INT) && NADINE_NATIVE_ENDIAN_INT_AUTO)      \
    || (!defined(NADINE_NATIVE_ENDIAN_FLOAT) && NADINE_NATIVE_ENDIAN_FLOAT_AUTO)
/* architecture detection if we are going automatic for either */
#define NADINE_I_DETECT_ARCH 1
#endif

#if NADINE_I_DETECT_ARCH
/* try to detect CPU architecture from defines */
#if defined(__i386__) || defined(_M_I86) || defined(_M_IX86) || defined(_M_X64)\
        || defined(__X86__) || defined(__amd64__) || defined(__x86_64__)
#define NADINE_I_ARCH_X86 1
#endif
#endif

#if !defined(NADINE_NATIVE_ENDIAN_INT) && NADINE_NATIVE_ENDIAN_INT_AUTO
/* try to auto-detect NADINE_NATIVE_ENDIAN_INT through system headers
   or compiler extensions */
        /* GCC/Clang */
#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__)                \
        && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define NADINE_NATIVE_ENDIAN_INT NADINE_ENDIAN_LITTLE
#elif defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__)                 \
        && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define NADINE_NATIVE_ENDIAN_INT NADINE_ENDIAN_BIG
#elif defined(__BYTE_ORDER__) && defined(__ORDER_PDP_ENDIAN__)                 \
        && __BYTE_ORDER__ == __ORDER_PDP_ENDIAN__
#define NADINE_NATIVE_ENDIAN_INT NADINE_ENDIAN_BIG | NADINE_ENDIAN_SWAPCHARS
        /* Linux, Android, Haiku */
#elif defined(__linux) || defined(__linux__) || defined(__HAIKU__)             \
        || defined(__ANDROID__) || HAVE_ENDIAN_H
#include <endian.h>
#define NADINE_I_CHECK_BYTE_ORDER 1
        /* BSD */
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)      \
        || defined(__DragonFly__) || HAVE_SYS_ENDIAN_H
#include <sys/endian.h>
#define NADINE_I_CHECK_BYTE_ORDER 1
#elif HAVE_SYS_PARAM_H
#include <sys/param.h>
#define NADINE_I_CHECK_BYTE_ORDER 1
        /* used at least by Apple and IBM */
#elif __LITTLE_ENDIAN__ && !__BIG_ENDIAN__
#define NADINE_NATIVE_ENDIAN_INT NADINE_ENDIAN_LITTLE
#elif __BIG_ENDIAN__ && !__LITTLE_ENDIAN__
#define NADINE_NATIVE_ENDIAN_INT NADINE_ENDIAN_BIG
        /* architectures known to have little-endian float order: x86/x64 */
#elif NADINE_I_ARCH_X86
#define NADINE_NATIVE_ENDIAN_FLOAT NADINE_ENDIAN_LITTLE
#endif /* system headers/compiler extensions */
#endif /* NADINE_NATIVE_ENDIAN_INT_AUTO */

#if NADINE_I_CHECK_BYTE_ORDER
#if defined(__BYTE_ORDER) && defined(__LITTLE_ENDIAN)                          \
        && __BYTE_ORDER == __LITTLE_ENDIAN
#define NADINE_NATIVE_ENDIAN_INT NADINE_ENDIAN_LITTLE
#elif defined(__BYTE_ORDER) && defined(__BIG_ENDIAN)                           \
        && __BYTE_ORDER == __BIG_ENDIAN
#define NADINE_NATIVE_ENDIAN_INT NADINE_ENDIAN_BIG
#elif defined(_BYTE_ORDER) && defined(_LITTLE_ENDIAN)                          \
        && _BYTE_ORDER == _LITTLE_ENDIAN
#define NADINE_NATIVE_ENDIAN_INT NADINE_ENDIAN_LITTLE
#elif defined(_BYTE_ORDER) && defined(_BIG_ENDIAN)                             \
        && _BYTE_ORDER == _BIG_ENDIAN
#define NADINE_NATIVE_ENDIAN_INT NADINE_ENDIAN_BIG
#elif defined(BYTE_ORDER) && defined(LITTLE_ENDIAN)                            \
        && BYTE_ORDER == LITTLE_ENDIAN
#define NADINE_NATIVE_ENDIAN_INT NADINE_ENDIAN_LITTLE
#elif defined(BYTE_ORDER) && defined(BIG_ENDIAN)                               \
        && BYTE_ORDER == BIG_ENDIAN
#define NADINE_NATIVE_ENDIAN_INT NADINE_ENDIAN_BIG
#endif
#endif /* NADINE_I_CHECK_BYTE_ORDER */

#if !defined(NADINE_NATIVE_ENDIAN_FLOAT) && NADINE_NATIVE_ENDIAN_FLOAT_AUTO
#if HAVE_IEEEFP_H
#include <ieeefp.h>
#endif
/* try to auto-detect NADINE_NATIVE_ENDIAN_FLOAT through system headers
   or compiler extensions */
        /* GCC (Clang does not have __FLOAT_WORD_ORDER__)*/
#if defined(__FLOAT_WORD_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__)          \
        && __FLOAT_WORD_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define NADINE_NATIVE_ENDIAN_FLOAT NADINE_ENDIAN_LITTLE
#elif defined(__FLOAT_WORD_ORDER__) && defined(__ORDER_BIG_ENDIAN__)           \
        && __FLOAT_WORD_ORDER__ == __ORDER_BIG_ENDIAN__
#define NADINE_NATIVE_ENDIAN_FLOAT NADINE_ENDIAN_BIG
        /* used at least by ? */
#elif __IEEE_LITTLE_ENDIAN && !__IEEE_BIG_ENDIAN
#define NADINE_NATIVE_ENDIAN_FLOAT NADINE_ENDIAN_LITTLE
#elif __IEEE_BIG_ENDIAN && !__IEEE_LITTLE_ENDIAN
#define NADINE_NATIVE_ENDIAN_FLOAT NADINE_ENDIAN_BIG
        /* architectures known to have little-endian float order: x86/x64 */
#elif NADINE_I_ARCH_X86
#define NADINE_NATIVE_ENDIAN_FLOAT NADINE_ENDIAN_LITTLE
#endif /* system headers/compiler extensions */
#endif /* NADINE_NATIVE_ENDIAN_FLOAT_AUTO */

/* detect bi-endian architectures which support little-endian/big-endian 
   through processor flags, memory page, etc. (e.g. SPARC, PowerPC) */
#if defined(__sparc__) || defined(__powerpc__)                                 \
        || defined(_M_PPC) || defined(_XL_PPC)
#define NADINE_I_BIENDIAN 1
#endif

/* static asserts */
#if NADINE_I_C23
#define NADINE_I_STATIC_ASSERT(msg, expr) static_assert(expr, msg)
#elif NADINE_I_C11
#define NADINE_I_STATIC_ASSERT(msg, expr) _Static_assert(expr, msg)
#else
#define NADINE_I_STATIC_ASSERT_I(msg, expr, line)                              \
    struct nadine_static_assert_frame_##line {                                 \
        int static_assert_[sizeof(msg)]; int fail_[(expr) ? 1 : -1];           \
    }
#define NADINE_I_STATIC_ASSERT_E(msg, expr, line)                              \
    NADINE_I_STATIC_ASSERT_I(msg, expr, line)
#define NADINE_I_STATIC_ASSERT(msg, expr)                                      \
    NADINE_I_STATIC_ASSERT_E(msg, expr, __LINE__)
#endif /* static assert define */

/* ensure integral types are unpadded */
NADINE_I_STATIC_ASSERT("unsigned char may not have padding", 
                       UCHAR_MAX >> (CHAR_BIT - 1));
NADINE_I_STATIC_ASSERT("unsigned short may not have padding",
                       USHRT_MAX >> (sizeof(unsigned short) * CHAR_BIT - 1));
NADINE_I_STATIC_ASSERT("unsigned int may not have padding",
                       UINT_MAX  >> (sizeof(unsigned int) * CHAR_BIT - 1));
NADINE_I_STATIC_ASSERT("unsigned long may not have padding",
                       ULONG_MAX >> (sizeof(unsigned long) * CHAR_BIT - 1));
#if NADINE_I_HAS_ULL
NADINE_I_STATIC_ASSERT("unsigned long long may not have padding",
                    ULLONG_MAX >> (sizeof(unsigned long long) * CHAR_BIT - 1));
#endif

#ifndef NADINE_I_INLINE
#if NADINE_I_C99
#define NADINE_I_INLINE inline
#endif
#endif /* NADINE_I_INLINE */

/* function definition specifiers */
#ifdef NADINE_I_INLINE
#define NADINE_I_FNSX static NADINE_I_INLINE
#else
#define NADINE_I_FNSX static
#endif /* NADINE_I_INLINE */

/* do not complain about unused functions, if compiler supports ignoring that */
#if NADINE_I_C23
#define NADINE_I_FNS [[maybe_unused]] NADINE_I_FNSX
#elif __GNUC__ >= 3
#define NADINE_I_FNS __attribute__((unused)) NADINE_I_FNSX
#else
#define NADINE_I_FNS NADINE_I_FNSX
#endif

#if NADINE_STATIC
#define NADINE_I_FN NADINE_I_FNS
#else
#define NADINE_I_FN 
#endif

#if NADINE_MEMCPY
#include <string.h>
#define nadine_i_memcpy memcpy
#endif

#if NADINE_STATIC || NADINE_IMPL

/* swap unsigned char values a and b */
#define NADINE_I_BSWAP(a, b) do { unsigned char t = a; a = b; b = t; } while (0)

/* memcpy implementation */
#if !NADINE_MEMCPY
NADINE_I_FN void nadine_i_memcpy(void *p, const void *s, size_t n) {
    /* cast for C++ compatibility */
    const unsigned char *src = (const unsigned char *)s;
    unsigned char *dst = (unsigned char *)p;
    while (n--) *dst++ = *src++;
}
#endif /* #if !NADINE_MEMCPY */

/* word byte order reversion operations */
#if CHAR_BIT == 8
#if __clang_major__ >= 4 || \
    (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8))
#define NADINE_I_WREV2(T, x) ((T)(__builtin_bswap16((T)(x))))
#define NADINE_I_WREV4(T, x) ((T)(__builtin_bswap32((T)(x))))
#define NADINE_I_WREV8(T, x) ((T)(__builtin_bswap64((T)(x))))
#define NADINE_I_WREV_INTRINSIC 1
#else
/* standard C: reverse 2-char, 4-char, (C99+) 8-char int */
#define NADINE_I_WREV2(T, x) ((T)(                                             \
            ((((T)(x) & 0x00FFU)) << 8U)                                       \
          | ((((T)(x) & 0xFF00U)) >> 8U)))
#define NADINE_I_CW4(x) x##UL
#define NADINE_I_WREV4(T, x) ((T)(                                             \
            ((((T)(x) & NADINE_I_CW4(0x000000FF))) << 24U)                     \
          | ((((T)(x) & NADINE_I_CW4(0x0000FF00))) <<  8U)                     \
          | ((((T)(x) & NADINE_I_CW4(0x00FF0000))) >>  8U)                     \
          | ((((T)(x) & NADINE_I_CW4(0xFF000000))) >> 24U)))
#if NADINE_I_HAS_ULL
#define NADINE_I_CW8(x) x##ULL
#define NADINE_I_WREV8(T, x) ((T)(                                             \
            ((((T)(x) & NADINE_I_CW8(0x00000000000000FF))) << 56U)             \
          | ((((T)(x) & NADINE_I_CW8(0x000000000000FF00))) << 40U)             \
          | ((((T)(x) & NADINE_I_CW8(0x0000000000FF0000))) << 24U)             \
          | ((((T)(x) & NADINE_I_CW8(0x00000000FF000000))) <<  8U)             \
          | ((((T)(x) & NADINE_I_CW8(0x000000FF00000000))) >>  8U)             \
          | ((((T)(x) & NADINE_I_CW8(0x0000FF0000000000))) >> 24U)             \
          | ((((T)(x) & NADINE_I_CW8(0x00FF000000000000))) >> 40U)             \
          | ((((T)(x) & NADINE_I_CW8(0xFF00000000000000))) >> 56U)))
#endif /* NADINE_I_HAS_ULL */
#endif /* stdc */
#endif /* CHAR_BIT == 8 */

/* reverse unsigned char array p[n] */
NADINE_I_FN void nadine_i_memrev(void *p, size_t n) {
    /* cast for C++ compatibility */
    unsigned char *a = (unsigned char *)p, *b;

    /* optimize common cases, not for run-time benefit,
       but in the hope that the compiler understands what we are doing */
    switch (n) {
    case 1:
        return;
    case 2:
        NADINE_I_BSWAP(a[0], a[1]);
        return;
    case 4:
        NADINE_I_BSWAP(a[0], a[3]);
        NADINE_I_BSWAP(a[1], a[2]);
        return;
    case 8:
        NADINE_I_BSWAP(a[0], a[7]);
        NADINE_I_BSWAP(a[1], a[6]);
        NADINE_I_BSWAP(a[2], a[5]);
        NADINE_I_BSWAP(a[3], a[4]);
        return;
    }

    b = &a[n - 1];
    while (a < b) {
        NADINE_I_BSWAP(*a, *b);
        ++a, --b;
    }
}

/* swap char pairs in char array p[n] */
NADINE_I_FN void nadine_i_byteswap(void *p, size_t n) {
    /* cast for C++ compatibility */
    unsigned char *a = (unsigned char *)p;
    unsigned char *b = &a[n - 1];
    while (a < b) {
        NADINE_I_BSWAP(a[0], a[1]);
        a += 2;
    }
}

/* applies needed transformations for XORed endians xf */
NADINE_I_FN void nadine_i_xform(void *p, size_t n, unsigned xf) {
    if (xf & 1) nadine_i_memrev(p, n);
    if (xf & 2) nadine_i_byteswap(p, n);
}

#else

#if !NADINE_MEMCPY
extern void nadine_i_memcpy(void *p, const void *s, size_t n);
#endif /* #if !NADINE_MEMCPY */

extern void nadine_i_memrev(void *p, size_t n);
extern void nadine_i_byteswap(void *p, size_t n);
extern void nadine_i_xform(void *p, size_t n, unsigned xf);

#endif /* #if NADINE_STATIC || NADINE_IMPL */

#ifdef NADINE_I_WREV8
#define NADINE_I_MAYBE_WREV8(T, x) return NADINE_I_WREV8(T, x)
#else /* NADINE_I_WREV8 */
#define NADINE_I_MAYBE_WREV8(T, x) 
#endif /* NADINE_I_WREV8 */

/* convert_from_, convert_to_ aliases */
#define NADINE_I_IMPL_CVTAL(T, N)                                              \
    NADINE_I_FNS T nadine_convert_from_##N(unsigned endian, T value) {         \
        return nadine_convert_##N(endian, value);                              \
    }                                                                          \
    NADINE_I_FNS T nadine_convert_to_##N(unsigned endian, T value) {           \
        return nadine_convert_##N(endian, value);                              \
    }

#if NADINE_STATIC || NADINE_IMPL

#ifndef NADINE_I_TYPE_PUN_UNIONS
#if NADINE_I_C99
/* can type pun through unions in C99 */
#define NADINE_I_TYPE_PUN_UNIONS 1
#else
#define NADINE_I_TYPE_PUN_UNIONS 0
#endif
#endif

/* basic operations: type punning, etc. */
#if NADINE_I_TYPE_PUN_UNIONS

/* use unions if safe */
    /* initializes type punning (for accessing chars) */
#define NADINE_I_MAKE_TYPE_PUNNER(p, T)                                        \
    union { T uv; unsigned char ub[sizeof(T)]; } p
    /* initializes type punner with value v of type T */
#define NADINE_I_TYPE_PUN(p, T, v) p.uv = v
    /* access chars of punned value */
#define NADINE_I_TYPE_ACCESS(p) p.ub
    /* access value of type T back from punner (e.g. after modification) */
#define NADINE_I_TYPE_UNPUN(p, T, v) v = p.uv

/* for bit casting from type T1 to T2 and T2 to T1 */
#define NADINE_I_MAKE_TYPE_ALIASER(p, T1, T2) union { T1 v1; T2 v2; } p
    /* set up aliasing of value v of type T1 */
#define NADINE_I_TYPE_ALIAS_DO(p, T1, T2, v) p.v1 = v
    /* access value of type T2 */
#define NADINE_I_TYPE_ALIASED(p) p.v2
    /* convert aliased value of type T2 back into T1 v */
#define NADINE_I_TYPE_ALIAS_UNDO(p, T1, T2, v) v = p.v1

#else /* NADINE_I_TYPE_PUN_UNIONS */

/* use memcpy if unions are not safe */
    /* initializes type punning (for accessing chars) */
#define NADINE_I_MAKE_TYPE_PUNNER(p, T) unsigned char p[sizeof(T)]
    /* initializes type punner with value v of type T */
#define NADINE_I_TYPE_PUN(p, T, v) do {                                        \
    T v_ = (v); nadine_i_memcpy(p, &v_, sizeof(T)); } while (0)
    /* access chars of punned value */
#define NADINE_I_TYPE_ACCESS(p) p
    /* access value of type T back from punner (e.g. after modification) */
#define NADINE_I_TYPE_UNPUN(p, T, v) nadine_i_memcpy(&v, p, sizeof(T))

/* for bit casting from type T1 to T2 and T2 to T1 */
#define NADINE_I_MAKE_TYPE_ALIASER(p, T1, T2) T2 p
    /* set up aliasing of value v of type T1 */
#define NADINE_I_TYPE_ALIAS_DO(p, T1, T2, v) do {                              \
    T1 v_ = (v); nadine_i_memcpy(&p, &v_, sizeof(T1)); } while (0)
    /* access value of type T2 */
#define NADINE_I_TYPE_ALIASED(p) p
    /* convert aliased value of type T2 back into T1 v */
#define NADINE_I_TYPE_ALIAS_UNDO(p, T1, T2, v)                                 \
    nadine_i_memcpy(&v, &p, sizeof(T1))

#endif /* NADINE_I_TYPE_PUN_UNIONS */

/* define native endianness detection function for integer type T */
#ifdef NADINE_NATIVE_ENDIAN_INT
#define NADINE_I_IMPL_NN_UI(T, N)                                              \
    NADINE_I_FN unsigned nadine_endian_native_##N(void) {                      \
        return (NADINE_NATIVE_ENDIAN_INT);                                     \
    }
#define NADINE_I_NATIVE_INT(T, N) (NADINE_NATIVE_ENDIAN_INT)
#else /* NADINE_NATIVE_ENDIAN_INT */
#define NADINE_I_IMPL_NN_UI(T, N)                                              \
    NADINE_I_FN unsigned nadine_endian_native_##N(void) {                      \
        NADINE_I_MAKE_TYPE_PUNNER(pun, T);                                     \
        /* int value 1 will only have the lowest byte set in IEEE 754 */       \
        NADINE_I_TYPE_PUN(pun, T, (T)1);                                       \
        if (NADINE_I_TYPE_ACCESS(pun)[0])                                      \
            return NADINE_ENDIAN_LITTLE;                                       \
        if (NADINE_I_TYPE_ACCESS(pun)[sizeof(T) - 1])                          \
            return NADINE_ENDIAN_BIG;                                          \
        if (NADINE_I_TYPE_ACCESS(pun)[1])                                      \
            return NADINE_ENDIAN_LITTLE | NADINE_ENDIAN_SWAPCHARS;             \
        if (NADINE_I_TYPE_ACCESS(pun)[sizeof(T) - 2])                          \
            return NADINE_ENDIAN_BIG | NADINE_ENDIAN_SWAPCHARS;                \
        return NADINE_ENDIAN_UNKNOWN;  /* unsupported */                       \
    }
#if NADINE_I_C99
/* on C99 with compound literals, optimize big/little-endian cases */
/* int value 1 will only have the lowest byte set in IEEE 754 */
#define NADINE_I_UNION_PUN_INT(N)                                              \
        (((union { T v; unsigned char b[sizeof(T)]; }){ (T)1 }).b)
#define NADINE_I_NATIVE_INT(T, N) (                                            \
        (NADINE_I_UNION_PUN_INT(T)[0])             ? NADINE_ENDIAN_LITTLE :    \
        (NADINE_I_UNION_PUN_INT(T)[sizeof(T) - 1]) ? NADINE_ENDIAN_BIG    :    \
        nadine_endian_native_##N())
#else /* NADINE_I_C99 */
#define NADINE_I_NATIVE_INT(T, N) nadine_endian_native_##N()
#endif /* NADINE_I_C99 */
#endif /* NADINE_NATIVE_ENDIAN_INT */

#define NADINE_I_IMPL_NN_SI(T, N, TU, NU) NADINE_I_IMPL_NN_UI(T, N)

/* define native endianness detection function for floating-point type T */
#ifdef NADINE_NATIVE_ENDIAN_FLOAT
#define NADINE_I_IMPL_NN_F(T, N)                                               \
    NADINE_I_FN unsigned nadine_endian_native_##N(void) {                      \
        return (NADINE_NATIVE_ENDIAN_FLOAT);                                   \
    }
#define NADINE_I_NATIVE_FLOAT(T, N) (NADINE_NATIVE_ENDIAN_FLOAT)
#else /* NADINE_NATIVE_ENDIAN_FLOAT */
#define NADINE_I_IMPL_NN_F(T, N)                                               \
    NADINE_I_FN unsigned nadine_endian_native_##N(void) {                      \
        NADINE_I_MAKE_TYPE_PUNNER(pun, T);                                     \
        /* float value 2.0 will only have the highest byte set in IEEE 754 */  \
        NADINE_I_TYPE_PUN(pun, T, (T)2.0f);                                    \
        if (NADINE_I_TYPE_ACCESS(pun)[0])                                      \
            return NADINE_ENDIAN_BIG;                                          \
        if (NADINE_I_TYPE_ACCESS(pun)[sizeof(T) - 1])                          \
            return NADINE_ENDIAN_LITTLE;                                       \
        if (NADINE_I_TYPE_ACCESS(pun)[1])                                      \
            return NADINE_ENDIAN_BIG | NADINE_ENDIAN_SWAPCHARS;                \
        if (NADINE_I_TYPE_ACCESS(pun)[sizeof(T) - 2])                          \
            return NADINE_ENDIAN_LITTLE | NADINE_ENDIAN_SWAPCHARS;             \
        return NADINE_ENDIAN_UNKNOWN;  /* unsupported */                       \
    }
#if NADINE_I_C99
/* on C99 with compound literals, optimize big/little-endian cases */
/* float value 2.0 will only have the highest byte set in IEEE 754 */
#define NADINE_I_UNION_PUN_FLOAT(N)                                            \
        (((union { T v; unsigned char b[sizeof(T)]; }){ (T)2.0 }).b)
#define NADINE_I_NATIVE_FLOAT(T, N) (                                          \
        (NADINE_I_UNION_PUN_FLOAT(T)[sizeof(T) - 1]) ? NADINE_ENDIAN_LITTLE :  \
        (NADINE_I_UNION_PUN_FLOAT(T)[0])             ? NADINE_ENDIAN_BIG    :  \
        nadine_endian_native_##N())
#else /* NADINE_I_C99 */
#define NADINE_I_NATIVE_FLOAT(T, N) nadine_endian_native_##N()
#endif /* NADINE_I_C99 */
#endif /* NADINE_NATIVE_ENDIAN_FLOAT */

/* define conversion function for unsigned integer type T */
#define NADINE_I_IMPL_CVT_UI(T, N)                                             \
    NADINE_I_FN T nadine_convert_##N(unsigned endian, T value) {               \
        const unsigned native = NADINE_I_NATIVE_INT(T, N);                     \
        /* special case #1 */                                                  \
        if (native == endian || sizeof(T) == 1) return value;                  \
        /* special case #2: only need to reverse */                            \
        if ((native ^ endian) == 1 && CHAR_BIT == 8) {                         \
            switch (sizeof(T)) {                                               \
                case 2: return NADINE_I_WREV2(T, value);                       \
                case 4: return NADINE_I_WREV4(T, value);                       \
                case 8: NADINE_I_MAYBE_WREV8(T, value);                        \
            }                                                                  \
        }                                                                      \
        {                                                                      \
            NADINE_I_MAKE_TYPE_PUNNER(pun, T);                                 \
            NADINE_I_TYPE_PUN(pun, T, value);                                  \
            nadine_i_xform(NADINE_I_TYPE_ACCESS(pun), sizeof(T),               \
                            native ^ endian);                                  \
            NADINE_I_TYPE_UNPUN(pun, T, value);                                \
            return value;                                                      \
        }                                                                      \
    }

/* define conversion function for signed integer type T with unsigned TU */
#define NADINE_I_IMPL_CVT_SI(T, N, TU, NU)                                     \
    NADINE_I_FN T nadine_convert_##N(unsigned endian, T value) {               \
        NADINE_I_MAKE_TYPE_ALIASER(u, T, TU);                                  \
        NADINE_I_TYPE_ALIAS_DO(u, T, TU, value);                               \
        NADINE_I_TYPE_ALIASED(u) = nadine_convert_##NU(endian,                 \
                    NADINE_I_TYPE_ALIASED(u));                                 \
        NADINE_I_TYPE_ALIAS_UNDO(u, T, TU, value);                             \
        return value;                                                          \
    }

/* define conversion function for floating-point type T */
#define NADINE_I_IMPL_CVT_F(T, N)                                              \
    NADINE_I_FN T nadine_convert_##N(unsigned endian, T value) {               \
        const unsigned native = NADINE_I_NATIVE_FLOAT(T, N);                   \
        NADINE_I_MAKE_TYPE_PUNNER(pun, T);                                     \
        NADINE_I_TYPE_PUN(pun, T, value);                                      \
        nadine_i_xform(NADINE_I_TYPE_ACCESS(pun), sizeof(T), native ^ endian); \
        NADINE_I_TYPE_UNPUN(pun, T, value);                                    \
        return value;                                                          \
    }

/* use shift-based read/write only when inlining, or if on a bi-endian arch */
#if defined(NADINE_I_INLINE) || NADINE_I_BIENDIAN
#define NADINE_I_USE_SHIFT_RW 1
#else
#define NADINE_I_USE_SHIFT_RW 0
#endif

/* define read/write function for type T */
#if NADINE_I_USE_SHIFT_RW
#define NADINE_I_INDEX(endian, i)                                              \
    (((endian & 1) ? (n - i - 1) : i) ^ ((endian & 2) ? 1 : 0))
#define NADINE_I_IMPL_RW_UI(T, N)                                              \
    NADINE_I_FN T nadine_read_##N(unsigned endian, const void *s) {            \
        /* cast for C++ compatibility */                                       \
        const unsigned char *src = (const unsigned char *)s;                   \
        size_t i, n = sizeof(T);                                               \
        T v = 0;                                                               \
        for (i = 0; i < n; ++i)                                                \
            v |= (T)(src[NADINE_I_INDEX(endian, i)]) << (CHAR_BIT * i);        \
        return v;                                                              \
    }                                                                          \
    NADINE_I_FN void nadine_write_##N(unsigned endian, void *d, T v) {         \
        /* cast for C++ compatibility */                                       \
        unsigned char *dst = (unsigned char *)d;                               \
        size_t i, n = sizeof(T);                                               \
        for (i = 0; i < n; ++i)                                                \
            dst[NADINE_I_INDEX(endian, i)] =                                   \
                        (unsigned char)(v >> (CHAR_BIT * i));                  \
    }
#else
#define NADINE_I_IMPL_RW_UI(T, N)                                              \
    NADINE_I_FN T nadine_read_##N(unsigned endian, const void *s) {            \
        const unsigned native = NADINE_I_NATIVE_INT(T, N);                     \
        T v;                                                                   \
        NADINE_I_MAKE_TYPE_PUNNER(p, T);                                       \
        nadine_i_memcpy(NADINE_I_TYPE_ACCESS(p), s, sizeof(T));                \
        nadine_i_xform(NADINE_I_TYPE_ACCESS(p), sizeof(T), native ^ endian);   \
        NADINE_I_TYPE_UNPUN(p, T, v);                                          \
        return v;                                                              \
    }                                                                          \
    NADINE_I_FN void nadine_write_##N(unsigned endian, void *d, T v) {         \
        const unsigned native = NADINE_I_NATIVE_INT(T, N);                     \
        NADINE_I_MAKE_TYPE_PUNNER(p, T);                                       \
        NADINE_I_TYPE_PUN(p, T, v);                                            \
        nadine_i_xform(NADINE_I_TYPE_ACCESS(p), sizeof(T), native ^ endian);   \
        nadine_i_memcpy(d, NADINE_I_TYPE_ACCESS(p), sizeof(T));                \
    }
#endif

#define NADINE_I_IMPL_RW_SI(T, N, TU, NU) NADINE_I_IMPL_RW_UI(T, N)

#define NADINE_I_IMPL_RW_F(T, N)                                               \
    NADINE_I_FN T nadine_read_##N(unsigned endian, const void *s) {            \
        const unsigned native = NADINE_I_NATIVE_FLOAT(T, N);                   \
        T v;                                                                   \
        NADINE_I_MAKE_TYPE_PUNNER(p, T);                                       \
        nadine_i_memcpy(NADINE_I_TYPE_ACCESS(p), s, sizeof(T));                \
        nadine_i_xform(NADINE_I_TYPE_ACCESS(p), sizeof(T), native ^ endian);   \
        NADINE_I_TYPE_UNPUN(p, T, v);                                          \
        return v;                                                              \
    }                                                                          \
    NADINE_I_FN void nadine_write_##N(unsigned endian, void *d, T v) {         \
        const unsigned native = NADINE_I_NATIVE_FLOAT(T, N);                   \
        NADINE_I_MAKE_TYPE_PUNNER(p, T);                                       \
        NADINE_I_TYPE_PUN(p, T, v);                                            \
        nadine_i_xform(NADINE_I_TYPE_ACCESS(p), sizeof(T), native ^ endian);   \
        nadine_i_memcpy(d, NADINE_I_TYPE_ACCESS(p), sizeof(T));                \
    }


/* define the five basic functions for T when T is an unsigned integer type */
#define NADINE_I_IMPL_UI(T, N)                                                 \
    NADINE_I_IMPL_NN_UI(T, N)                                                  \
    NADINE_I_IMPL_CVT_UI(T, N)                                                 \
    NADINE_I_IMPL_CVTAL(T, N)                                                  \
    NADINE_I_IMPL_RW_UI(T, N)

/* define the five basic functions for T when T is a signed integer type */
#define NADINE_I_IMPL_SI(T, N, TU, NU)                                         \
    NADINE_I_IMPL_NN_SI(T, N, TU, NU)                                          \
    NADINE_I_IMPL_CVT_SI(T, N, TU, NU)                                         \
    NADINE_I_IMPL_CVTAL(T, N)                                                  \
    NADINE_I_IMPL_RW_SI(T, N, TU, NU)

/* define the five basic functions for T when T is a floating-point type */
#define NADINE_I_IMPL_F(T, N)                                                  \
    NADINE_I_IMPL_NN_F(T, N)                                                   \
    NADINE_I_IMPL_CVT_F(T, N)                                                  \
    NADINE_I_IMPL_CVTAL(T, N)                                                  \
    NADINE_I_IMPL_RW_F(T, N)

#else /* NADINE_STATIC || NADINE_IMPL */

/* declare the five basic functions for T */
#define NADINE_I_DECLARE(T, N)                                                 \
    extern T nadine_convert_##N(unsigned endian, T value);                     \
    extern T nadine_read_##N(unsigned endian, const void *source);             \
    extern void nadine_write_##N(unsigned endian, void *destination, T value); \
    extern unsigned nadine_endian_native_##N(void);                            \
    NADINE_I_IMPL_CVTAL(T, N)

#define NADINE_I_IMPL_UI(T, N) NADINE_I_DECLARE(T, N)
#define NADINE_I_IMPL_SI(T, N, TU, NU) NADINE_I_DECLARE(T, N)
#define NADINE_I_IMPL_F(T, N) NADINE_I_DECLARE(T, N)

#endif /* NADINE_STATIC || NADINE_IMPL */

#define NADINE_I_IMPL_I(TS, NS, TU, NU)                                        \
    NADINE_I_IMPL_UI(TU, NU)                                                   \
    NADINE_I_IMPL_SI(TS, NS, TU, NU)

/* data types */
NADINE_I_IMPL_I(short, short, unsigned short, unsigned_short)
NADINE_I_IMPL_I(int, int, unsigned int, unsigned_int)
NADINE_I_IMPL_I(long, long, unsigned long, unsigned_long)
#if NADINE_I_HAS_ULL
NADINE_I_IMPL_I(long long, long_long, unsigned long long, unsigned_long_long)
#endif

#if NADINE_STDINT
#if defined(UINT16_MAX) && defined(INT16_MAX)
NADINE_I_IMPL_I(int16_t, int16, uint16_t, uint16)
#endif
#if defined(UINT32_MAX) && defined(INT32_MAX)
NADINE_I_IMPL_I(int32_t, int32, uint32_t, uint32)
#endif
#if defined(UINT64_MAX) && defined(INT64_MAX)
NADINE_I_IMPL_I(int64_t, int64, uint64_t, uint64)
#endif
#endif

#if NADINE_FLOAT
NADINE_I_IMPL_F(float, float)
NADINE_I_IMPL_F(double, double)
#endif

#ifdef __cplusplus
}
#endif

#endif /* NADINE_H */
