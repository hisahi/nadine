
# nadine

nadine is a platform-independent single-header C library for
endianness conversion.

It is compatible with ANSI C and above, provided that the following
requirements are met:

* system endianness (for integer and floating-point types, but not necessarily
  the same for them) is one of the following:
    * little-endian
    * big-endian
    * little-endian for 2 char units, but big-endian past 2-char pairs (PDP)
    * big-endian for 2 char units, but little-endian past 2-char pairs
* for integer types:
    * all bits are significant; there are no padding or parity bits
* for floating-point types:
    * floating-point types use IEEE 754 compatible encoding

If these requirements are met, then you can use `nadine.h`. For correct use,
exactly one source file should define `NADINE_IMPL` prior to including
`nadine.h` in order to implement the functions that nadine needs to work.
(Alternatively, define `NADINE_STATIC` in every file before including
`nadine.h`, if you can trust your compiler to deduplicate these functions.)

`nadine.h` does not require any hosted headers to function, although it will
try to use `memcpy` if it is available.

## API

For representing endianness, nadine supports the following constants
* `NADINE_ENDIAN_LITTLE` = 0
    * Little-endian: `char`s are in order from least to most significant.
* `NADINE_ENDIAN_BIG` = 1
    * Big-endian: `char`s are in order from most to least significant.

...combined with any number of the following:
* `NADINE_ENDIAN_SWAPCHARS` = 2
    * Swap adjacent `char` pairs.

Examples:
* `NADINE_ENDIAN_BIG | NADINE_ENDIAN_SWAPCHARS` is the PDP-11 byte order
* `NADINE_ENDIAN_LITTLE | NADINE_ENDIAN_SWAPCHARS` is the
  Honeywell 316 byte order

nadine provides four sets of functions for a variety of types.
* _T_ `nadine_convert_`_N_`(unsigned endian, `_T_` value)`
    * Converts the given value to or from the given endianness.
      `endian` must be a valid endianness value, or the behavior is undefined.
    * Aliases of this function (as macros) are also provided with the same
      names, but `convert` replaced with `convert_from` and `convert_to`.
      Since endianness conversions are involutions, these functions have
      identical behavior, and their purpose is merely to allow for
      self-documenting code.
* _T_ `nadine_read_`_N_`(unsigned endian, const void *source)`
    * Reads an integer or floating-point value of type _T_ at the given pointer
      with the specified endianness and returns it. This function results in
      undefined behavior if `source` does not contain at least `sizeof(T)`
      characters of information. `endian` must be a valid endianness value,
      or the behavior is undefined.
* `void nadine_write_`_N_`(unsigned endian, void *destination, `_T_` value)`
    * Writes an integer or floating-point value of type _T_ at the given pointer
      with the specified endianness. This function results in undefined behavior
      if `destination` does not have space for at least `sizeof(T)` characters
      of information. `endian` must be a valid endianness value,
      or the behavior is undefined.
* `unsigned nadine_endian_native_`_N_`(void)`
    * Returns the native endianness of the system for the type
      corresponding to _N_.
    * The return value is always either a valid `endian` value,
      or `NADINE_ENDIAN_UNKNOWN` (not a valid `endian`)
      if the endianness is unsupported.
    * If compiler support is available, endianness may be detected
      automatically, in which case this may instead be implemented
      as a preprocessor macro.

Internal functions are prefixed with `nadine_i_`.

Note that functions in the public API are not guaranteed to have
external linkage. They are not even guaranteed to be actual functions
rather than preprocessor macros.

These sets of functions are provided for the following types:

| _T_                  | _N_                  | Notes                          |
| -------------------- | -------------------- | ------------------------------ |
| `short`              | `short`              |                                |
| `unsigned short`     | `unsigned_short`     |                                |
| `int`                | `int`                |                                |
| `unsigned int`       | `unsigned_int`       |                                |
| `long`               | `long`               |                                |
| `unsigned long`      | `unsigned_long`      |                                |
| `long long`          | `long_long`          | C99 and above only             |
| `unsigned long long` | `unsigned_long_long` | C99 and above only             |
| `int8_t`             | `int8`               | only with `stdint.h`           |
| `uint8_t`            | `uint8`              | only with `stdint.h`           |
| `int16_t`            | `int16`              | only with `stdint.h`           |
| `uint16_t`           | `uint16`             | only with `stdint.h`           |
| `int32_t`            | `int32`              | only with `stdint.h`           |
| `uint32_t`           | `uint32`             | only with `stdint.h`           |
| `int64_t`            | `int64`              | only with `stdint.h`           |
| `uint64_t`           | `uint64`             | only with `stdint.h`           |
| `float`              | `float`              | only with IEEE 754             |
| `double`             | `double`             | only with IEEE 754             |

Example of a full name under this scheme: `nadine_convert_int`

## Detecting native endianness

By default, nadine will try to detect the native endianness through compiler
features or platform headers. This can be disabled by defining
`NADINE_NATIVE_ENDIAN_INT_AUTO` (for integers) and/or
`NADINE_NATIVE_ENDIAN_FLOAT_AUTO` (for floating-point types) as 0.

If the endianness is known and should be specified in advance, you can
define `NADINE_NATIVE_ENDIAN_INT` (for integers) and
`NADINE_NATIVE_ENDIAN_FLOAT` (for floating-point types) with one
of the endianness values as described above.

## Floating-point types

nadine supports floating-point types only if the floating-point types use an
IEEE 754 compatible encoding. This is detected automatically. Define
`NADINE_FLOAT` as `1` to always enable, or as `0` to always disable.

## stdint.h

Support for fixed-width integer types is enabled by default only if compiling
on C99 or above. Define `NADINE_STDINT` as `1` to always enable,
or as `0` to always disable.

## FAQ

* **Q**: Why "nadine"?
    * **A**: "endian" reversed is "naidne", and "nadine" sounds a lot better.
* **Q**: It's not really C89 compatible, is it?
    * **A**: It is, as long as your compiler supports more than the minimum
      of 6 significant initial characters in external symbols. Most libraries
      claiming to be ANSI C compatible do not care about this limitation either.
* **Q**: Is there an explicit byte order swap function?
    * **A**: No. nadine assumes that you know the endianness you are supposed
      to be using. For example, instead of byte-swapping big-endian data when
      writing code for a little-endian system, instead use the appropriate
      read, write or convert functions with the endianness you need.
      Their behavior will be correct regardless of what endianness the
      architecture you compile your code into will be using (as long as
      nadine supports it).

      If you insist, you can force an explicit byte order swap by
      taking the native endianness, inverting the lowest bit with `^ 1` and
      using the convert function with that endianness.
* **Q**: What about performance?
    * **A**: nadine is designed for correctness, robustness and portability
      over performance. However, there are some optimizations for common cases,
      and most good compilers are able to pick up on these when compiling
      with optimizations (such as by recognizing nadine's pattern for detecting
      the system's native endianness and then pruning unreachable branches,
      as well as recognizing some of the internal operations for what they do).

## License

nadine is dual-licensed under the Unlicense (public domain equivalent)
and the MIT license.

## Examples

### Read little endian uint32_t from buffer
```c
const char buffer[4] = { 4, 3, 2, 1 };
uint32_t value = nadine_read_uint32(NADINE_ENDIAN_LITTLE, buffer);
/* value = 0x01020304 */
```

### Write big endian int32_t to buffer
```c
char buffer[4];
nadine_write_uint32(NADINE_ENDIAN_BIG, &buffer, 0x01020304UL);
/* buffer = { 1, 2, 3, 4 } */
```

## Tests

The included `nadine_test.c` tests aspects of the nadine library on
fixed-width integer types (`stdint.h` support is required). It should compile
on any platform that has the C standard library, including `stdint.h`,
available for use by applications.
