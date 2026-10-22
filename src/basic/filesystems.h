/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"
#অন্তর্ভাব "stat-util.h"

#define FILESYSTEM_MAGIC_MAX 10

প্রকারলেখ গণনা FilesystemGroups {
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

প্রকারলেখ কাঠামো FilesystemSet {
        ধ্রুবক অক্ষর *name;
        ধ্রুবক অক্ষর *help;
        ধ্রুবক অক্ষর *মান;
} FilesystemSet;

বাহ্যিক ধ্রুবক FilesystemSet filesystem_sets[];

ধ্রুবক FilesystemSet *filesystem_set_find(ধ্রুবক অক্ষর *name);

ধ্রুবক অক্ষর* fs_type_to_string(statfs_f_type_t magic);
পূর্ণসংখ্যা fs_type_from_string(ধ্রুবক অক্ষর *name, ধ্রুবক statfs_f_type_t **ret);
bool fs_in_group(ধ্রুবক কাঠামো statfs *s, গণনা FilesystemGroups fs_group);

/* gperf prototypes */
ধ্রুবক কাঠামো FilesystemMagic* filesystems_gperf_lookup(ধ্রুবক অক্ষর *str, GPERF_LEN_TYPE length);
