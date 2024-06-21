/* TEST PROGRAM FOR NADINE */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define NADINE_STATIC 1
#include "nadine.h"

#if CHAR_BIT != 8
#error test program requires CHAR_BIT 8
#endif

#if __GNUC__ >= 3
__attribute__((unused))
#endif
static void hexdump(const void *p, size_t n) {
    size_t i;
    const unsigned char *c = p;
    for (i = 0; i < n; ++i) printf("%02x ", c[i]);
}

static int verify(int line, int cond, const char *msg) {
    if (!cond) {
        printf("Assert '%s' failed on line %u.\n", msg, line);
        return 1;
    }
    return 0;
}

#define VERIFY(cond, msg) verify(__LINE__, cond, msg)

static int test_uint16(void) {
    int failed = 0;

    uint16_t vbe = UINT16_C(0x0102), vle = UINT16_C(0x0201);
    unsigned char buf[sizeof(vbe)];
    int big;

    nadine_i_memcpy(buf, &vbe, sizeof(buf));
    big = buf[0] < buf[1];

    if (big) {
        puts("u16 host=BE");
        failed += VERIFY(
                nadine_convert_uint16(NADINE_ENDIAN_BIG, vbe) == vbe,
                "u16 host=BE, target=BE, should not swap");
        failed += VERIFY(
                nadine_convert_uint16(NADINE_ENDIAN_LITTLE, vbe) == vle,
                "u16 host=BE, target=LE, should swap");
    } else {
        puts("u16 host=LE");
        failed += VERIFY(
                nadine_endian_native_uint16() == NADINE_ENDIAN_LITTLE,
                "u16 host=LE native");
        failed += VERIFY(
                nadine_convert_uint16(NADINE_ENDIAN_BIG, vbe) == vle,
                "u16 host=LE, target=BE, should swap");
        failed += VERIFY(
                nadine_convert_uint16(NADINE_ENDIAN_LITTLE, vbe) == vbe,
                "u16 host=LE, target=LE, should not swap");
    }

    buf[0] = 1, buf[1] = 2;
    failed += VERIFY(nadine_read_uint16(NADINE_ENDIAN_BIG, buf) == vbe,
                     "u16 read BE fail");
    failed += VERIFY(nadine_read_uint16(NADINE_ENDIAN_LITTLE, buf) == vle,
                     "u16 read LE fail");

    nadine_write_uint16(NADINE_ENDIAN_BIG, buf, vbe);
    failed += VERIFY(buf[0] == 1 && buf[1] == 2, "u16 write BE fail");

    nadine_write_uint16(NADINE_ENDIAN_LITTLE, buf, vbe);
    failed += VERIFY(buf[0] == 2 && buf[1] == 1, "u16 write LE fail");

    return failed;
}

static int test_int16(void) {
    int failed = 0;

    int16_t vbe = INT16_C(0x0102), vle = INT16_C(0x0201);
    unsigned char buf[sizeof(vbe)];
    int big;

    nadine_i_memcpy(buf, &vbe, sizeof(buf));
    big = buf[0] < buf[1];

    if (big) {
        puts("s16 host=BE");
        failed += VERIFY(
                nadine_endian_native_int16() == NADINE_ENDIAN_BIG,
                "i16 host=BE native");
        failed += VERIFY(
                nadine_convert_int16(NADINE_ENDIAN_BIG, vbe) == vbe,
                "i16 host=BE, target=BE, should not swap");
        failed += VERIFY(
                nadine_convert_int16(NADINE_ENDIAN_LITTLE, vbe) == vle,
                "i16 host=BE, target=LE, should swap");
    } else {
        puts("s16 host=LE");
        failed += VERIFY(
                nadine_endian_native_int16() == NADINE_ENDIAN_LITTLE,
                "i16 host=LE native");
        failed += VERIFY(
                nadine_convert_int16(NADINE_ENDIAN_BIG, vbe) == vle,
                "i16 host=LE, target=BE, should swap");
        failed += VERIFY(
                nadine_convert_int16(NADINE_ENDIAN_LITTLE, vbe) == vbe,
                "i16 host=LE, target=LE, should not swap");
    }

    buf[0] = 1, buf[1] = 2;
    failed += VERIFY(nadine_read_int16(NADINE_ENDIAN_BIG, buf) == vbe,
                     "i16 read BE fail");
    failed += VERIFY(nadine_read_int16(NADINE_ENDIAN_LITTLE, buf) == vle,
                     "i16 read LE fail");

    nadine_write_int16(NADINE_ENDIAN_BIG, buf, vbe);
    failed += VERIFY(buf[0] == 1 && buf[1] == 2, "i16 write BE fail");

    nadine_write_int16(NADINE_ENDIAN_LITTLE, buf, vbe);
    failed += VERIFY(buf[0] == 2 && buf[1] == 1, "i16 write LE fail");

    return failed;
}

static int test_uint32(void) {
    int failed = 0;

    uint32_t vbe = UINT32_C(0x01020304),
             vle = UINT32_C(0x04030201);
    unsigned char buf[sizeof(vbe)];
    int big, straight;

    nadine_i_memcpy(buf, &vbe, sizeof(buf));
    big = buf[0] < buf[1];
    straight = big ? buf[1] < buf[2] : buf[1] > buf[2];

    if (straight) {
        if (big) {
            puts("u32 host=BE");
            failed += VERIFY(
                    nadine_endian_native_uint32() == NADINE_ENDIAN_BIG,
                    "u32 host=BE native");
            failed += VERIFY(
                    nadine_convert_uint32(NADINE_ENDIAN_BIG, vbe) == vbe,
                    "u32 host=BE, target=BE, should not swap");
            failed += VERIFY(
                    nadine_convert_uint32(NADINE_ENDIAN_LITTLE, vbe) == vle,
                    "u32 host=BE, target=LE, should swap");
        } else {
            puts("u32 host=LE");
            failed += VERIFY(
                    nadine_endian_native_uint32() == NADINE_ENDIAN_LITTLE,
                    "u32 host=LE native");
            failed += VERIFY(
                    nadine_convert_uint32(NADINE_ENDIAN_BIG, vbe) == vle,
                    "u32 host=LE, target=BE, should swap");
            failed += VERIFY(
                    nadine_convert_uint32(NADINE_ENDIAN_LITTLE, vbe) == vbe,
                    "u32 host=LE, target=LE, should not swap");
        }
    } else {
        puts("u32 host=mixed");
    }

    buf[0] = 1, buf[1] = 2, buf[2] = 3, buf[3] = 4;
    failed += VERIFY(nadine_read_uint32(NADINE_ENDIAN_BIG, buf) == vbe,
                     "u32 read BE fail");
    failed += VERIFY(nadine_read_uint32(NADINE_ENDIAN_LITTLE, buf) == vle,
                     "u32 read LE fail");

    nadine_write_uint32(NADINE_ENDIAN_BIG, buf, vbe);
    failed += VERIFY(buf[0] == 1 && buf[1] == 2
                  && buf[2] == 3 && buf[3] == 4, "u32 write BE fail");

    nadine_write_uint32(NADINE_ENDIAN_LITTLE, buf, vbe);
    failed += VERIFY(buf[0] == 4 && buf[1] == 3
                  && buf[2] == 2 && buf[3] == 1, "u32 write LE fail");

    nadine_write_uint32(NADINE_ENDIAN_BIG | NADINE_ENDIAN_SWAPCHARS, buf, vbe);
    failed += VERIFY(buf[0] == 2 && buf[1] == 1
                  && buf[2] == 4 && buf[3] == 3, "u32 write PDP fail");

    nadine_write_uint32(NADINE_ENDIAN_LITTLE | NADINE_ENDIAN_SWAPCHARS, buf, vbe);
    failed += VERIFY(buf[0] == 3 && buf[1] == 4
                  && buf[2] == 1 && buf[3] == 2, "u32 write 316 fail");

    return failed;
}

static int test_int32(void) {
    int failed = 0;

    int32_t vbe = INT32_C(0x01020304),
            vle = INT32_C(0x04030201);
    unsigned char buf[sizeof(vbe)];
    int big, straight;

    nadine_i_memcpy(buf, &vbe, sizeof(buf));
    big = buf[0] < buf[1];
    straight = big ? buf[1] < buf[2] : buf[1] > buf[2];

    if (straight) {
        if (big) {
            puts("s32 host=BE");
            failed += VERIFY(
                    nadine_endian_native_int32() == NADINE_ENDIAN_BIG,
                    "i32 host=BE native");
            failed += VERIFY(
                    nadine_convert_int32(NADINE_ENDIAN_BIG, vbe) == vbe,
                    "i32 host=BE, target=BE, should not swap");
            failed += VERIFY(
                    nadine_convert_int32(NADINE_ENDIAN_LITTLE, vbe) == vle,
                    "i32 host=BE, target=LE, should swap");
        } else {
            puts("s32 host=LE");
            failed += VERIFY(
                    nadine_endian_native_int32() == NADINE_ENDIAN_LITTLE,
                    "i32 host=LE native");
            failed += VERIFY(
                    nadine_convert_int32(NADINE_ENDIAN_BIG, vbe) == vle,
                    "i32 host=LE, target=BE, should swap");
            failed += VERIFY(
                    nadine_convert_int32(NADINE_ENDIAN_LITTLE, vbe) == vbe,
                    "i32 host=LE, target=LE, should not swap");
        }
    } else {
        puts("s32 host=mixed");
    }

    buf[0] = 1, buf[1] = 2, buf[2] = 3, buf[3] = 4;
    failed += VERIFY(nadine_read_int32(NADINE_ENDIAN_BIG, buf) == vbe,
                     "i32 read BE fail");
    failed += VERIFY(nadine_read_int32(NADINE_ENDIAN_LITTLE, buf) == vle,
                     "i32 read LE fail");

    nadine_write_int32(NADINE_ENDIAN_BIG, buf, vbe);
    failed += VERIFY(buf[0] == 1 && buf[1] == 2
                  && buf[2] == 3 && buf[3] == 4, "i32 write BE fail");

    nadine_write_int32(NADINE_ENDIAN_LITTLE, buf, vbe);
    failed += VERIFY(buf[0] == 4 && buf[1] == 3
                  && buf[2] == 2 && buf[3] == 1, "i32 write LE fail");

    nadine_write_int32(NADINE_ENDIAN_BIG | NADINE_ENDIAN_SWAPCHARS, buf, vbe);
    failed += VERIFY(buf[0] == 2 && buf[1] == 1
                  && buf[2] == 4 && buf[3] == 3, "i32 write PDP fail");

    nadine_write_int32(NADINE_ENDIAN_LITTLE | NADINE_ENDIAN_SWAPCHARS, buf, vbe);
    failed += VERIFY(buf[0] == 3 && buf[1] == 4
                  && buf[2] == 1 && buf[3] == 2, "i32 write 316 fail");

    return failed;
}

static int test_uint64(void) {
    int failed = 0;

    uint64_t vbe = UINT64_C(0x0102030405060708),
             vle = UINT64_C(0x0807060504030201);
    unsigned char buf[sizeof(vbe)];
    int big, straight;

    nadine_i_memcpy(buf, &vbe, sizeof(buf));
    big = buf[0] < buf[1];
    straight = big ? buf[3] < buf[4] : buf[3] > buf[4];

    if (straight) {
        if (big) {
            puts("u64 host=BE");
            failed += VERIFY(
                    nadine_endian_native_uint64() == NADINE_ENDIAN_BIG,
                    "u64 host=BE native");
            failed += VERIFY(
                    nadine_convert_uint64(NADINE_ENDIAN_BIG, vbe) == vbe,
                    "u64 host=BE, target=BE, should not swap");
            failed += VERIFY(
                    nadine_convert_uint64(NADINE_ENDIAN_LITTLE, vbe) == vle,
                    "u64 host=BE, target=LE, should swap");
        } else {
            puts("u64 host=LE");
            failed += VERIFY(
                    nadine_endian_native_uint64() == NADINE_ENDIAN_LITTLE,
                    "u64 host=LE native");
            failed += VERIFY(
                    nadine_convert_uint64(NADINE_ENDIAN_BIG, vbe) == vle,
                    "u64 host=LE, target=BE, should swap");
            failed += VERIFY(
                    nadine_convert_uint64(NADINE_ENDIAN_LITTLE, vbe) == vbe,
                    "u64 host=LE, target=LE, should not swap");
        }
    } else {
        puts("u64 host=mixed");
    }

    buf[0] = 1, buf[1] = 2, buf[2] = 3, buf[3] = 4;
    buf[4] = 5, buf[5] = 6, buf[6] = 7, buf[7] = 8;
    failed += VERIFY(nadine_read_uint64(NADINE_ENDIAN_BIG, buf) == vbe,
                     "u64 read BE fail");
    failed += VERIFY(nadine_read_uint64(NADINE_ENDIAN_LITTLE, buf) == vle,
                     "u64 read LE fail");

    nadine_write_uint64(NADINE_ENDIAN_BIG, buf, vbe);
    failed += VERIFY(buf[0] == 1 && buf[1] == 2
                  && buf[2] == 3 && buf[3] == 4
                  && buf[4] == 5 && buf[5] == 6
                  && buf[6] == 7 && buf[7] == 8, "u64 write BE fail");

    nadine_write_uint64(NADINE_ENDIAN_LITTLE, buf, vbe);
    failed += VERIFY(buf[0] == 8 && buf[1] == 7
                  && buf[2] == 6 && buf[3] == 5
                  && buf[4] == 4 && buf[5] == 3
                  && buf[6] == 2 && buf[7] == 1, "u64 write LE fail");

    return failed;
}

static int test_int64(void) {
    int failed = 0;

    int64_t vbe = INT64_C(0x0102030405060708),
            vle = INT64_C(0x0807060504030201);
    unsigned char buf[sizeof(vbe)];
    int big, straight;

    nadine_i_memcpy(buf, &vbe, sizeof(buf));
    big = buf[0] < buf[1];
    straight = big ? buf[3] < buf[4] : buf[3] > buf[4];

    if (straight) {
        if (big) {
            puts("s64 host=BE");
            failed += VERIFY(
                    nadine_endian_native_int64() == NADINE_ENDIAN_BIG,
                    "i64 host=BE native");
            failed += VERIFY(
                    nadine_convert_int64(NADINE_ENDIAN_BIG, vbe) == vbe,
                    "i64 host=BE, target=BE, should not swap");
            failed += VERIFY(
                    nadine_convert_int64(NADINE_ENDIAN_LITTLE, vbe) == vle,
                    "i64 host=BE, target=LE, should swap");
        } else {
            puts("s64 host=LE");
            failed += VERIFY(
                    nadine_endian_native_int64() == NADINE_ENDIAN_LITTLE,
                    "i64 host=LE native");
            failed += VERIFY(
                    nadine_convert_int64(NADINE_ENDIAN_BIG, vbe) == vle,
                    "i64 host=LE, target=BE, should swap");
            failed += VERIFY(
                    nadine_convert_int64(NADINE_ENDIAN_LITTLE, vbe) == vbe,
                    "i64 host=LE, target=LE, should not swap");
        }
    } else {
        puts("s64 host=mixed");
    }

    buf[0] = 1, buf[1] = 2, buf[2] = 3, buf[3] = 4;
    buf[4] = 5, buf[5] = 6, buf[6] = 7, buf[7] = 8;
    failed += VERIFY(nadine_read_int64(NADINE_ENDIAN_BIG, buf) == vbe,
                     "i64 read BE fail");
    failed += VERIFY(nadine_read_int64(NADINE_ENDIAN_LITTLE, buf) == vle,
                     "i64 read LE fail");

    nadine_write_int64(NADINE_ENDIAN_BIG, buf, vbe);
    failed += VERIFY(buf[0] == 1 && buf[1] == 2
                  && buf[2] == 3 && buf[3] == 4
                  && buf[4] == 5 && buf[5] == 6
                  && buf[6] == 7 && buf[7] == 8, "i64 write BE fail");

    nadine_write_int64(NADINE_ENDIAN_LITTLE, buf, vbe);
    failed += VERIFY(buf[0] == 8 && buf[1] == 7
                  && buf[2] == 6 && buf[3] == 5
                  && buf[4] == 4 && buf[5] == 3
                  && buf[6] == 2 && buf[7] == 1, "i64 write LE fail");

    return failed;
}

#if NADINE_FLOAT
static int test_float(void) {
    int failed = 0;

    float val = 7.0;
    unsigned char buf[sizeof(val)];

    nadine_i_memcpy(buf, &val, sizeof(buf));
    switch (nadine_endian_native_float()) {
    case NADINE_ENDIAN_LITTLE:
        puts("f32 host=LE");
        failed += VERIFY(buf[0] == 0x00 && buf[1] == 0x00
                      && buf[2] == 0xE0 && buf[3] == 0x40,
                      "f32 host=LE mismatch");
        break;
    case NADINE_ENDIAN_BIG:
        puts("f32 host=BE");
        failed += VERIFY(buf[0] == 0x40 && buf[1] == 0xE0
                      && buf[2] == 0x00 && buf[3] == 0x00,
                      "f32 host=BE mismatch");
        break;
    default:
        puts("f32 host=mixed - bug or rare platform?");
        break;
    }

    buf[0] = 0x40; buf[1] = 0xE0; buf[2] = 0x00; buf[3] = 0x00;
    failed += VERIFY(nadine_read_float(NADINE_ENDIAN_BIG, buf) == val,
                     "f32 read BE fail");

    buf[0] = 0x00; buf[1] = 0x00; buf[2] = 0xE0; buf[3] = 0x40;
    failed += VERIFY(nadine_read_float(NADINE_ENDIAN_LITTLE, buf) == val,
                     "f32 read LE fail");

    nadine_write_float(NADINE_ENDIAN_BIG, buf, val);
    failed += VERIFY(buf[0] == 0x40 && buf[1] == 0xE0
                  && buf[2] == 0x00 && buf[3] == 0x00,
                  "f32 write BE fail");

    nadine_write_float(NADINE_ENDIAN_LITTLE, buf, val);
    failed += VERIFY(buf[0] == 0x00 && buf[1] == 0x00
                  && buf[2] == 0xE0 && buf[3] == 0x40,
                  "f32 write LE fail");

    return failed;
}

static int test_double(void) {
    int failed = 0;

    double val = 7.0;
    unsigned char buf[sizeof(val)];

    nadine_i_memcpy(buf, &val, sizeof(buf));
    switch (nadine_endian_native_double()) {
    case NADINE_ENDIAN_LITTLE:
        puts("f64 host=LE");
        failed += VERIFY(buf[0] == 0x00 && buf[1] == 0x00
                      && buf[2] == 0x00 && buf[3] == 0x00
                      && buf[4] == 0x00 && buf[5] == 0x00
                      && buf[6] == 0x1C && buf[7] == 0x40,
                      "f64 host=LE mismatch");
        break;
    case NADINE_ENDIAN_BIG:
        puts("f64 host=BE");
        failed += VERIFY(buf[0] == 0x40 && buf[1] == 0x1C
                      && buf[2] == 0x00 && buf[3] == 0x00
                      && buf[4] == 0x00 && buf[5] == 0x00
                      && buf[6] == 0x00 && buf[7] == 0x00,
                      "f64 host=BE mismatch");
        break;
    default:
        puts("f64 host=mixed - bug or rare platform?");
        break;
    }

    buf[0] = 0x40; buf[1] = 0x1C; buf[2] = 0x00; buf[3] = 0x00;
    buf[4] = 0x00; buf[5] = 0x00; buf[6] = 0x00; buf[7] = 0x00;
    failed += VERIFY(nadine_read_double(NADINE_ENDIAN_BIG, buf) == val,
                     "f64 read BE fail");

    buf[0] = 0x00; buf[1] = 0x00; buf[2] = 0x00; buf[3] = 0x00;
    buf[4] = 0x00; buf[5] = 0x00; buf[6] = 0x1C; buf[7] = 0x40;
    failed += VERIFY(nadine_read_double(NADINE_ENDIAN_LITTLE, buf) == val,
                     "f64 read LE fail");

    nadine_write_double(NADINE_ENDIAN_BIG, buf, val);
    failed += VERIFY(buf[0] == 0x40 && buf[1] == 0x1C
                  && buf[2] == 0x00 && buf[3] == 0x00
                  && buf[4] == 0x00 && buf[5] == 0x00
                  && buf[6] == 0x00 && buf[7] == 0x00,
                  "f64 write BE fail");

    nadine_write_double(NADINE_ENDIAN_LITTLE, buf, val);
    failed += VERIFY(buf[0] == 0x00 && buf[1] == 0x00
                  && buf[2] == 0x00 && buf[3] == 0x00
                  && buf[4] == 0x00 && buf[5] == 0x00
                  && buf[6] == 0x1C && buf[7] == 0x40,
                  "f64 write LE fail");

    return failed;
}
#endif

int main(int argc, char *argv[]) {
    int failed = 0;

    failed += test_uint16();
    failed += test_int16();

    failed += test_uint32();
    failed += test_int32();

    failed += test_uint64();
    failed += test_int64();

#if NADINE_FLOAT
    failed += test_float();
    failed += test_double();
#endif

    if (failed) puts("Some tests failed.");
    else        puts("All tests OK.");
    return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
