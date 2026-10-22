/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"
#شامل "stat-util.h"

#define FILESYSTEM_MAGIC_MAX 10

تعریفنوع شمارش FilesystemGroups {
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

تعریفنوع ساختار FilesystemSet {
        ثابت نویسه *name;
        ثابت نویسه *help;
        ثابت نویسه *مقدار;
} FilesystemSet;

بیرونی ثابت FilesystemSet filesystem_sets[];

ثابت FilesystemSet *filesystem_set_find(ثابت نویسه *name);

ثابت نویسه* fs_type_to_string(statfs_f_type_t magic);
صحیح fs_type_from_string(ثابت نویسه *name, ثابت statfs_f_type_t **ret);
bool fs_in_group(ثابت ساختار statfs *s, شمارش FilesystemGroups fs_group);

/* gperf prototypes */
ثابت ساختار FilesystemMagic* filesystems_gperf_lookup(ثابت نویسه *str, GPERF_LEN_TYPE length);
