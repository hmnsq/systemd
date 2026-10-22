/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"
#ಅನ್ತರ್ಭಾವ "stat-util.h"

#define FILESYSTEM_MAGIC_MAX 10

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ FilesystemGroups {
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

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ FilesystemSet {
        ಧ್ರುವ ವರ್ಣ *name;
        ಧ್ರುವ ವರ್ಣ *help;
        ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ;
} FilesystemSet;

ಬಾಹ್ಯ ಧ್ರುವ FilesystemSet filesystem_sets[];

ಧ್ರುವ FilesystemSet *filesystem_set_find(ಧ್ರುವ ವರ್ಣ *name);

ಧ್ರುವ ವರ್ಣ* fs_type_to_string(statfs_f_type_t magic);
ಪೂರ್ಣಾಙ್ಕ fs_type_from_string(ಧ್ರುವ ವರ್ಣ *name, ಧ್ರುವ statfs_f_type_t **ret);
bool fs_in_group(ಧ್ರುವ ಸಂರಚನಾ statfs *s, ಗಣನಾ FilesystemGroups fs_group);

/* gperf prototypes */
ಧ್ರುವ ಸಂರಚನಾ FilesystemMagic* filesystems_gperf_lookup(ಧ್ರುವ ವರ್ಣ *str, GPERF_LEN_TYPE length);
