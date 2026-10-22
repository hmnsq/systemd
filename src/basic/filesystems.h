/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"
#include "stat-util.h"

#define FILESYSTEM_MAGIC_MAX 10

typedef ga_nanaa FilesystemGroups {
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

typedef sa_nracanaa FilesystemSet {
        const var_na *name;
        const var_na *help;
        const var_na *value;
} FilesystemSet;

extern const FilesystemSet filesystem_sets[];

const FilesystemSet *filesystem_set_find(const var_na *name);

const var_na* fs_type_to_string(statfs_f_type_t magic);
ចំនួនគត់ fs_type_from_string(const var_na *name, const statfs_f_type_t **ret);
bool fs_in_group(const sa_nracanaa statfs *s, ga_nanaa FilesystemGroups fs_group);

/* gperf prototypes */
const sa_nracanaa FilesystemMagic* filesystems_gperf_lookup(const var_na *str, GPERF_LEN_TYPE length);
