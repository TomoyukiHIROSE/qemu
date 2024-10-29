/*
 * QEMU memory access test device
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Copyright (c) 2024 IGEL Co., Ltd.
 * Author: Tomoyuki HIROSE <tomoyuki.hirose@igel.co.jp>
 */
/*
 * This device is used to test memory acccess, like:
 *
 *     qemu-system-x86_64 -device memaccess-testdev,address=0x10000000
 */

#include "qemu/osdep.h"
#include "exec/address-spaces.h"
#include "exec/memory.h"
#include "hw/qdev-core.h"
#include "hw/qdev-properties.h"
#include "qapi/error.h"
#include "qemu/typedefs.h"
#include "qom/object.h"

#include "hw/misc/memaccess-testdev.h"

typedef struct MrOpsList {
    const char *name;
    const MemoryRegionOps *ops_array;
    const size_t ops_array_len;
    const size_t offset_idx;
} MrOpsList;

static void testdev_init_memory_region(MemoryRegion *mr,
                                       Object *owner,
                                       const MemoryRegionOps *ops,
                                       void *opaque,
                                       const char *name,
                                       uint64_t size,
                                       MemoryRegion *container,
                                       hwaddr container_offset)
{
    memory_region_init_io(mr, owner, ops, opaque, name, size);
    memory_region_add_subregion(container, container_offset, mr);
}

static void testdev_init_from_mr_ops_list(MemAccessTestDev *testdev,
                                          const MrOpsList *l)
{
    for (size_t i = 0; i < l->ops_array_len; i++) {
        g_autofree gchar *name = g_strdup_printf("%s-%ld", l->name, i);
        testdev_init_memory_region(&testdev->memory_regions[l->offset_idx + i],
                                   OBJECT(testdev), &l->ops_array[i],
                                   testdev->mr_data[l->offset_idx + i],
                                   name,
                                   MEMACCESS_TESTDEV_REGION_SIZE,
                                   &testdev->container,
                                   MEMACCESS_TESTDEV_REGION_SIZE *
                                   (l->offset_idx + i));
    }
}

#define _DEFINE_MR_OPS_LIST(_n, _a, _l, _o)                            \
    {                                                                  \
        .name = (_n),                                                  \
            .ops_array = (_a),                                         \
            .ops_array_len = (_l),                                     \
            .offset_idx = (_o),                                        \
    }

static const MrOpsList mr_ops_list[] = {
    _DEFINE_MR_OPS_LIST("little-b-valid",
                        ops_list_little_b_valid,
                        N_OPS_LIST_LITTLE_B_VALID,
                        OFF_IDX_OPS_LIST_LITTLE_B_VALID),
    _DEFINE_MR_OPS_LIST("little-b-invalid",
                        ops_list_little_b_invalid,
                        N_OPS_LIST_LITTLE_B_INVALID,
                        OFF_IDX_OPS_LIST_LITTLE_B_INVALID),
    _DEFINE_MR_OPS_LIST("little-w-valid",
                        ops_list_little_w_valid,
                        N_OPS_LIST_LITTLE_W_VALID,
                        OFF_IDX_OPS_LIST_LITTLE_W_VALID),
    _DEFINE_MR_OPS_LIST("little-w-invalid",
                        ops_list_little_w_invalid,
                        N_OPS_LIST_LITTLE_W_INVALID,
                        OFF_IDX_OPS_LIST_LITTLE_W_INVALID),
    _DEFINE_MR_OPS_LIST("little-l-valid",
                        ops_list_little_l_valid,
                        N_OPS_LIST_LITTLE_L_VALID,
                        OFF_IDX_OPS_LIST_LITTLE_L_VALID),
    _DEFINE_MR_OPS_LIST("little-l-invalid",
                        ops_list_little_l_invalid,
                        N_OPS_LIST_LITTLE_L_INVALID,
                        OFF_IDX_OPS_LIST_LITTLE_L_INVALID),
    _DEFINE_MR_OPS_LIST("little-q-valid",
                        ops_list_little_q_valid,
                        N_OPS_LIST_LITTLE_Q_VALID,
                        OFF_IDX_OPS_LIST_LITTLE_Q_VALID),
    _DEFINE_MR_OPS_LIST("little-q-invalid",
                        ops_list_little_q_invalid,
                        N_OPS_LIST_LITTLE_Q_INVALID,
                        OFF_IDX_OPS_LIST_LITTLE_Q_INVALID),
    _DEFINE_MR_OPS_LIST("big-b-valid",
                        ops_list_big_b_valid,
                        N_OPS_LIST_BIG_B_VALID,
                        OFF_IDX_OPS_LIST_BIG_B_VALID),
    _DEFINE_MR_OPS_LIST("big-b-invalid",
                        ops_list_big_b_invalid,
                        N_OPS_LIST_BIG_B_INVALID,
                        OFF_IDX_OPS_LIST_BIG_B_INVALID),
    _DEFINE_MR_OPS_LIST("big-w-valid",
                        ops_list_big_w_valid,
                        N_OPS_LIST_BIG_W_VALID,
                        OFF_IDX_OPS_LIST_BIG_W_VALID),
    _DEFINE_MR_OPS_LIST("big-w-invalid",
                        ops_list_big_w_invalid,
                        N_OPS_LIST_BIG_W_INVALID,
                        OFF_IDX_OPS_LIST_BIG_W_INVALID),
    _DEFINE_MR_OPS_LIST("big-l-valid",
                        ops_list_big_l_valid,
                        N_OPS_LIST_LITTLE_L_VALID,
                        OFF_IDX_OPS_LIST_BIG_L_VALID),
    _DEFINE_MR_OPS_LIST("big-l-invalid",
                        ops_list_big_l_invalid,
                        N_OPS_LIST_BIG_L_INVALID,
                        OFF_IDX_OPS_LIST_BIG_L_INVALID),
    _DEFINE_MR_OPS_LIST("big-q-valid",
                        ops_list_big_q_valid,
                        N_OPS_LIST_BIG_Q_VALID,
                        OFF_IDX_OPS_LIST_BIG_Q_VALID),
    _DEFINE_MR_OPS_LIST("big-q-invalid",
                        ops_list_big_q_invalid,
                        N_OPS_LIST_BIG_Q_INVALID,
                        OFF_IDX_OPS_LIST_BIG_Q_INVALID),
};
#define N_MR_OPS_LIST (sizeof(mr_ops_list) / sizeof(MrOpsList))

static void init_testdev(MemAccessTestDev *testdev)
{
    memory_region_init(&testdev->container, OBJECT(testdev), "memtest-regions",
                       MEMACCESS_TESTDEV_REGION_SIZE * N_OPS_LIST);
    testdev->mr_data = g_malloc(MEMACCESS_TESTDEV_MR_DATA_SIZE);

    for (size_t i = 0; i < N_MR_OPS_LIST; i++) {
        testdev_init_from_mr_ops_list(testdev, &mr_ops_list[i]);
    }

    memory_region_add_subregion(get_system_memory(), testdev->base,
                                &testdev->container);
}

static void memaccess_testdev_realize(DeviceState *dev, Error **errp)
{
    MemAccessTestDev *d = MEM_ACCESS_TEST_DEV(dev);

    if (d->base == UINT64_MAX) {
        error_setg(errp, "base address is not assigned");
        return;
    }

    init_testdev(d);
}

static void memaccess_testdev_unrealize(DeviceState *dev)
{
    MemAccessTestDev *d = MEM_ACCESS_TEST_DEV(dev);
    g_free(d->mr_data);
}

static Property memaccess_testdev_props[] = {
    DEFINE_PROP_UINT64("address", MemAccessTestDev, base, UINT64_MAX),
    DEFINE_PROP_END_OF_LIST(),
};

static void memaccess_testdev_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = memaccess_testdev_realize;
    dc->unrealize = memaccess_testdev_unrealize;
    device_class_set_props(dc, memaccess_testdev_props);
    set_bit(DEVICE_CATEGORY_MISC, dc->categories);
}

static const TypeInfo memaccess_testdev_info = {
    .name = TYPE_MEM_ACCESS_TEST_DEV,
    .parent = TYPE_DEVICE,
    .instance_size = sizeof(MemAccessTestDev),
    .class_init = memaccess_testdev_class_init,
};

static void memaccess_testdev_register_types(void)
{
    type_register_static(&memaccess_testdev_info);
}

type_init(memaccess_testdev_register_types)
