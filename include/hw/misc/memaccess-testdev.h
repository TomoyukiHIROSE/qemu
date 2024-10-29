/*
 * QEMU memory access test device
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Copyright (c) 2024 IGEL Co., Ltd.
 * Author: Tomoyuki HIROSE <tomoyuki.hirose@igel.co.jp>
 */

#ifndef HW_MISC_MEMACCESS_TESTDEV_H
#define HW_MISC_MEMACCESS_TESTDEV_H

#include "qemu/osdep.h"
#include "exec/memory.h"
#include "hw/qdev-core.h"

#define TYPE_MEM_ACCESS_TEST_DEV "memaccess-testdev"
#define MEMACCESS_TESTDEV_REGION_SIZE (32)

static uint64_t memaccess_testdev_read_little(void *opaque, hwaddr addr,
                                              unsigned int size)
{
    g_assert(addr + size < MEMACCESS_TESTDEV_REGION_SIZE);
    void *s = (uint8_t *)opaque + addr;
    return ldn_le_p(s, size);
}

static void memaccess_testdev_write_little(void *opaque, hwaddr addr,
                                           uint64_t data, unsigned int size)
{
    g_assert(addr + size < MEMACCESS_TESTDEV_REGION_SIZE);
    void *d = (uint8_t *)opaque + addr;
    stn_le_p(d, size, data);
}

static uint64_t memaccess_testdev_read_big(void *opaque, hwaddr addr,
                                           unsigned int size)
{
    g_assert(addr + size < MEMACCESS_TESTDEV_REGION_SIZE);
    void *s = (uint8_t *)opaque + addr;
    return ldn_be_p(s, size);
}

static void memaccess_testdev_write_big(void *opaque, hwaddr addr,
                                        uint64_t data, unsigned int size)
{
    g_assert(addr + size < MEMACCESS_TESTDEV_REGION_SIZE);
    void *d = (uint8_t *)opaque + addr;
    stn_be_p(d, size, data);
}

#define __JOIN6(a, b, c, d, e, f) __JOIN6_AGAIN(a, b, c, d, e, f)
#define __JOIN6_AGAIN(a, b, c, d, e, f) a##b##c##d##e##f
#define __JOIN2(a, b) __JOIN2_AGAIN(a, b)
#define __JOIN2_AGAIN(a, b) a##b
#define __STR(x) __STR_AGAIN(x)
#define __STR_AGAIN(x) #x

#define NAME_OPS_LITTLE(valid_max, valid_min, valid_unaligned,          \
                        impl_max, impl_min, impl_unaligned)             \
    __JOIN2(ops_little,                                                 \
            __JOIN6(valid_max, valid_min, valid_unaligned,              \
                    impl_max, impl_min, impl_unaligned))

#define NAME_OPS_BIG(valid_max, valid_min, valid_unaligned,             \
                     impl_max, impl_min, impl_unaligned)                \
    __JOIN2(ops_big,                                                    \
            __JOIN6(valid_max, valid_min, valid_unaligned,              \
                    impl_max, impl_min, impl_unaligned))

#define GEN_OPS_LITTLE(valid_max,                                       \
                       valid_min,                                       \
                       valid_unaligned,                                 \
                       impl_max,                                        \
                       impl_min,                                        \
                       impl_unaligned)                                  \
    static const MemoryRegionOps                                        \
    NAME_OPS_LITTLE(valid_max, valid_min, valid_unaligned,              \
                    impl_max, impl_min, impl_unaligned)                 \
    = {                                                                 \
        .read = memaccess_testdev_read_little,                          \
        .write = memaccess_testdev_write_little,                        \
        .endianness = DEVICE_LITTLE_ENDIAN,                             \
        .valid = {                                                      \
            .max_access_size = valid_max,                               \
            .min_access_size = valid_min,                               \
            .unaligned = valid_unaligned,                               \
        },                                                              \
        .impl = {                                                       \
            .max_access_size = impl_max,                                \
            .min_access_size = impl_min,                                \
            .unaligned = impl_unaligned,                                \
        },                                                              \
    };

#define GEN_OPS_BIG(valid_max,                                          \
                    valid_min,                                          \
                    valid_unaligned,                                    \
                    impl_max,                                           \
                    impl_min,                                           \
                    impl_unaligned)                                     \
    static const MemoryRegionOps                                        \
    NAME_OPS_BIG(valid_max, valid_min, valid_unaligned,                 \
                 impl_max, impl_min, impl_unaligned)                    \
    = {                                                                 \
        .read = memaccess_testdev_read_big,                             \
        .write = memaccess_testdev_write_big,                           \
        .endianness = DEVICE_BIG_ENDIAN,                                \
        .valid = {                                                      \
            .max_access_size = valid_max,                               \
            .min_access_size = valid_min,                               \
            .unaligned = valid_unaligned,                               \
        },                                                              \
        .impl = {                                                       \
            .max_access_size = impl_max,                                \
            .min_access_size = impl_min,                                \
            .unaligned = impl_unaligned,                                \
        },                                                              \
    };

#define APPLY(m, ...)                           \
    m(__VA_ARGS__)

#define APPLY_WITH_COMMA(m, ...)                \
    m(__VA_ARGS__),

#define GEN_PARAM_COMBINATIONS(f, g6, max, min, unaligned)             \
    f(g6, max, min, unaligned, 1, 1,  true)                            \
    f(g6, max, min, unaligned, 2, 1,  true)                            \
    f(g6, max, min, unaligned, 4, 1,  true)                            \
    f(g6, max, min, unaligned, 8, 1,  true)                            \
    f(g6, max, min, unaligned, 2, 2,  true)                            \
    f(g6, max, min, unaligned, 4, 2,  true)                            \
    f(g6, max, min, unaligned, 8, 2,  true)                            \
    f(g6, max, min, unaligned, 4, 4,  true)                            \
    f(g6, max, min, unaligned, 8, 4,  true)                            \
    f(g6, max, min, unaligned, 8, 8,  true)                            \
    f(g6, max, min, unaligned, 1, 1, false)                            \
    f(g6, max, min, unaligned, 2, 1, false)                            \
    f(g6, max, min, unaligned, 4, 1, false)                            \
    f(g6, max, min, unaligned, 8, 1, false)                            \
    f(g6, max, min, unaligned, 2, 2, false)                            \
    f(g6, max, min, unaligned, 4, 2, false)                            \
    f(g6, max, min, unaligned, 8, 2, false)                            \
    f(g6, max, min, unaligned, 4, 4, false)                            \
    f(g6, max, min, unaligned, 8, 4, false)                            \
    f(g6, max, min, unaligned, 8, 8, false)

GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_LITTLE, 1, 1,  true)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_LITTLE, 2, 1,  true)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_LITTLE, 4, 1,  true)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_LITTLE, 8, 1,  true)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_LITTLE, 2, 2,  true)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_LITTLE, 4, 2,  true)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_LITTLE, 8, 2,  true)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_LITTLE, 4, 4,  true)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_LITTLE, 8, 4,  true)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_LITTLE, 8, 8,  true)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_LITTLE, 1, 1, false)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_LITTLE, 2, 1, false)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_LITTLE, 4, 1, false)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_LITTLE, 8, 1, false)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_LITTLE, 2, 2, false)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_LITTLE, 4, 2, false)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_LITTLE, 8, 2, false)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_LITTLE, 4, 4, false)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_LITTLE, 8, 4, false)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_LITTLE, 8, 8, false)

GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_BIG, 1, 1,  true)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_BIG, 2, 1,  true)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_BIG, 4, 1,  true)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_BIG, 8, 1,  true)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_BIG, 2, 2,  true)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_BIG, 4, 2,  true)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_BIG, 8, 2,  true)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_BIG, 4, 4,  true)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_BIG, 8, 4,  true)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_BIG, 8, 8,  true)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_BIG, 1, 1, false)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_BIG, 2, 1, false)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_BIG, 4, 1, false)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_BIG, 8, 1, false)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_BIG, 2, 2, false)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_BIG, 4, 2, false)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_BIG, 8, 2, false)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_BIG, 4, 4, false)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_BIG, 8, 4, false)
GEN_PARAM_COMBINATIONS(APPLY, GEN_OPS_BIG, 8, 8, false)

static const MemoryRegionOps ops_list_little_b_valid[] = {
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_LITTLE, 1, 1,  true)
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_LITTLE, 2, 1,  true)
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_LITTLE, 4, 1,  true)
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_LITTLE, 8, 1,  true)
};

static const MemoryRegionOps ops_list_little_b_invalid[] = {
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_LITTLE, 1, 1, false)
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_LITTLE, 2, 1, false)
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_LITTLE, 4, 1, false)
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_LITTLE, 8, 1, false)
};

static const MemoryRegionOps ops_list_little_w_valid[] = {
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_LITTLE, 2, 2,  true)
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_LITTLE, 4, 2,  true)
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_LITTLE, 8, 2,  true)
};

static const MemoryRegionOps ops_list_little_w_invalid[] = {
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_LITTLE, 2, 2, false)
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_LITTLE, 4, 2, false)
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_LITTLE, 8, 2, false)
};

static const MemoryRegionOps ops_list_little_l_valid[] = {
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_LITTLE, 4, 4,  true)
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_LITTLE, 8, 4,  true)
};

static const MemoryRegionOps ops_list_little_l_invalid[] = {
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_LITTLE, 4, 4, false)
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_LITTLE, 8, 4, false)
};

static const MemoryRegionOps ops_list_little_q_valid[] = {
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_LITTLE, 8, 8,  true)
};

static const MemoryRegionOps ops_list_little_q_invalid[] = {
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_LITTLE, 8, 8, false)
};

static const MemoryRegionOps ops_list_big_b_valid[] = {
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_BIG, 1, 1,  true)
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_BIG, 2, 1,  true)
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_BIG, 4, 1,  true)
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_BIG, 8, 1,  true)
};

static const MemoryRegionOps ops_list_big_b_invalid[] = {
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_BIG, 1, 1, false)
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_BIG, 2, 1, false)
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_BIG, 4, 1, false)
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_BIG, 8, 1, false)
};

static const MemoryRegionOps ops_list_big_w_valid[] = {
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_BIG, 2, 2,  true)
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_BIG, 4, 2,  true)
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_BIG, 8, 2,  true)
};

static const MemoryRegionOps ops_list_big_w_invalid[] = {
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_BIG, 2, 2, false)
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_BIG, 4, 2, false)
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_BIG, 8, 2, false)
};

static const MemoryRegionOps ops_list_big_l_valid[] = {
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_BIG, 4, 4,  true)
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_BIG, 8, 4,  true)
};

static const MemoryRegionOps ops_list_big_l_invalid[] = {
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_BIG, 4, 4, false)
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_BIG, 8, 4, false)
};

static const MemoryRegionOps ops_list_big_q_valid[] = {
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_BIG, 8, 8,  true)
};

static const MemoryRegionOps ops_list_big_q_invalid[] = {
    GEN_PARAM_COMBINATIONS(APPLY_WITH_COMMA, NAME_OPS_BIG, 8, 8, false)
};

#define N_OPS_LIST_LITTLE_B_VALID \
    (sizeof(ops_list_little_b_valid) / sizeof(MemoryRegionOps))
#define N_OPS_LIST_LITTLE_W_VALID \
    (sizeof(ops_list_little_w_valid) / sizeof(MemoryRegionOps))
#define N_OPS_LIST_LITTLE_L_VALID \
    (sizeof(ops_list_little_l_valid) / sizeof(MemoryRegionOps))
#define N_OPS_LIST_LITTLE_Q_VALID \
    (sizeof(ops_list_little_q_valid) / sizeof(MemoryRegionOps))
#define N_OPS_LIST_LITTLE_B_INVALID \
    (sizeof(ops_list_little_b_invalid) / sizeof(MemoryRegionOps))
#define N_OPS_LIST_LITTLE_W_INVALID \
    (sizeof(ops_list_little_w_invalid) / sizeof(MemoryRegionOps))
#define N_OPS_LIST_LITTLE_L_INVALID \
    (sizeof(ops_list_little_l_invalid) / sizeof(MemoryRegionOps))
#define N_OPS_LIST_LITTLE_Q_INVALID \
    (sizeof(ops_list_little_q_invalid) / sizeof(MemoryRegionOps))
#define N_OPS_LIST_BIG_B_VALID \
    (sizeof(ops_list_big_b_valid) / sizeof(MemoryRegionOps))
#define N_OPS_LIST_BIG_W_VALID \
    (sizeof(ops_list_big_w_valid) / sizeof(MemoryRegionOps))
#define N_OPS_LIST_BIG_L_VALID \
    (sizeof(ops_list_big_l_valid) / sizeof(MemoryRegionOps))
#define N_OPS_LIST_BIG_Q_VALID \
    (sizeof(ops_list_big_q_valid) / sizeof(MemoryRegionOps))
#define N_OPS_LIST_BIG_B_INVALID \
    (sizeof(ops_list_big_b_invalid) / sizeof(MemoryRegionOps))
#define N_OPS_LIST_BIG_W_INVALID \
    (sizeof(ops_list_big_w_invalid) / sizeof(MemoryRegionOps))
#define N_OPS_LIST_BIG_L_INVALID \
    (sizeof(ops_list_big_l_invalid) / sizeof(MemoryRegionOps))
#define N_OPS_LIST_BIG_Q_INVALID \
    (sizeof(ops_list_big_q_invalid) / sizeof(MemoryRegionOps))

#define N_OPS_LIST                              \
    (N_OPS_LIST_LITTLE_B_VALID +                \
     N_OPS_LIST_LITTLE_B_INVALID +              \
     N_OPS_LIST_LITTLE_W_VALID +                \
     N_OPS_LIST_LITTLE_W_INVALID +              \
     N_OPS_LIST_LITTLE_L_VALID +                \
     N_OPS_LIST_LITTLE_L_INVALID +              \
     N_OPS_LIST_LITTLE_Q_VALID +                \
     N_OPS_LIST_LITTLE_Q_INVALID +              \
     N_OPS_LIST_BIG_B_VALID +                   \
     N_OPS_LIST_BIG_B_INVALID +                 \
     N_OPS_LIST_BIG_W_VALID +                   \
     N_OPS_LIST_BIG_W_INVALID +                 \
     N_OPS_LIST_BIG_L_VALID +                   \
     N_OPS_LIST_BIG_L_INVALID +                 \
     N_OPS_LIST_BIG_Q_VALID +                   \
     N_OPS_LIST_BIG_Q_INVALID)

#define OFF_IDX_OPS_LIST_LITTLE_B_VALID \
    (0)
#define OFF_IDX_OPS_LIST_LITTLE_B_INVALID \
    (OFF_IDX_OPS_LIST_LITTLE_B_VALID + N_OPS_LIST_LITTLE_B_VALID)
#define OFF_IDX_OPS_LIST_LITTLE_W_VALID \
    (OFF_IDX_OPS_LIST_LITTLE_B_INVALID + N_OPS_LIST_LITTLE_B_INVALID)
#define OFF_IDX_OPS_LIST_LITTLE_W_INVALID \
    (OFF_IDX_OPS_LIST_LITTLE_W_VALID + N_OPS_LIST_LITTLE_W_VALID)
#define OFF_IDX_OPS_LIST_LITTLE_L_VALID \
    (OFF_IDX_OPS_LIST_LITTLE_W_INVALID + N_OPS_LIST_LITTLE_W_INVALID)
#define OFF_IDX_OPS_LIST_LITTLE_L_INVALID \
    (OFF_IDX_OPS_LIST_LITTLE_L_VALID + N_OPS_LIST_LITTLE_L_VALID)
#define OFF_IDX_OPS_LIST_LITTLE_Q_VALID \
    (OFF_IDX_OPS_LIST_LITTLE_L_INVALID + N_OPS_LIST_LITTLE_L_INVALID)
#define OFF_IDX_OPS_LIST_LITTLE_Q_INVALID \
    (OFF_IDX_OPS_LIST_LITTLE_Q_VALID + N_OPS_LIST_LITTLE_Q_VALID)
#define OFF_IDX_OPS_LIST_BIG_B_VALID \
    (OFF_IDX_OPS_LIST_LITTLE_Q_INVALID + N_OPS_LIST_LITTLE_Q_INVALID)
#define OFF_IDX_OPS_LIST_BIG_B_INVALID \
    (OFF_IDX_OPS_LIST_BIG_B_VALID + N_OPS_LIST_BIG_B_VALID)
#define OFF_IDX_OPS_LIST_BIG_W_VALID \
    (OFF_IDX_OPS_LIST_BIG_B_INVALID + N_OPS_LIST_BIG_B_INVALID)
#define OFF_IDX_OPS_LIST_BIG_W_INVALID \
    (OFF_IDX_OPS_LIST_BIG_W_VALID + N_OPS_LIST_BIG_W_VALID)
#define OFF_IDX_OPS_LIST_BIG_L_VALID \
    (OFF_IDX_OPS_LIST_BIG_W_INVALID + N_OPS_LIST_BIG_W_INVALID)
#define OFF_IDX_OPS_LIST_BIG_L_INVALID \
    (OFF_IDX_OPS_LIST_BIG_L_VALID + N_OPS_LIST_BIG_L_VALID)
#define OFF_IDX_OPS_LIST_BIG_Q_VALID \
    (OFF_IDX_OPS_LIST_BIG_L_INVALID + N_OPS_LIST_BIG_L_INVALID)
#define OFF_IDX_OPS_LIST_BIG_Q_INVALID \
    (OFF_IDX_OPS_LIST_BIG_Q_VALID + N_OPS_LIST_BIG_Q_VALID)

typedef uint8_t MrData[MEMACCESS_TESTDEV_REGION_SIZE];
#define MEMACCESS_TESTDEV_MR_DATA_SIZE (sizeof(MrData) * N_OPS_LIST)

typedef DeviceClass MemAccessTestDevClass;
typedef struct MemAccessTestDev {
    /* Private */
    DeviceState parent_obj;
    /* Public */
    MemoryRegion container;
    MemoryRegion memory_regions[N_OPS_LIST];
    uint64_t base;
    MrData *mr_data;
} MemAccessTestDev;

#define MEM_ACCESS_TEST_DEV_GET_CLASS(obj)                              \
    OBJECT_GET_CLASS(MemAccessTestDevClass, obj, TYPE_MEM_ACCESS_TEST_DEV)
#define MEM_ACCESS_TEST_DEV_CLASS(klass)                                \
    OBJECT_CLASS_CHECK(MemAccessTestDevClass, klass, TYPE_MEM_ACCESS_TEST_DEV)
#define MEM_ACCESS_TEST_DEV(obj)                                    \
    OBJECT_CHECK(MemAccessTestDev, obj, TYPE_MEM_ACCESS_TEST_DEV)

#undef GEN_OPS_LITTLE
#undef GEN_OPS_BIG
#undef NAME_OPS_LITTLE
#undef NAME_OPS_BIG
#undef APPLY
#undef APPLY_WITH_COMMA
#undef GEN_PARAM_COMBINATIONS
#undef __JOIN2
#undef __JOIN2_AGAIN
#undef __JOIN6
#undef __JOIN6_AGAIN
#undef __STR
#undef __STR_AGAIN

#endif
