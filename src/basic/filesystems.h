/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ "forward.h"
#অন্তর্ভাৱ "stat-util.h"

#define FILESYSTEM_MAGIC_MAX 10

প্রকারনির্দেশ গণনা FilesystemGroups {
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

প্রকারনির্দেশ সংরচনা FilesystemSet {
        ধ্রুৱ ৱর্ণ *name;
        ধ্রুৱ ৱর্ণ *help;
        ধ্রুৱ ৱর্ণ *মান;
} FilesystemSet;

বাহ্য ধ্রুৱ FilesystemSet filesystem_sets[];

ধ্রুৱ FilesystemSet *filesystem_set_find(ধ্রুৱ ৱর্ণ *name);

ধ্রুৱ ৱর্ণ* fs_type_to_string(statfs_f_type_t magic);
পূর্ণাঙ্ক fs_type_from_string(ধ্রুৱ ৱর্ণ *name, ধ্রুৱ statfs_f_type_t **ret);
bool fs_in_group(ধ্রুৱ সংরচনা statfs *s, গণনা FilesystemGroups fs_group);

/* gperf prototypes */
ধ্রুৱ সংরচনা FilesystemMagic* filesystems_gperf_lookup(ধ্রুৱ ৱর্ণ *str, GPERF_LEN_TYPE length);
