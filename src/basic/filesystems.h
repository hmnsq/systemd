/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"
#ضمن "stat-util.h"

#define FILESYSTEM_MAGIC_MAX 10

تعريفنوع تعداد FilesystemGroups {
        /* Please leave BASIC_API first and KNOWN last, but sort the rest alphabetically */
        FILESYSTEM_SET_BASIC_API,
        FILESYSTEM_SET_ANONYMOUS,
        FILESYSTEM_SET_APPLICATION,
        FILESYSTEM_SET_AUXILIARY_API,
        FILESYSTEM_SET_COMMON_BLOCK,
        FILESYSTEM_SET_HISTORICAL_BLOCK,
        FILESYSTEM_SET_NETWORK,
        FILESYSTEM_SET_PRIVILEGED_API,
        FILESYSTEM_SET_SECURITY,
        FILESYSTEM_SET_TEMPORARY,
        FILESYSTEM_SET_KNOWN,
        _FILESYSTEM_SET_MAX,
        _FILESYSTEM_SET_INVALID = -EINVAL,
} FilesystemGroups;

تعريفنوع بنية FilesystemSet {
        ثابت حرف *name;
        ثابت حرف *help;
        ثابت حرف *بەها;
} FilesystemSet;

خارجي ثابت FilesystemSet filesystem_sets[];

ثابت FilesystemSet *filesystem_set_find(ثابت حرف *name);

ثابت حرف* fs_type_to_string(statfs_f_type_t magic);
صحيح fs_type_from_string(ثابت حرف *name, ثابت statfs_f_type_t **ret);
bool fs_in_group(ثابت بنية statfs *s, تعداد FilesystemGroups fs_group);

/* gperf prototypes */
ثابت بنية FilesystemMagic* filesystems_gperf_lookup(ثابت حرف *str, GPERF_LEN_TYPE length);
